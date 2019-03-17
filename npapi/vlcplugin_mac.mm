/*****************************************************************************
 * vlcplugin_mac.cpp: a VLC plugin for Mozilla (Mac interface)
 *****************************************************************************
 * Copyright (C) 2011-2015 VLC Authors and VideoLAN
 * $Id$
 *
 * Authors: Felix Paul Kühne <fkuehne # videolan # org>
 *          Cheng Sun <chengsun9@gmail.com>
 *          Jean-Baptiste Kempf <jb@videolan.org>
 *          James Bates <james.h.bates@gmail.com>
 *          Pierre d'Herbemont <pdherbemont # videolan.org>
 *          David Fuhrmann <david dot fuhrmann at googlemail dot com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#pragma mark includes and fix-ups

#import "vlcplugin_mac.h"
#import <npapi.h>

#import <QuartzCore/QuartzCore.h>
#import <AppKit/AppKit.h>

/* compilation support for 10.6 */
#define OSX_LION NSAppKitVersionNumber >= 1115.2
#ifndef MAC_OS_X_VERSION_10_7

@interface NSView (IntroducedInLion)
- (NSRect)convertRectToBacking:(NSRect)aRect;
@end

#endif

#pragma mark - prototypes

CGImageRef createImageNamed(NSString *);

#pragma mark - objc class interfaces

@interface VLCNoMediaLayer : CALayer
{
    VlcPluginMac *_cppPlugin;
}

@property (readwrite) VlcPluginMac *cppPlugin;

@end

@interface VLCBrowserRootLayer : CALayer {
    NSTimer *_interfaceUpdateTimer;
    VlcPluginMac *_cppPlugin;
}

@property (readwrite) VlcPluginMac *cppPlugin;

- (void)startUIUpdateTimer;

- (void)addVoutLayer:(CALayer *)aLayer;
- (void)removeVoutLayer:(CALayer *)aLayer;
- (CGSize)currentOutputSize;
@end

@interface VLCControllerLayer : CALayer {
    CGImageRef _playImage;
    CGImageRef _pauseImage;

    CGImageRef _sliderTrackLeft;
    CGImageRef _sliderTrackRight;
    CGImageRef _sliderTrackCenter;

    CGImageRef _enterFullscreen;
    CGImageRef _leaveFullscreen;

    CGImageRef _knob;

    BOOL _wasPlayingBeforeMouseDown;
    BOOL _isScrubbing;
    CGFloat _mouseDownXDelta;

    double _mediaPosition;
    BOOL _isPlaying;
    BOOL _isFullscreen;
    VlcPluginMac *_cppPlugin;
}
@property (readwrite) double mediaPosition;
@property (readwrite) BOOL isPlaying;
@property (readwrite) BOOL isFullscreen;
@property (readwrite) VlcPluginMac *cppPlugin;

- (void)handleMouseDown:(CGPoint)point;
- (void)handleMouseUp:(CGPoint)point;
- (void)handleMouseDragged:(CGPoint)point;

@end

@interface VLCControllerLayer (Internal)
- (CGRect)_playPauseButtonRect;
- (CGRect)_fullscreenButtonRect;
- (CGRect)_sliderRect;
@end

@interface VLCPlaybackLayer : CALayer
- (void)mouseButtonDown:(int)buttonNumber;
- (void)mouseButtonUp:(int)buttonNumber;
- (void)mouseMovedToX:(double)xValue Y:(double)yValue;
@end

@interface VLCFullscreenContentView : NSView {
    VlcPluginMac *_cppPlugin;
    NSTimeInterval _timeSinceLastMouseMove;
}
@property (readwrite) VlcPluginMac *cppPlugin;

- (void)hideToolbar;

@end

@interface VLCFullscreenWindow : NSWindow {
    NSRect _initialFrame;
    VLCFullscreenContentView *_customContentView;
}
@property (readonly) VLCFullscreenContentView *customContentView;

- (id)initWithContentRect:(NSRect)contentRect;

@end

@interface NSScreen (VLCAdditions)
- (BOOL)hasMenuBar;
- (BOOL)hasDock;
- (CGDirectDisplayID)displayID;
@end

@interface VLCPerInstanceStorage : NSObject
{
    VlcPluginMac *_cppPlugin;
    VLCBrowserRootLayer *_browserRootLayer;
    VLCPlaybackLayer *_playbackLayer;
    VLCNoMediaLayer *_noMediaLayer;
    VLCControllerLayer *_controllerLayer;
    VLCFullscreenWindow *_fullscreenWindow;
    VLCFullscreenContentView *_fullscreenView;
}

@property (readwrite, assign) VlcPluginMac *cppPlugin;
@property (readwrite, retain) VLCBrowserRootLayer *browserRootLayer;
@property (readwrite, retain) VLCPlaybackLayer *playbackLayer;
@property (readwrite, retain) VLCNoMediaLayer *noMediaLayer;
@property (readwrite, retain) VLCControllerLayer *controllerLayer;
@property (readwrite, retain) VLCFullscreenWindow *fullscreenWindow;
@property (readwrite, retain) VLCFullscreenContentView *fullscreenView;

@end

@implementation VLCPerInstanceStorage

@synthesize cppPlugin = _cppPlugin, browserRootLayer = _browserRootLayer, playbackLayer = _playbackLayer, noMediaLayer = _noMediaLayer, controllerLayer = _controllerLayer, fullscreenWindow = _fullscreenWindow, fullscreenView = _fullscreenView;

@end

#pragma mark - handling of c++ bindings

VlcPluginMac::VlcPluginMac(NPP instance, NPuint16_t mode) :
    VlcPluginBase(instance, mode)
{
    _perInstanceStorage = [[VLCPerInstanceStorage alloc] init];
    [(VLCPerInstanceStorage *)_perInstanceStorage setCppPlugin: this];
}

VlcPluginMac::~VlcPluginMac()
{
    [(VLCPerInstanceStorage *)_perInstanceStorage release];
}

void VlcPluginMac::set_player_window()
{
    /* pass base layer to libvlc to pass it on to the vout */
    libvlc_media_player_set_nsobject(getMD(), [(VLCPerInstanceStorage *)this->_perInstanceStorage browserRootLayer]);
}

void VlcPluginMac::toggle_fullscreen()
{
    if (!get_options().get_enable_fs())
        return;
    libvlc_toggle_fullscreen(getMD());
    this->update_controls();

    if (get_fullscreen() != 0) {
        /* this window is kind of useless. however, we need to support 10.5, since enterFullScreenMode depends on the
         * existance of a parent window. This is solved in 10.6 and we should remove the window once we require it. */
        [(VLCPerInstanceStorage *)this->_perInstanceStorage setFullscreenWindow:[[VLCFullscreenWindow alloc] initWithContentRect: NSMakeRect(0., 0., npwindow.width, npwindow.height)]];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenWindow] setLevel:CGShieldingWindowLevel()];
        [(VLCPerInstanceStorage *)this->_perInstanceStorage setFullscreenView:[[(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenWindow] customContentView]];

        /* CAVE: the order of these methods is important, since we want a layer-hosting view instead of
         * a layer-backed view, which you'd get if you do it the other way around */
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenView] setLayer:[CALayer layer]];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenView] setWantsLayer:YES];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenView] setCppPlugin:this];

        [[(VLCPerInstanceStorage *)this->_perInstanceStorage noMediaLayer] removeFromSuperlayer];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer] removeFromSuperlayer];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer] removeFromSuperlayer];

        if ([(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenView] == nil)
            return;
        if ([(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenView].layer == nil)
            return;

        [[(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenView].layer addSublayer: [(VLCPerInstanceStorage *)this->_perInstanceStorage noMediaLayer]];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenView].layer addSublayer: [(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer]];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenView].layer addSublayer: [(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer]];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenView].layer setNeedsDisplay];

        [[(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenWindow].contentView enterFullScreenMode: [NSScreen mainScreen] withOptions: [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithInt: 0], NSFullScreenModeAllScreens, nil]];

        [[(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenView] performSelector:@selector(hideToolbar) withObject:nil afterDelay: 4.1];
    } else {
        if (![(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenWindow])
            return;
        if (![(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenWindow].contentView)
            return;

        [[(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenWindow].contentView exitFullScreenModeWithOptions: nil];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage noMediaLayer] removeFromSuperlayer];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer] removeFromSuperlayer];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer] removeFromSuperlayer];

        [[(VLCPerInstanceStorage *)this->_perInstanceStorage browserRootLayer] addSublayer: [(VLCPerInstanceStorage *)this->_perInstanceStorage noMediaLayer]];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage browserRootLayer] addSublayer: [(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer]];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage browserRootLayer] addSublayer: [(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer]];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage fullscreenWindow] orderOut: nil];
        [(VLCPerInstanceStorage *)this->_perInstanceStorage setFullscreenWindow: nil];
    }
}

void VlcPluginMac::set_fullscreen(int i_value)
{
    if (!get_options().get_enable_fs())
        return;
    libvlc_set_fullscreen(getMD(), i_value);
    this->update_controls();
}

int  VlcPluginMac::get_fullscreen()
{
    return libvlc_get_fullscreen(getMD());
}

void VlcPluginMac::set_toolbar_visible(bool b_value)
{
    if (!get_options().get_show_toolbar()) {
        [(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer].hidden = YES;
        return;
    }
    [(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer].hidden = !b_value;
}

bool VlcPluginMac::get_toolbar_visible()
{
    return [(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer].isHidden;
}

void VlcPluginMac::update_controls()
{
    libvlc_state_t currentstate = libvlc_media_player_get_state(getMD());
    if (currentstate == libvlc_Playing || currentstate == libvlc_Paused || currentstate == libvlc_Opening) {
        [(VLCPerInstanceStorage *)this->_perInstanceStorage noMediaLayer].hidden = YES;
        [(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer].hidden = NO;
    } else {
        [(VLCPerInstanceStorage *)this->_perInstanceStorage noMediaLayer].hidden = NO;
        [(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer].hidden = YES;
    }

    if ([(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer] != nil) {
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer] setMediaPosition: libvlc_media_player_get_position(getMD())];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer] setIsPlaying: playlist_isplaying()];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer] setIsFullscreen:this->get_fullscreen()];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer] setNeedsDisplay];
    }
}

bool VlcPluginMac::create_windows()
{
    return true;
}

bool VlcPluginMac::resize_windows()
{
    return true;
}

bool VlcPluginMac::destroy_windows()
{
    npwindow.window = NULL;
    return true;
}

NPError VlcPluginMac::get_root_layer(void *value)
{
    if ([(VLCPerInstanceStorage *)this->_perInstanceStorage browserRootLayer] == nil) {
        [(VLCPerInstanceStorage *)this->_perInstanceStorage setBrowserRootLayer:[[VLCBrowserRootLayer alloc] init]];
        [(VLCPerInstanceStorage *)this->_perInstanceStorage browserRootLayer].cppPlugin = this;

        const char *userAgent = NPN_UserAgent(this->getBrowser());
        if (strstr(userAgent, "Safari") && strstr(userAgent, "Version/5")) {
            NSLog(@"Safari 5 detected, deploying UI update timer");
            [[(VLCPerInstanceStorage *)this->_perInstanceStorage browserRootLayer] performSelector:@selector(startUIUpdateTimer) withObject:nil afterDelay:1.];
        } else if (strstr(userAgent, "Firefox"))
            this->runningWithinFirefox = true;

        [(VLCPerInstanceStorage *)this->_perInstanceStorage setNoMediaLayer:[[VLCNoMediaLayer alloc] init]];
        [(VLCPerInstanceStorage *)this->_perInstanceStorage noMediaLayer].opaque = 1.;
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage noMediaLayer] setCppPlugin:this];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage browserRootLayer] addSublayer:[(VLCPerInstanceStorage *)this->_perInstanceStorage noMediaLayer]];

        [(VLCPerInstanceStorage *)this->_perInstanceStorage setControllerLayer:[[VLCControllerLayer alloc] init]];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage browserRootLayer] addSublayer:[(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer]];
        [[(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer] setCppPlugin:this];

        [[(VLCPerInstanceStorage *)this->_perInstanceStorage browserRootLayer] setNeedsDisplay];
    }

    *(CALayer **)value = [(VLCPerInstanceStorage *)this->_perInstanceStorage browserRootLayer];
    return NPERR_NO_ERROR;
}

bool VlcPluginMac::handle_event(void *event)
{
    NPCocoaEvent* cocoaEvent = (NPCocoaEvent*)event;

    if (!event)
        return false;

    NPCocoaEventType eventType = cocoaEvent->type;

    switch (eventType) {
        case NPCocoaEventMouseDown:
        {
            if ([(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer] != nil) {
                if ([[(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer] respondsToSelector:@selector(mouseButtonDown:)])
                    [[(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer] mouseButtonDown:cocoaEvent->data.mouse.buttonNumber];
            }
            if (cocoaEvent->data.mouse.clickCount >= 2)
                this->toggle_fullscreen();

            CGPoint point = CGPointMake(cocoaEvent->data.mouse.pluginX,
                                        // Flip the y coordinate
                                        npwindow.height - cocoaEvent->data.mouse.pluginY);
            if ([(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer] != nil)
                [[(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer] handleMouseDown:[[(VLCPerInstanceStorage *)this->_perInstanceStorage browserRootLayer] convertPoint:point toLayer:[(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer]]];

            return true;
        }
        case NPCocoaEventMouseUp:
        {
            if ([(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer] != nil) {
                if ([[(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer] respondsToSelector:@selector(mouseButtonUp:)])
                    [[(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer] mouseButtonUp:cocoaEvent->data.mouse.buttonNumber];
            }
            CGPoint point = CGPointMake(cocoaEvent->data.mouse.pluginX,
                                        // Flip the y coordinate
                                        npwindow.height - cocoaEvent->data.mouse.pluginY);

            if ([(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer] != nil)
                [[(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer] handleMouseUp:[[(VLCPerInstanceStorage *)this->_perInstanceStorage browserRootLayer] convertPoint:point toLayer:[(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer]]];

            return true;
        }
        case NPCocoaEventMouseMoved:
        {
            if ([(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer] != nil) {
                if ([[(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer] respondsToSelector:@selector(mouseMovedToX:Y:)])
                    [[(VLCPerInstanceStorage *)this->_perInstanceStorage playbackLayer] mouseMovedToX:cocoaEvent->data.mouse.pluginX Y:cocoaEvent->data.mouse.pluginY];
            }
        }
        case NPCocoaEventMouseDragged:
        {
            CGPoint point = CGPointMake(cocoaEvent->data.mouse.pluginX,
                                        // Flip the y coordinate
                                        npwindow.height - cocoaEvent->data.mouse.pluginY);

            if ([(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer] != nil)
                [[(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer] handleMouseDragged:[[(VLCPerInstanceStorage *)this->_perInstanceStorage browserRootLayer] convertPoint:point toLayer:[(VLCPerInstanceStorage *)this->_perInstanceStorage controllerLayer]]];

            return true;
        }
        case NPCocoaEventMouseEntered:
        {
            this->set_toolbar_visible(true);
            return true;
        }
        case NPCocoaEventMouseExited:
        {
            this->set_toolbar_visible(false);
            return true;
        }
        case NPCocoaEventKeyDown:
        {
            if (cocoaEvent->data.key.keyCode == 53) {
                toggle_fullscreen();
                return true;
            } else if (cocoaEvent->data.key.keyCode == 49) {
                playlist_togglePause();
                return true;
            }
        }
        case NPCocoaEventKeyUp:
        case NPCocoaEventFocusChanged:
        case NPCocoaEventScrollWheel:
            return true;

        default:
            break;
    }

    if (eventType == NPCocoaEventDrawRect) {
        /* even though we are using the CoreAnimation drawing model
         * this can be called by the browser, especially when doing
         * screenshots.
         * Since speed isn't important in this case, we could fetch
         * fetch the current frame from libvlc and render it as an
         * image.
         * However, for sakes of simplicity, just show a black
         * rectancle for now. */
        CGContextRef cgContext = cocoaEvent->data.draw.context;
        if (!cgContext) {
            return false;
        }

        float windowWidth = npwindow.width;
        float windowHeight = npwindow.height;

        CGContextSaveGState(cgContext);

        // this context is flipped..
        CGContextTranslateCTM(cgContext, 0.0, windowHeight);
        CGContextScaleCTM(cgContext, 1., -1.);

        // draw black rectancle
        CGContextAddRect(cgContext, CGRectMake(0, 0, windowWidth, windowHeight));
        CGContextSetGrayFillColor(cgContext, 0., 1.);
        CGContextDrawPath(cgContext, kCGPathFill);

        CGContextRestoreGState(cgContext);

        return true;
    }

    return VlcPluginBase::handle_event(event);
}

#pragma mark - objc class implementations

@implementation VLCBrowserRootLayer

@synthesize cppPlugin = _cppPlugin;

- (id)init
{
    if (self = [super init]) {
        self.needsDisplayOnBoundsChange = YES;
        self.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
    }

    return self;
}

- (void)startUIUpdateTimer
{
    _interfaceUpdateTimer = [NSTimer scheduledTimerWithTimeInterval:0.3 target:self selector:@selector(_updateUI) userInfo:nil repeats:YES];
    [_interfaceUpdateTimer retain];
    [_interfaceUpdateTimer fire];
}

- (void)dealloc
{
    if (_interfaceUpdateTimer) {
        [_interfaceUpdateTimer invalidate];
        [_interfaceUpdateTimer release];
    }

    [super dealloc];
}

- (void)_updateUI
{
    if (_cppPlugin)
        _cppPlugin->update_controls();
}

- (void)addVoutLayer:(CALayer *)aLayer
{
    [CATransaction begin];
    VLCPlaybackLayer *playbackLayer = (VLCPlaybackLayer *)[aLayer retain];
    playbackLayer.opaque = 1.;
    playbackLayer.hidden = NO;

    if (libvlc_get_fullscreen(_cppPlugin->getMD()) != 0) {
        /* work-around a 32bit runtime limitation where we can't cast
         * NSRect to CGRect */
        NSRect fullscreenViewFrame = [(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage fullscreenView].frame;
        playbackLayer.bounds = CGRectMake(fullscreenViewFrame.origin.x,
                                          fullscreenViewFrame.origin.y,
                                          fullscreenViewFrame.size.width,
                                          fullscreenViewFrame.size.height);
        playbackLayer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
        [(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage setPlaybackLayer: playbackLayer];

        [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage controllerLayer] removeFromSuperlayer];
        [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage fullscreenView].layer addSublayer: [(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer]];
        [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage fullscreenView].layer addSublayer: [(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage controllerLayer]];
        [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage fullscreenView].layer setNeedsDisplay];
    } else {
        [(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage setPlaybackLayer: playbackLayer];
        [(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer].bounds = [(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage noMediaLayer].bounds;
        [self insertSublayer:[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] below:[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage controllerLayer]];
    }
    [self setNeedsDisplay];
    [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] setNeedsDisplay];
    CGRect frame = [(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer].bounds;
    frame.origin.x = 0.;
    frame.origin.y = 0.;
    [(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer].frame = frame;
    [CATransaction commit];
}

- (void)removeVoutLayer:(CALayer *)aLayer
{
    [CATransaction begin];
    [aLayer removeFromSuperlayer];
    [CATransaction commit];

    if ([(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] == aLayer)
        [(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage setPlaybackLayer:nil];
}

- (CGSize)currentOutputSize
{
    return [(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage browserRootLayer].visibleRect.size;
}

@end

@implementation VLCNoMediaLayer

@synthesize cppPlugin = _cppPlugin;

- (id)init
{
    if (self = [super init]) {
        self.needsDisplayOnBoundsChange = YES;
        self.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
    }

    return self;
}

- (void)drawInContext:(CGContextRef)cgContext
{
    float windowWidth = self.visibleRect.size.width;
    float windowHeight = self.visibleRect.size.height;

    CGContextSaveGState(cgContext);

    CGColorRef backgroundColor;
    unsigned r = 0, g = 0, b = 0;
    HTMLColor2RGB(self.cppPlugin->get_options().get_bg_color().c_str(), &r, &g, &b);
    backgroundColor = CGColorCreateGenericRGB(r, g, b, 1.);

    if (self.cppPlugin->get_options().get_enable_branding()) {
        // draw background
        CGContextAddRect(cgContext, CGRectMake(0, 0, windowWidth, windowHeight));
        CGContextSetFillColorWithColor(cgContext, backgroundColor);
        CGContextDrawPath(cgContext, kCGPathFill);

        // draw gradient
        CGImageRef gradient = createImageNamed(@"gradient");
        CGContextDrawImage(cgContext, CGRectMake(0., 0., windowWidth, 150.), gradient);
        CGImageRelease(gradient);

        // draw info text
        CGContextSetGrayStrokeColor(cgContext, .95, 1.);
        CGContextSetTextDrawingMode(cgContext, kCGTextFill);
        CGContextSetGrayFillColor(cgContext, 1., 1.);
        CFStringRef keys[2];
        keys[0] = kCTFontAttributeName;
        keys[1] = kCTForegroundColorFromContextAttributeName;
        CFTypeRef values[2];
        values[0] = CTFontCreateWithName(CFSTR("HelveticaNeue-Light"),18,NULL);
        values[1] = kCFBooleanTrue;
        CFDictionaryRef stylesDict = CFDictionaryCreate(kCFAllocatorDefault,
                                                        (const void **)&keys,
                                                        (const void **)&values,
                                                        2, NULL, NULL);
        CFAttributedStringRef attRef = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("VLC Web Plugin"), stylesDict);
        CTLineRef textLine = CTLineCreateWithAttributedString(attRef);
        CGContextSetTextPosition(cgContext, 25., 60.);
        CTLineDraw(textLine, cgContext);
        CFRelease(textLine);
        CFRelease(attRef);

        // print smaller text from here
        CFRelease(stylesDict);
        values[0] = CTFontCreateWithName(CFSTR("Helvetica"),12,NULL);
        stylesDict = CFDictionaryCreate(kCFAllocatorDefault,
                                        (const void **)&keys,
                                        (const void **)&values,
                                        2, NULL, NULL);

        // draw version string
        CFStringRef arch;
    #ifdef __x86_64__
        arch = CFSTR("64-bit");
    #else
        arch = CFSTR("32-bit");
    #endif

        attRef = CFAttributedStringCreate(kCFAllocatorDefault, CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%s — windowed mode — %@"), libvlc_get_version(), arch), stylesDict);
        textLine = CTLineCreateWithAttributedString(attRef);
        CGContextSetTextPosition(cgContext, 25., 40.);
        CTLineDraw(textLine, cgContext);
        CFRelease(textLine);
        CFRelease(attRef);
        CFRelease(stylesDict);

        // draw cone
        CGImageRef cone = createImageNamed(@"cone");
        CGFloat coneWidth = CGImageGetWidth(cone);
        CGFloat coneHeight = CGImageGetHeight(cone);
        if (windowHeight <= 320.) {
            coneWidth = coneWidth / 2.;
            coneHeight = coneHeight / 2.;
        }
        CGContextDrawImage(cgContext, CGRectMake((windowWidth - coneWidth) / 2., (windowHeight - coneHeight) / 2., coneWidth, coneHeight), cone);
        CGImageRelease(cone);

        // draw custom text
        values[0] = CTFontCreateWithName(CFSTR("Helvetica"),14,NULL);
        stylesDict = CFDictionaryCreate(kCFAllocatorDefault,
                                        (const void **)&keys,
                                        (const void **)&values,
                                        2, NULL, NULL);
        const char *text = self.cppPlugin->get_options().get_bg_text().c_str();
        if (text != NULL) {
            attRef = CFAttributedStringCreate(kCFAllocatorDefault, CFStringCreateWithCString(kCFAllocatorDefault, text, kCFStringEncodingUTF8), stylesDict);
            textLine = CTLineCreateWithAttributedString(attRef);
            CGRect textRect = CTLineGetImageBounds(textLine, cgContext);
            CGContextSetTextPosition(cgContext, ((windowWidth - textRect.size.width) / 2.), (windowHeight / 2.) + (coneHeight / 2.) + textRect.size.height + 50.);
            CTLineDraw(textLine, cgContext);
            CFRelease(textLine);
            CFRelease(attRef);
        }
        CFRelease(stylesDict);
    } else {
        // draw a background colored rect
        CGContextAddRect(cgContext, CGRectMake(0, 0, windowWidth, windowHeight));
        CGContextSetFillColorWithColor(cgContext, backgroundColor);
        CGContextDrawPath(cgContext, kCGPathFill);

        const char *text = self.cppPlugin->get_options().get_bg_text().c_str();
        if (text != NULL) {
            CGContextSetGrayStrokeColor(cgContext, .95, 1.);
            CGContextSetTextDrawingMode(cgContext, kCGTextFill);
            CGContextSetGrayFillColor(cgContext, 1., 1.);
            CFStringRef keys[2];
            keys[0] = kCTFontAttributeName;
            keys[1] = kCTForegroundColorFromContextAttributeName;
            CFTypeRef values[2];
            values[0] = CTFontCreateWithName(CFSTR("HelveticaNeue-Light"),18,NULL);
            values[1] = kCFBooleanTrue;
            CFDictionaryRef stylesDict = CFDictionaryCreate(kCFAllocatorDefault,
                                                            (const void **)&keys,
                                                            (const void **)&values,
                                                            2, NULL, NULL);
            CFAttributedStringRef attRef = CFAttributedStringCreate(kCFAllocatorDefault, CFStringCreateWithCString(kCFAllocatorDefault, text, kCFStringEncodingUTF8), stylesDict);
            CTLineRef textLine = CTLineCreateWithAttributedString(attRef);
            CGRect textRect = CTLineGetImageBounds(textLine, cgContext);
            CGContextSetTextPosition(cgContext, ((windowWidth - textRect.size.width) / 2.), (windowHeight / 2.));
            CTLineDraw(textLine, cgContext);
            CFRelease(textLine);
            CFRelease(attRef);
            CFRelease(stylesDict);
        }
    }
    CGColorRelease(backgroundColor);

    CGContextRestoreGState(cgContext);
}

@end

@implementation VLCControllerLayer

@synthesize cppPlugin = _cppPlugin, mediaPosition = _mediaPosition, isPlaying = _isPlaying, isFullscreen = _isFullscreen;

- (id)init
{
    if (self = [super init]) {
        self.needsDisplayOnBoundsChange = YES;
        self.frame = CGRectMake(0, 0, 0, 25);
        self.autoresizingMask = kCALayerWidthSizable;

        _playImage = createImageNamed(@"Play");
        _pauseImage = createImageNamed(@"Pause");
        _sliderTrackLeft = createImageNamed(@"SliderTrackLeft");
        _sliderTrackRight = createImageNamed(@"SliderTrackRight");
        _sliderTrackCenter = createImageNamed(@"SliderTrackCenter");

        _enterFullscreen = createImageNamed(@"enter-fullscreen");
        _leaveFullscreen = createImageNamed(@"leave-fullscreen");

        _knob = createImageNamed(@"Knob");
    }

    return self;
}

- (void)dealloc
{
    CGImageRelease(_playImage);
    CGImageRelease(_pauseImage);

    CGImageRelease(_sliderTrackLeft);
    CGImageRelease(_sliderTrackRight);
    CGImageRelease(_sliderTrackCenter);

    CGImageRelease(_enterFullscreen);
    CGImageRelease(_leaveFullscreen);

    CGImageRelease(_knob);

    [super dealloc];
}

- (CGRect)_playPauseButtonRect
{
    return CGRectMake(4., (25. - CGImageGetHeight(_playImage)) / 2., CGImageGetWidth(_playImage), CGImageGetHeight(_playImage));
}

- (CGRect)_fullscreenButtonRect
{
    return CGRectMake( CGRectGetMaxX([self _sliderRect]), (25. - CGImageGetHeight(_enterFullscreen)) / 2., CGImageGetWidth(_enterFullscreen), CGImageGetHeight(_enterFullscreen));
}

- (CGRect)_sliderRect
{
    CGFloat sliderYPosition = (self.bounds.size.height - CGImageGetHeight(_sliderTrackLeft)) / 2.;
    CGFloat playPauseButtonWidth = [self _playPauseButtonRect].size.width;
    CGFloat fullscreenButtonWidth = self.cppPlugin->get_options().get_enable_fs() ? CGImageGetWidth(_enterFullscreen) : 0.;

    return CGRectMake(playPauseButtonWidth + 7, sliderYPosition,
                      self.bounds.size.width - playPauseButtonWidth - fullscreenButtonWidth - 15., CGImageGetHeight(_sliderTrackLeft));
}

- (CGRect)_sliderThumbRect
{
    CGRect sliderRect = [self _sliderRect];

    CGFloat x = self.mediaPosition * (CGRectGetWidth(sliderRect) - CGImageGetWidth(_knob));

    return CGRectMake(CGRectGetMinX(sliderRect) + x, CGRectGetMinY(sliderRect) + 1,
                      CGImageGetWidth(_knob), CGImageGetHeight(_knob));
}

- (CGRect)_innerSliderRect
{
    return CGRectInset([self _sliderRect], CGRectGetWidth([self _sliderThumbRect]) / 2, 0);
}

- (void)_drawPlayPauseButtonInContext:(CGContextRef)context
{
    CGContextDrawImage(context, [self _playPauseButtonRect], self.isPlaying ? _pauseImage : _playImage);
}

- (void)_drawSliderInContext:(CGContextRef)context
{
    // Draw the thumb
    CGRect sliderThumbRect = [self _sliderThumbRect];
    CGContextDrawImage(context, sliderThumbRect, _knob);

    CGRect sliderRect = [self _sliderRect];

    // Draw left part
    CGRect sliderLeftTrackRect = CGRectMake(CGRectGetMinX(sliderRect), CGRectGetMinY(sliderRect),
                                            CGImageGetWidth(_sliderTrackLeft), CGImageGetHeight(_sliderTrackLeft));
    CGContextDrawImage(context, sliderLeftTrackRect, _sliderTrackLeft);

    // Draw center part
    CGRect sliderCenterTrackRect = CGRectInset(sliderRect, CGImageGetWidth(_sliderTrackLeft), 0);
    CGContextDrawImage(context, sliderCenterTrackRect, _sliderTrackCenter);

    // Draw right part
    CGRect sliderRightTrackRect = CGRectMake(CGRectGetMaxX(sliderCenterTrackRect), CGRectGetMinY(sliderRect),
                                             CGImageGetWidth(_sliderTrackRight), CGImageGetHeight(_sliderTrackRight));
    CGContextDrawImage(context, sliderRightTrackRect, _sliderTrackRight);

    // Draw fullscreen button
    if (self.cppPlugin->get_options().get_enable_fs()) {
        CGRect fullscreenButtonRect = [self _fullscreenButtonRect];
        fullscreenButtonRect.origin.x = CGRectGetMaxX(sliderRightTrackRect) + 5;
        CGContextDrawImage(context, fullscreenButtonRect, self.isFullscreen ? _leaveFullscreen : _enterFullscreen);
    }
}

- (void)drawInContext:(CGContextRef)cgContext
{
    CGContextSetFillColorWithColor(cgContext, CGColorGetConstantColor(kCGColorBlack));
    CGContextFillRect(cgContext, self.bounds);

    [self _drawPlayPauseButtonInContext:cgContext];
    [self _drawSliderInContext:cgContext];
}

- (void)_setNewTimeForThumbCenterX:(CGFloat)centerX
{
    CGRect innerRect = [self _innerSliderRect];

    double fraction = (centerX - CGRectGetMinX(innerRect)) / CGRectGetWidth(innerRect);
    if (fraction > 1.0)
        fraction = 1.0;
    else if (fraction < 0.0)
        fraction = 0.0;

    libvlc_media_player_set_position(self.cppPlugin->getMD(), fraction);

    [self setNeedsDisplay];
}

- (void)handleMouseDown:(CGPoint)point
{
    if (CGRectContainsPoint([self _sliderRect], point)) {
        _wasPlayingBeforeMouseDown = self.isPlaying;
        _isScrubbing = YES;

        if (CGRectContainsPoint([self _sliderThumbRect], point))
            _mouseDownXDelta = point.x - CGRectGetMidX([self _sliderThumbRect]);
        else {
            [self _setNewTimeForThumbCenterX:point.x];
            _mouseDownXDelta = 0;
        }
    }
}

- (void)handleMouseUp:(CGPoint)point
{
    if (_isScrubbing) {
        _isScrubbing = NO;
        _mouseDownXDelta = 0;

        return;
    }

    if (CGRectContainsPoint([self _playPauseButtonRect], point)) {
        self.cppPlugin->playlist_togglePause();
        return;
    }
    if (CGRectContainsPoint([self _fullscreenButtonRect], point)) {
        self.cppPlugin->toggle_fullscreen();
        return;
    }
}

- (void)handleMouseDragged:(CGPoint)point
{
    if (!_isScrubbing)
        return;

    point.x -= _mouseDownXDelta;

    [self _setNewTimeForThumbCenterX:point.x];
}

@end

@implementation NSScreen (VLCAdditions)

- (BOOL)hasMenuBar
{
    return ([self displayID] == [[[NSScreen screens] objectAtIndex:0] displayID]);
}

- (BOOL)hasDock
{
    NSRect screen_frame = [self frame];
    NSRect screen_visible_frame = [self visibleFrame];
    CGFloat f_menu_bar_thickness = [self hasMenuBar] ? [[NSStatusBar systemStatusBar] thickness] : 0.0;

    BOOL b_found_dock = NO;
    if (screen_visible_frame.size.width < screen_frame.size.width)
        b_found_dock = YES;
    else if (screen_visible_frame.size.height + f_menu_bar_thickness < screen_frame.size.height)
        b_found_dock = YES;

    return b_found_dock;
}

- (CGDirectDisplayID)displayID
{
    return (CGDirectDisplayID)[[[self deviceDescription] objectForKey: @"NSScreenNumber"] intValue];
}

@end

@implementation VLCFullscreenWindow

@synthesize customContentView = _customContentView;

- (id)initWithContentRect:(NSRect)contentRect
{
    if (self = [super initWithContentRect:contentRect styleMask:NSBorderlessWindowMask backing:NSBackingStoreBuffered defer:NO]) {
        _initialFrame = contentRect;
        [self setBackgroundColor:[NSColor blackColor]];
        [self setAcceptsMouseMovedEvents: YES];

        _customContentView = [[VLCFullscreenContentView alloc] initWithFrame:_initialFrame];
        [_customContentView setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
        [[self contentView] addSubview: _customContentView];
        [self setInitialFirstResponder:_customContentView];
    }
    return self;
}

- (void)dealloc
{
    [_customContentView release];
    [super dealloc];
}

- (BOOL)canBecomeKeyWindow
{
    return YES;
}

- (BOOL)canBecomeMainWindow
{
    return YES;
}

@end

@implementation VLCFullscreenContentView

@synthesize cppPlugin = _cppPlugin;

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)canBecomeKeyView
{
    return YES;
}

- (void)keyDown:(NSEvent *)theEvent
{
    NSString * characters = [theEvent charactersIgnoringModifiers];
    unichar key = 0;

    if ([characters length] > 0) {
        key = [[characters lowercaseString] characterAtIndex: 0];
        if (key) {
            /* Escape should always get you out of fullscreen */
            if (key == (unichar) 0x1b) {
                _cppPlugin->toggle_fullscreen();
                return;
            } else if (key == ' ') {
                _cppPlugin->playlist_togglePause();
                return;
            }
        }
    }
    [super keyDown: theEvent];
}

- (void)mouseDown:(NSEvent *)theEvent
{
    NSEventType eventType = [theEvent type];

    if (eventType == NSLeftMouseDown && !([theEvent modifierFlags] & NSControlKeyMask)) {
        if ([theEvent clickCount] >= 2)
            _cppPlugin->toggle_fullscreen();
        else {
            NSPoint point = [NSEvent mouseLocation];
            /* for Firefox, retina doesn't exist yet so it will return pixels instead of points when doing the conversation
             * so don't convert for Firefox */
            if (!_cppPlugin->runningWithinFirefox)
                [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage controllerLayer] handleMouseDown:[[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage browserRootLayer] convertPoint:CGPointMake(point.x, point.y) toLayer:[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage controllerLayer]]];
            else
                [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage controllerLayer] handleMouseDown:CGPointMake(point.x, point.y)];
        }
    }
    if ([(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] != nil) {
        if ([[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] respondsToSelector:@selector(mouseButtonDown:)]) {
            if (eventType == NSLeftMouseDown)
                [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] mouseButtonDown:0];
            else if (eventType == NSRightMouseDown)
                [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] mouseButtonDown:1];
            else
                [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] mouseButtonDown:2];
        }
    }

    [super mouseDown: theEvent];
}

- (void)mouseUp:(NSEvent *)theEvent
{
    NSPoint point = [NSEvent mouseLocation];
    NSEventType eventType = [theEvent type];

    /* for Firefox, retina doesn't exist yet so it will return pixels instead of points when doing the conversation
     * so don't convert for Firefox */
    if (!_cppPlugin->runningWithinFirefox)
        [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage controllerLayer] handleMouseUp:[[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage browserRootLayer] convertPoint:CGPointMake(point.x, point.y) toLayer:[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage controllerLayer]]];
    else
        [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage controllerLayer] handleMouseUp:CGPointMake(point.x, point.y)];

    if ([(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] != nil) {
        if ([[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] respondsToSelector:@selector(mouseButtonUp:)]) {
            if (eventType == NSLeftMouseUp)
                [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] mouseButtonUp:0];
            else if (eventType == NSRightMouseUp)
                [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] mouseButtonUp:1];
            else
                [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] mouseButtonUp:2];
        }
    }

    [super mouseUp: theEvent];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
    NSPoint point = [NSEvent mouseLocation];

    [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage controllerLayer] handleMouseDragged:[[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage browserRootLayer] convertPoint:CGPointMake(point.x, point.y) toLayer:[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage controllerLayer]]];

    [super mouseDragged: theEvent];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    self.cppPlugin->set_toolbar_visible(true);
    _timeSinceLastMouseMove = [NSDate timeIntervalSinceReferenceDate];
    [self performSelector:@selector(_hideToolbar) withObject:nil afterDelay: 4.1];

    if ([(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] != nil) {
        if ([[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] respondsToSelector:@selector(mouseMovedToX:Y:)]) {
            NSPoint ml = [theEvent locationInWindow];
            [[(VLCPerInstanceStorage *)_cppPlugin->_perInstanceStorage playbackLayer] mouseMovedToX:ml.x Y:([self.window frame].size.height - ml.y)];
        }
    }

    [super mouseMoved: theEvent];
}

- (void)_hideToolbar
{
    if ([NSDate timeIntervalSinceReferenceDate] - _timeSinceLastMouseMove >= 4)
        [self hideToolbar];
}

- (void)hideToolbar
{
    self.cppPlugin->set_toolbar_visible(false);
    [NSCursor setHiddenUntilMouseMoves:YES];
}

@end

#pragma mark - helpers

CGImageRef createImageNamed(NSString *name)
{
    CFURLRef url = CFBundleCopyResourceURL(CFBundleGetBundleWithIdentifier(CFSTR("org.videolan.vlc-npapi-plugin")), (CFStringRef)name, CFSTR("png"), NULL);

    if (!url)
        return NULL;

    CGImageSourceRef imageSource = CGImageSourceCreateWithURL(url, NULL);
    if (!imageSource)
        return NULL;

    CGImageRef image = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);
    CFRelease(imageSource);

    return image;
}
