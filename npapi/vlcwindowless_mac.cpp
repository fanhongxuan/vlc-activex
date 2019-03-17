/*****************************************************************************
 * vlcwindowless_mac.cpp: VLC NPAPI windowless plugin for Mac
 *****************************************************************************
 * Copyright (C) 2012-2014 VLC Authors and VideoLAN
 * $Id$
 *
 * Authors: Felix Paul Kühne <fkuehne # videolan # org>
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

#include <npapi.h>
#include "vlcwindowless_mac.h"

#define SHOW_BRANDING 1

CGImageRef createImageNamed(CFStringRef);

CGImageRef createImageNamed(CFStringRef name)
{
    CFURLRef url = CFBundleCopyResourceURL(CFBundleGetBundleWithIdentifier(CFSTR("org.videolan.vlc-npapi-plugin")), name, CFSTR("png"), NULL);

    if (!url)
        return NULL;

    CGImageSourceRef imageSource = CGImageSourceCreateWithURL(url, NULL);
    if (!imageSource) {
        CFRelease(url);
        return NULL;
    }

    CGImageRef image = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);
    CFRelease(imageSource);
    CFRelease(url);

    return image;
}

VlcWindowlessMac::VlcWindowlessMac(NPP instance, NPuint16_t mode) :
    VlcWindowlessBase(instance, mode)
{
    colorspace = CGColorSpaceCreateDeviceRGB();

    const char *userAgent = NPN_UserAgent(this->getBrowser());
    if (strstr(userAgent, "Safari") && strstr(userAgent, "Version/5")) {
        legacy_drawing_mode = true;
        fprintf(stderr, "Safari 5 detected, using legacy drawing mode\n");
    }
}

VlcWindowlessMac::~VlcWindowlessMac()
{
    if (lastFrame)
        CGImageRelease(lastFrame);
    CGColorSpaceRelease(colorspace);
}

void VlcWindowlessMac::drawNoPlayback(CGContextRef cgContext)
{
    float windowWidth = npwindow.width;
    float windowHeight = npwindow.height;

    CGContextSaveGState(cgContext);

    // this context is flipped..
    CGContextTranslateCTM(cgContext, 0.0, windowHeight);
    CGContextScaleCTM(cgContext, 1., -1.);

    CGColorRef backgroundColor;
    unsigned r = 0, g = 0, b = 0;
    HTMLColor2RGB(get_options().get_bg_color().c_str(), &r, &g, &b);
    backgroundColor = CGColorCreateGenericRGB(r, g, b, 1.);

    if (get_options().get_enable_branding()) {
        // draw background
        CGContextAddRect(cgContext, CGRectMake(0, 0, windowWidth, windowHeight));
        CGContextSetFillColorWithColor(cgContext, backgroundColor);
        CGContextDrawPath(cgContext, kCGPathFill);

        // draw gradient
        CGImageRef gradient = createImageNamed(CFSTR("gradient"));
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
        CGContextSetTextPosition(cgContext, 25., 45.);
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

        CFStringRef versionText = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%s — windowless mode — %@"), libvlc_get_version(), arch);
        attRef = CFAttributedStringCreate(kCFAllocatorDefault, versionText, stylesDict);
        CFRelease(versionText);
        textLine = CTLineCreateWithAttributedString(attRef);
        CGContextSetTextPosition(cgContext, 25., 25.);
        CTLineDraw(textLine, cgContext);
        CFRelease(textLine);
        CFRelease(attRef);
        CFRelease(stylesDict);

        // draw cone
        CGImageRef cone = createImageNamed(CFSTR("cone"));
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
        const char *text = get_options().get_bg_text().c_str();
        if (text != NULL) {
            CFStringRef customText = CFStringCreateWithCString(kCFAllocatorDefault, text, kCFStringEncodingUTF8);
            attRef = CFAttributedStringCreate(kCFAllocatorDefault, customText, stylesDict);
            CFRelease(customText);
            textLine = CTLineCreateWithAttributedString(attRef);
            CGRect textRect = CTLineGetImageBounds(textLine, cgContext);
            CGContextSetTextPosition(cgContext, ((windowWidth - textRect.size.width) / 2.), (windowHeight / 2.) + (coneHeight / 2.) + textRect.size.height + 50.);
            CTLineDraw(textLine, cgContext);
            CFRelease(textLine);
            CFRelease(attRef);
        }
        CFRelease(stylesDict);
    } else {
        CGContextAddRect(cgContext, CGRectMake(0, 0, windowWidth, windowHeight));
        CGContextSetFillColorWithColor(cgContext, backgroundColor);
        CGContextDrawPath(cgContext, kCGPathFill);

        const char *text = get_options().get_bg_text().c_str();
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
            CFStringRef customText = CFStringCreateWithCString(kCFAllocatorDefault, text, kCFStringEncodingUTF8);
            CFAttributedStringRef attRef = CFAttributedStringCreate(kCFAllocatorDefault, customText, stylesDict);
            CFRelease(customText);
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

NPError VlcWindowlessMac::get_root_layer(void *value)
{
    return NPERR_GENERIC_ERROR;
}

bool VlcWindowlessMac::handle_event(void *event)
{
    NPCocoaEvent* cocoaEvent = (NPCocoaEvent*)event;

    if (!event)
        return false;

    NPCocoaEventType eventType = cocoaEvent->type;

    switch (eventType) {
        case NPCocoaEventMouseDown:
        {
            if (cocoaEvent->data.mouse.clickCount >= 2)
                VlcWindowlessBase::toggle_fullscreen();

            return true;
        }
        case NPCocoaEventMouseUp:
        case NPCocoaEventKeyUp:
        case NPCocoaEventKeyDown:
        case NPCocoaEventFocusChanged:
        case NPCocoaEventScrollWheel:
            return true;

        default:
            break;
    }

    if (eventType == NPCocoaEventDrawRect) {
        CGContextRef cgContext = cocoaEvent->data.draw.context;
        if (!cgContext) {
            return false;
        }

        CGContextClearRect(cgContext, CGRectMake(0, 0, npwindow.width, npwindow.height) );

        if (m_media_width == 0 || m_media_height == 0 || (!lastFrame && !VlcPluginBase::playlist_isplaying()) || !get_player().is_open()) {
            drawNoPlayback(cgContext);
            return true;
        }

        CGContextSaveGState(cgContext);

        /* context is flipped */
        CGContextTranslateCTM(cgContext, 0.0, npwindow.height);
        CGContextScaleCTM(cgContext, 1., -1.);

        /* Compute the position of the video */
        float left = 0;
        float top  = 0;

        const size_t kComponentsPerPixel = 4;
        const size_t kBitsPerComponent = sizeof(unsigned char) * 8;
        CGRect rect;

        if (m_media_width != 0 && m_media_height != 0) {
            cached_width = m_media_width;
            cached_height = m_media_height;
            left = (npwindow.width  - m_media_width) / 2.;
            top = (npwindow.height - m_media_height) / 2.;

            /* fetch frame */
            CFDataRef dataRef = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault,
                                                            (const uint8_t *)&m_frame_buf[0],
                                                            sizeof(m_frame_buf[0]),
                                                            kCFAllocatorNull);
            CGDataProviderRef dataProvider = CGDataProviderCreateWithCFData(dataRef);
            lastFrame = CGImageCreate(m_media_width,
                                      m_media_height,
                                      kBitsPerComponent,
                                      kBitsPerComponent * kComponentsPerPixel,
                                      kComponentsPerPixel * m_media_width,
                                      colorspace,
                                      kCGBitmapByteOrder16Big,
                                      dataProvider,
                                      NULL,
                                      true,
                                      kCGRenderingIntentPerceptual);

            CGDataProviderRelease(dataProvider);
            CFRelease(dataRef);

            if (!lastFrame) {
                fprintf(stderr, "image creation failed\n");
                CGImageRelease(lastFrame);
                CGContextRestoreGState(cgContext);
                return true;
            }

            rect = CGRectMake(left, top, m_media_width, m_media_height);
        } else {
            fprintf(stderr, "drawing old frame again\n");
            left = (npwindow.width - cached_width) / 2.;
            top = (npwindow.height - cached_height) / 2.;
            rect = CGRectMake(left, top, cached_width, cached_width);
        }

        if(lastFrame) {
            CGContextDrawImage(cgContext, rect, lastFrame);
            CGImageRelease(lastFrame);
        }

        CGContextRestoreGState(cgContext);

        return true;
    }

    return VlcPluginBase::handle_event(event);
}

void VlcWindowlessMac::video_display_cb(void * /*picture*/)
{
    if (p_browser) {
        if (!legacy_drawing_mode)
            NPN_PluginThreadAsyncCall(p_browser,
                                      VlcWindowlessBase::invalidate_window_proxy,
                                      this);
        else
            invalidate_window();
    }
}

void VlcWindowlessMac::set_player_window() {
    libvlc_video_set_format_callbacks(getMD(),
                                      video_format_proxy,
                                      video_cleanup_proxy);
    libvlc_video_set_callbacks(getMD(),
                               video_lock_proxy,
                               video_unlock_proxy,
                               video_display_proxy,
                               this);
}
