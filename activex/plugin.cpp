/*****************************************************************************
 * plugin.cpp: ActiveX control for VLC
 *****************************************************************************
 * Copyright (C) 2006-2010 the VideoLAN team
 *
 * Authors: Damien Fouilleul <Damien.Fouilleul@laposte.net>
 *          Jean-Paul Saman <jpsaman@videolan.org>
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

#include "config.h"

#include "plugin.h"

#include "oleobject.h"
#include "olecontrol.h"
#include "oleinplaceobject.h"
#include "oleinplaceactiveobject.h"
#include "persistpropbag.h"
#include "persiststreaminit.h"
#include "persiststorage.h"
#include "provideclassinfo.h"
#include "connectioncontainer.h"
#include "objectsafety.h"
#include "vlccontrol.h"
#include "vlccontrol2.h"
#include "viewobject.h"
#include "dataobject.h"
#include "supporterrorinfo.h"

#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <winreg.h>
#include <winuser.h>
#include <servprov.h>
#include <shlwapi.h>
#include <wininet.h>
#include <assert.h>

using namespace std;

////////////////////////////////////////////////////////////////////////
//class factory

static LRESULT CALLBACK VLCInPlaceClassWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    VLCPlugin *p_instance = reinterpret_cast<VLCPlugin *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch( uMsg )
    {
        case WM_ERASEBKGND:
            return 1L;

        case WM_PAINT:
            PAINTSTRUCT ps;
            RECT pr;
            if( GetUpdateRect(hWnd, &pr, FALSE) )
            {
                RECT bounds;
                GetClientRect(hWnd, &bounds);
                BeginPaint(hWnd, &ps);
                p_instance->onPaint(ps.hdc, bounds, pr);
                EndPaint(hWnd, &ps);
            }
            return 0L;
        case WM_SIZE:{
            int new_client_width = LOWORD(lParam);
            int new_client_height = HIWORD(lParam);
            //first child will be resized to client area
            HWND hChildWnd = GetWindow(hWnd, GW_CHILD);
            if(hChildWnd){
                MoveWindow(hChildWnd, 0, 0, new_client_width, new_client_height, FALSE);
            }
            return 0L;
        }
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
};

VLCPluginClass::VLCPluginClass(LONG *p_class_ref, HINSTANCE hInstance, REFCLSID rclsid) :
    _p_class_ref(p_class_ref),
    _class_ref(0),
    _hinstance(hInstance),
    _classid(rclsid),
    _inplace_picture(NULL)
{
    WNDCLASS wClass;

    if( ! GetClassInfo(hInstance, getInPlaceWndClassName(), &wClass) )
    {
        wClass.style          = CS_NOCLOSE|CS_DBLCLKS;
        wClass.lpfnWndProc    = VLCInPlaceClassWndProc;
        wClass.cbClsExtra     = 0;
        wClass.cbWndExtra     = 0;
        wClass.hInstance      = hInstance;
        wClass.hIcon          = NULL;
        wClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wClass.hbrBackground  = NULL;
        wClass.lpszMenuName   = NULL;
        wClass.lpszClassName  = getInPlaceWndClassName();

        _inplace_wndclass_atom = RegisterClass(&wClass);
    }
    else
    {
        _inplace_wndclass_atom = 0;
    }

    HBITMAP hbitmap = (HBITMAP)LoadImage(getHInstance(), MAKEINTRESOURCE(2), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
    if( NULL != hbitmap )
    {
        PICTDESC pictDesc;

        pictDesc.cbSizeofstruct = sizeof(PICTDESC);
        pictDesc.picType        = PICTYPE_BITMAP;
        pictDesc.bmp.hbitmap    = hbitmap;
        pictDesc.bmp.hpal       = NULL;

        if( FAILED(OleCreatePictureIndirect(&pictDesc, IID_IPicture, TRUE, reinterpret_cast<LPVOID*>(&_inplace_picture))) )
            _inplace_picture = NULL;
    }
    AddRef();
};

VLCPluginClass::~VLCPluginClass()
{
    if( 0 != _inplace_wndclass_atom )
        UnregisterClass(MAKEINTATOM(_inplace_wndclass_atom), _hinstance);

    if( NULL != _inplace_picture )
        _inplace_picture->Release();
};

STDMETHODIMP VLCPluginClass::QueryInterface(REFIID riid, void **ppv)
{
    if( NULL == ppv )
        return E_INVALIDARG;

    if( (IID_IUnknown == riid)
     || (IID_IClassFactory == riid) )
    {
        AddRef();
        *ppv = reinterpret_cast<LPVOID>(this);

        return NOERROR;
    }

    *ppv = NULL;

    return E_NOINTERFACE;
};

STDMETHODIMP_(ULONG) VLCPluginClass::AddRef(void)
{
    InterlockedIncrement(_p_class_ref);
    return ++_class_ref;
};

STDMETHODIMP_(ULONG) VLCPluginClass::Release(void)
{
    InterlockedDecrement(_p_class_ref);
    if( 0 == (--_class_ref) )
    {
        delete this;
        return 0;
    }
    return _class_ref;
};

STDMETHODIMP VLCPluginClass::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void **ppv)
{
    if( NULL == ppv )
        return E_POINTER;

    *ppv = NULL;

    if( (NULL != pUnkOuter) && (IID_IUnknown != riid) ) {
        return CLASS_E_NOAGGREGATION;
    }

    VLCPlugin *plugin = new VLCPlugin(this, pUnkOuter);
    if( NULL != plugin )
    {
        HRESULT hr = plugin->QueryInterface(riid, ppv);
        // the following will destroy the object if QueryInterface() failed
        plugin->Release();
        return hr;
    }
    return E_OUTOFMEMORY;
};

STDMETHODIMP VLCPluginClass::LockServer(BOOL fLock)
{
    if( fLock )
        AddRef();
    else
        Release();

    return S_OK;
};

////////////////////////////////////////////////////////////////////////
extern HMODULE DllGetModule();

VLCPlugin::VLCPlugin(VLCPluginClass *p_class, LPUNKNOWN pUnkOuter) :
    _inplacewnd(NULL),
    _WindowsManager(DllGetModule(), _ViewRC, this),
    _p_class(p_class),
    _i_ref(1UL),
    _p_libvlc(NULL),
    _i_codepage(CP_ACP),
    _b_usermode(TRUE)
{
    _ViewRC.hDeFullscreenBitmap =
        LoadImage(DllGetModule(), MAKEINTRESOURCE(3),
                  IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);

    _ViewRC.hPlayBitmap =
        LoadImage(DllGetModule(), MAKEINTRESOURCE(4),
                  IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);

    _ViewRC.hPauseBitmap =
        LoadImage(DllGetModule(), MAKEINTRESOURCE(5),
                  IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);

    _ViewRC.hVolumeBitmap =
        LoadImage(DllGetModule(), MAKEINTRESOURCE(6),
                  IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);

    _ViewRC.hVolumeMutedBitmap =
        LoadImage(DllGetModule(), MAKEINTRESOURCE(7),
                  IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);

    _ViewRC.hBackgroundIcon =
        (HICON) LoadImage(DllGetModule(), MAKEINTRESOURCE(8),
                          IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);

    _ViewRC.hFullscreenBitmap =
        LoadImage(DllGetModule(), MAKEINTRESOURCE(9),
                  IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);

    p_class->AddRef();

    vlcOleControl = new VLCOleControl(this);
    vlcOleInPlaceObject = new VLCOleInPlaceObject(this);
    vlcOleInPlaceActiveObject = new VLCOleInPlaceActiveObject(this);
    vlcPersistStorage = new VLCPersistStorage(this);
    vlcPersistStreamInit = new VLCPersistStreamInit(this);
    vlcPersistPropertyBag = new VLCPersistPropertyBag(this);
    vlcProvideClassInfo = new VLCProvideClassInfo(this);
    vlcConnectionPointContainer = new VLCConnectionPointContainer(this);
    vlcObjectSafety = new VLCObjectSafety(this);
    vlcControl = new VLCControl(this);
    vlcControl2 = new VLCControl2(this);
    vlcViewObject = new VLCViewObject(this);
    vlcDataObject = new VLCDataObject(this);
    vlcOleObject = new VLCOleObject(this);
    vlcSupportErrorInfo = new VLCSupportErrorInfo(this);

    // configure controlling IUnknown interface for implemented interfaces
    this->pUnkOuter = (NULL != pUnkOuter) ? pUnkOuter : dynamic_cast<LPUNKNOWN>(this);

    // default picure
    _p_pict = p_class->getInPlacePict();

    // make sure that persistable properties are initialized
    onInit();
};

VLCPlugin::~VLCPlugin()
{
    if( vlc_player::is_open() )
    {
        if( isPlaying() )
            playlist_stop();

        player_unregister_events();
    }

    delete vlcSupportErrorInfo;
    delete vlcOleObject;
    delete vlcDataObject;
    delete vlcViewObject;
    delete vlcControl2;
    delete vlcControl;
    delete vlcConnectionPointContainer;
    delete vlcProvideClassInfo;
    delete vlcPersistPropertyBag;
    delete vlcPersistStreamInit;
    delete vlcPersistStorage;
    delete vlcOleInPlaceActiveObject;
    delete vlcOleInPlaceObject;
    delete vlcObjectSafety;

    delete vlcOleControl;
    if( _p_pict )
        _p_pict->Release();

    SysFreeString(_bstr_mrl);
    SysFreeString(_bstr_baseurl);

    if( _p_libvlc )  { libvlc_release(_p_libvlc); _p_libvlc=NULL; }

    _p_class->Release();
};

STDMETHODIMP VLCPlugin::QueryInterface(REFIID riid, void **ppv)
{
    if( NULL == ppv )
        return E_INVALIDARG;

    if( IID_IUnknown == riid )
        *ppv = reinterpret_cast<LPVOID>(this);
    else if( IID_IOleObject == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcOleObject);
    else if( IID_IOleControl == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcOleControl);
    else if( IID_IOleWindow == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcOleInPlaceObject);
    else if( IID_IOleInPlaceObject == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcOleInPlaceObject);
    else if( IID_IOleInPlaceActiveObject == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcOleInPlaceActiveObject);
    else if( IID_IPersist == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcPersistStreamInit);
    else if( IID_IPersistStreamInit == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcPersistStreamInit);
    else if( IID_IPersistStorage == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcPersistStorage);
    else if( IID_IPersistPropertyBag == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcPersistPropertyBag);
    else if( IID_IProvideClassInfo == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcProvideClassInfo);
    else if( IID_IProvideClassInfo2 == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcProvideClassInfo);
    else if( IID_IConnectionPointContainer == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcConnectionPointContainer);
    else if( IID_IObjectSafety == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcObjectSafety);
    else if( IID_IDispatch == riid )
        *ppv = (CLSID_VLCPlugin2 == getClassID()) ?
                reinterpret_cast<LPVOID>(vlcControl2) :
                reinterpret_cast<LPVOID>(vlcControl);
    else if( IID_IVLCControl == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcControl);
    else if( IID_IVLCControl2 == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcControl2);
    else if( IID_IViewObject == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcViewObject);
    else if( IID_IViewObject2 == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcViewObject);
    else if( IID_IDataObject == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcDataObject);
    else if( IID_ISupportErrorInfo == riid )
        *ppv = reinterpret_cast<LPVOID>(vlcSupportErrorInfo);
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    ((LPUNKNOWN)*ppv)->AddRef();
    return NOERROR;
};

STDMETHODIMP_(ULONG) VLCPlugin::AddRef(void)
{
    return InterlockedIncrement((LONG *)&_i_ref);
};

STDMETHODIMP_(ULONG) VLCPlugin::Release(void)
{
    if( ! InterlockedDecrement((LONG *)&_i_ref) )
    {
        delete this;
        return 0;
    }
    return _i_ref;
};

//////////////////////////////////////

HRESULT VLCPlugin::onInit(void)
{
    if( NULL == _p_libvlc )
    {
        // initialize persistable properties
        set_autoplay(true);
        _b_autoloop   = FALSE;
        _bstr_baseurl = NULL;
        _bstr_mrl     = NULL;
        _b_visible    = TRUE;
        _b_mute       = FALSE;
        _i_volume     = 50;
        _i_time       = 0;
        _i_backcolor  = 0;
        // set default/preferred size (320x240) pixels in HIMETRIC
        HDC hDC = CreateDevDC(NULL);
        _extent.cx = 320;
        _extent.cy = 240;
        HimetricFromDP(hDC, (LPPOINT)&_extent, 1);
        DeleteDC(hDC);

        return S_OK;
    }
    return CO_E_ALREADYINITIALIZED;
};

HRESULT VLCPlugin::onLoad(void)
{
    if( SysStringLen(_bstr_baseurl) == 0 )
    {
        /*
        ** try to retreive the base URL using the client site moniker, which for Internet Explorer
        ** is the URL of the page the plugin is embedded into.
        */
        LPOLECLIENTSITE pClientSite;
        if( SUCCEEDED(vlcOleObject->GetClientSite(&pClientSite)) && (NULL != pClientSite) )
        {
            IBindCtx *pBC = 0;
            if( SUCCEEDED(CreateBindCtx(0, &pBC)) )
            {
                LPMONIKER pContMoniker = NULL;
                if( SUCCEEDED(pClientSite->GetMoniker(OLEGETMONIKER_ONLYIFTHERE,
                                OLEWHICHMK_CONTAINER, &pContMoniker)) )
                {
                    LPOLESTR base_url;
                    if( SUCCEEDED(pContMoniker->GetDisplayName(pBC, NULL, &base_url)) )
                    {
                        /*
                        ** check that the moniker name is a URL
                        */
                        if( UrlIsW(base_url, URLIS_URL) )
                        {
                            /* copy base URL */
                            _bstr_baseurl = SysAllocString(base_url);
                        }
                        CoTaskMemFree(base_url);
                    }
                }
            }
        }
    }
    setDirty(FALSE);
    return S_OK;
};

void VLCPlugin::initVLC()
{
    static const char * const ppsz_argv[] = {
        "-vv",
        "--no-stats",
        "--intf=dummy",
        "--no-video-title-show",
    };

    _p_libvlc = libvlc_new(sizeof(ppsz_argv) / sizeof(*ppsz_argv), ppsz_argv);
    if( !_p_libvlc )
        return;

    if( !vlc_player::open(_p_libvlc) )
        return;

    vlc_player::set_mode(_b_autoloop ? libvlc_playback_mode_loop :
                                       libvlc_playback_mode_default);

    // register player events
    player_register_events();

    // initial playlist item
    if( SysStringLen(_bstr_mrl) > 0 )
    {
        char *psz_mrl = NULL;

        if( SysStringLen(_bstr_baseurl) > 0 )
        {
            /*
            ** if the MRL a relative URL, we should end up with an absolute URL
            */
            LPWSTR abs_url = CombineURL(_bstr_baseurl, _bstr_mrl);
            if( NULL != abs_url )
            {
                psz_mrl = CStrFromWSTR(CP_UTF8, abs_url, wcslen(abs_url));
                CoTaskMemFree(abs_url);
            }
            else
            {
                psz_mrl = CStrFromBSTR(CP_UTF8, _bstr_mrl);
            }
        }
        else
        {
            /*
            ** baseURL is empty, assume MRL is absolute
            */
            psz_mrl = CStrFromBSTR(CP_UTF8, _bstr_mrl);
        }
        if( NULL != psz_mrl )
        {
            const char *options[1];
            int i_options = 0;

            char timeBuffer[32];
            if( _i_time )
            {
                snprintf(timeBuffer, sizeof(timeBuffer), ":start-time=%d", _i_time);
                options[i_options++] = timeBuffer;
            }
            // add default target to playlist
            playlist_add_extended_untrusted(psz_mrl, i_options, options);
            CoTaskMemFree(psz_mrl);
        }
    }

    if( !isInPlaceActive()  )
    {
        LPOLECLIENTSITE pClientSite;
        if( SUCCEEDED(vlcOleObject->GetClientSite(&pClientSite)) && (NULL != pClientSite) )
        {
            vlcOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pClientSite, 0, NULL, NULL);
            pClientSite->Release();
        }
    }

    set_player_window();
};

void VLCPlugin::setErrorInfo(REFIID riid, const char *description)
{
    vlcSupportErrorInfo->setErrorInfo( getClassID() == CLSID_VLCPlugin2 ?
        OLESTR("VideoLAN.VLCPlugin.2") : OLESTR("VideoLAN.VLCPlugin.1"),
        riid, description );
};

HRESULT VLCPlugin::onAmbientChanged(LPUNKNOWN pContainer, DISPID dispID)
{
    VARIANT v;
    switch( dispID )
    {
        case DISPID_AMBIENT_BACKCOLOR:
            VariantInit(&v);
            V_VT(&v) = VT_I4;
            if( SUCCEEDED(GetObjectProperty(pContainer, dispID, v)) )
            {
                setBackColor(V_I4(&v));
            }
            break;
        case DISPID_AMBIENT_DISPLAYNAME:
            break;
        case DISPID_AMBIENT_FONT:
            break;
        case DISPID_AMBIENT_FORECOLOR:
            break;
        case DISPID_AMBIENT_LOCALEID:
            break;
        case DISPID_AMBIENT_MESSAGEREFLECT:
            break;
        case DISPID_AMBIENT_SCALEUNITS:
            break;
        case DISPID_AMBIENT_TEXTALIGN:
            break;
        case DISPID_AMBIENT_USERMODE:
            VariantInit(&v);
            V_VT(&v) = VT_BOOL;
            if( SUCCEEDED(GetObjectProperty(pContainer, dispID, v)) )
            {
                setUserMode(V_BOOL(&v) != VARIANT_FALSE);
            }
            break;
        case DISPID_AMBIENT_UIDEAD:
            break;
        case DISPID_AMBIENT_SHOWGRABHANDLES:
            break;
        case DISPID_AMBIENT_SHOWHATCHING:
            break;
        case DISPID_AMBIENT_DISPLAYASDEFAULT:
            break;
        case DISPID_AMBIENT_SUPPORTSMNEMONICS:
            break;
        case DISPID_AMBIENT_AUTOCLIP:
            break;
        case DISPID_AMBIENT_APPEARANCE:
            break;
        case DISPID_AMBIENT_CODEPAGE:
            VariantInit(&v);
            V_VT(&v) = VT_I4;
            if( SUCCEEDED(GetObjectProperty(pContainer, dispID, v)) )
            {
                setCodePage(V_I4(&v));
            }
            break;
        case DISPID_AMBIENT_PALETTE:
            break;
        case DISPID_AMBIENT_CHARSET:
            break;
        case DISPID_AMBIENT_RIGHTTOLEFT:
            break;
        case DISPID_AMBIENT_TOPTOBOTTOM:
            break;
        case DISPID_UNKNOWN:
            /*
            ** multiple property change, look up the ones we are interested in
            */
            VariantInit(&v);
            V_VT(&v) = VT_BOOL;
            if( SUCCEEDED(GetObjectProperty(pContainer, DISPID_AMBIENT_USERMODE, v)) )
            {
                setUserMode(V_BOOL(&v) != VARIANT_FALSE);
            }
            VariantInit(&v);
            V_VT(&v) = VT_I4;
            if( SUCCEEDED(GetObjectProperty(pContainer, DISPID_AMBIENT_CODEPAGE, v)) )
            {
                setCodePage(V_I4(&v));
            }
            break;
    }
    return S_OK;
};

HRESULT VLCPlugin::onClose(DWORD)
{
    if( isInPlaceActive() )
    {
        onInPlaceDeactivate();
    }
    if( isRunning() )
    {
        libvlc_instance_t* p_libvlc = _p_libvlc;

        _p_libvlc = NULL;
        vlcDataObject->onClose();

        if( p_libvlc )
            libvlc_release(p_libvlc);
    }
    return S_OK;
};

BOOL VLCPlugin::isInPlaceActive(void)
{
    return (NULL != _inplacewnd);
};

HRESULT VLCPlugin::onActivateInPlace(LPMSG, HWND hwndParent, LPCRECT lprcPosRect, LPCRECT lprcClipRect)
{
    RECT clipRect = *lprcClipRect;

    /*
    ** record keeping of control geometry within container
    */
    _posRect = *lprcPosRect;

    /*
    ** Create a window for in place activated control.
    ** the window geometry matches the control viewport
    ** within container so that embedded video is always
    ** properly displayed.
    */
    _inplacewnd = CreateWindow(_p_class->getInPlaceWndClassName(),
            TEXT("VLC Plugin In-Place Window"),
            WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
            lprcPosRect->left,
            lprcPosRect->top,
            lprcPosRect->right-lprcPosRect->left,
            lprcPosRect->bottom-lprcPosRect->top,
            hwndParent,
            0,
            _p_class->getHInstance(),
            NULL
           );

    if( NULL == _inplacewnd )
        return E_FAIL;

    SetWindowLongPtr(_inplacewnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    /* change cliprect coordinates system relative to window bounding rect */
    OffsetRect(&clipRect, -lprcPosRect->left, -lprcPosRect->top);

    HRGN clipRgn = CreateRectRgnIndirect(&clipRect);
    SetWindowRgn(_inplacewnd, clipRgn, TRUE);

    if( isUserMode() ) {
        _WindowsManager.CreateWindows(this->getInPlaceWindow());
    }

    if( _b_usermode )
    {
        /* will run vlc if not done already */
        libvlc_instance_t* p_libvlc;
        HRESULT result = getVLC(&p_libvlc);
        if( FAILED(result) )
            return result;

        if( get_autoplay() )
        {
            vlc_player::play(0);
        }
    }

    if( isVisible() )
        ShowWindow(_inplacewnd, SW_SHOW);

    return S_OK;
};

void VLCPlugin::toggleFullscreen()
{
    _WindowsManager.ToggleFullScreen();
}

HRESULT VLCPlugin::onInPlaceDeactivate(void)
{
    if( isPlaying() )
    {
        playlist_stop();
        fireOnStopEvent();
    }

    _WindowsManager.DestroyWindows();

    DestroyWindow(_inplacewnd);
    _inplacewnd = NULL;

    return S_OK;
};

void VLCPlugin::setVisible(BOOL fVisible)
{
    if( fVisible != _b_visible )
    {
        _b_visible = fVisible;
        if( isInPlaceActive() )
        {
            ShowWindow(_inplacewnd, fVisible ? SW_SHOW : SW_HIDE);
            if( fVisible )
                InvalidateRect(_inplacewnd, NULL, TRUE);
        }
        setDirty(TRUE);
        firePropChangedEvent(DISPID_Visible);
    }
};

void VLCPlugin::setVolume(int volume)
{
    if( volume < 0 )
        volume = 0;
    else if( volume > 200 )
        volume = 200;

    if( volume != _i_volume )
    {
        _i_volume = volume;
        if( isRunning() )
        {
            libvlc_media_player_t *p_md;
            HRESULT hr = getMD(&p_md);
            if( SUCCEEDED(hr) )
                libvlc_audio_set_volume(p_md, _i_volume);
        }
        setDirty(TRUE);
    }
};

void VLCPlugin::setBackColor(OLE_COLOR backcolor)
{
    if( _i_backcolor != backcolor )
    {
        _i_backcolor = backcolor;
        if( isInPlaceActive() )
        {

        }
        setDirty(TRUE);
    }
};

void VLCPlugin::setTime(int seconds)
{
    if( seconds < 0 )
        seconds = 0;

    if( seconds != _i_time )
    {
        setStartTime(_i_time);
        if( vlc_player::is_open() )
        {
            vlc_player::set_time(_i_time);
        }
    }
};

void VLCPlugin::setFocus(BOOL fFocus)
{
    if( fFocus )
        SetActiveWindow(_inplacewnd);
};

BOOL VLCPlugin::hasFocus(void)
{
    return GetActiveWindow() == _inplacewnd;
};

void VLCPlugin::onDraw(DVTARGETDEVICE * ptd, HDC hicTargetDev,
        HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds)
{
    if( isVisible() )
    {
        long width = lprcBounds->right-lprcBounds->left;
        long height = lprcBounds->bottom-lprcBounds->top;

        RECT bounds = { lprcBounds->left, lprcBounds->top, lprcBounds->right, lprcBounds->bottom };

        if( isUserMode() )
        {
            /* VLC is in user mode, just draw background color */
            COLORREF colorref = RGB(0, 0, 0);
            OleTranslateColor(_i_backcolor, (HPALETTE)GetStockObject(DEFAULT_PALETTE), &colorref);
            if( colorref != RGB(0, 0, 0) )
            {
                /* custom background */
                HBRUSH colorbrush = CreateSolidBrush(colorref);
                FillRect(hdcDraw, &bounds, colorbrush);
                DeleteObject((HANDLE)colorbrush);
            }
            else
            {
                /* black background */
                FillRect(hdcDraw, &bounds, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
        }
        else
        {
            /* VLC is in design mode, draw the VLC logo */
            FillRect(hdcDraw, &bounds, (HBRUSH)GetStockObject(WHITE_BRUSH));

            LPPICTURE pict = getPicture();
            if( NULL != pict )
            {
                OLE_XSIZE_HIMETRIC picWidth;
                OLE_YSIZE_HIMETRIC picHeight;

                pict->get_Width(&picWidth);
                pict->get_Height(&picHeight);

                SIZEL picSize = { picWidth, picHeight };

                if( NULL != hicTargetDev )
                {
                    DPFromHimetric(hicTargetDev, (LPPOINT)&picSize, 1);
                }
                else if( NULL != (hicTargetDev = CreateDevDC(ptd)) )
                {
                    DPFromHimetric(hicTargetDev, (LPPOINT)&picSize, 1);
                    DeleteDC(hicTargetDev);
                }

                if( picSize.cx > width-4 )
                    picSize.cx = width-4;
                if( picSize.cy > height-4 )
                    picSize.cy = height-4;

                LONG dstX = lprcBounds->left+(width-picSize.cx)/2;
                LONG dstY = lprcBounds->top+(height-picSize.cy)/2;

                if( NULL != lprcWBounds )
                {
                    RECT wBounds = { lprcWBounds->left, lprcWBounds->top, lprcWBounds->right, lprcWBounds->bottom };
                    pict->Render(hdcDraw, dstX, dstY, picSize.cx, picSize.cy,
                            0L, picHeight, picWidth, -picHeight, &wBounds);
                }
                else
                    pict->Render(hdcDraw, dstX, dstY, picSize.cx, picSize.cy,
                            0L, picHeight, picWidth, -picHeight, NULL);

                pict->Release();
            }

            SelectObject(hdcDraw, GetStockObject(BLACK_BRUSH));

            MoveToEx(hdcDraw, bounds.left, bounds.top, NULL);
            LineTo(hdcDraw, bounds.left+width-1, bounds.top);
            LineTo(hdcDraw, bounds.left+width-1, bounds.top+height-1);
            LineTo(hdcDraw, bounds.left, bounds.top+height-1);
            LineTo(hdcDraw, bounds.left, bounds.top);
        }
    }
};

void VLCPlugin::onPaint(HDC hdc, const RECT &bounds, const RECT &)
{
    if( isVisible() )
    {
        /* if VLC is in design mode, draw control logo */
        HDC hdcDraw = CreateCompatibleDC(hdc);
        if( NULL != hdcDraw )
        {
            SIZEL size = getExtent();
            DPFromHimetric(hdc, (LPPOINT)&size, 1);
            RECTL posRect = { 0, 0, size.cx, size.cy };

            int width = bounds.right-bounds.left;
            int height = bounds.bottom-bounds.top;

            HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
            if( NULL != hBitmap )
            {
                HBITMAP oldBmp = (HBITMAP)SelectObject(hdcDraw, hBitmap);

                if( (size.cx != width) || (size.cy != height) )
                {
                    // needs to scale canvas
                    SetMapMode(hdcDraw, MM_ANISOTROPIC);
                    SetWindowExtEx(hdcDraw, size.cx, size.cy, NULL);
                    SetViewportExtEx(hdcDraw, width, height, NULL);
                }

                onDraw(NULL, hdc, hdcDraw, &posRect, NULL);

                SetMapMode(hdcDraw, MM_TEXT);
                BitBlt(hdc, bounds.left, bounds.top,
                        width, height,
                        hdcDraw, 0, 0,
                        SRCCOPY);

                SelectObject(hdcDraw, oldBmp);
                DeleteObject(hBitmap);
            }
            DeleteDC(hdcDraw);
        }
    }
};

void VLCPlugin::onPositionChange(LPCRECT lprcPosRect, LPCRECT lprcClipRect)
{
    RECT clipRect = *lprcClipRect;

    //RedrawWindow(GetParent(_inplacewnd), &_posRect, NULL, RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN);

    /*
    ** record keeping of control geometry within container
    */
    _posRect = *lprcPosRect;

    /*
    ** change in-place window geometry to match clipping region
    */
    SetWindowPos(_inplacewnd, NULL,
            lprcPosRect->left,
            lprcPosRect->top,
            lprcPosRect->right-lprcPosRect->left,
            lprcPosRect->bottom-lprcPosRect->top,
            SWP_NOACTIVATE|
            SWP_NOCOPYBITS|
            SWP_NOZORDER|
            SWP_NOOWNERZORDER );

    /* change cliprect coordinates system relative to window bounding rect */
    OffsetRect(&clipRect, -lprcPosRect->left, -lprcPosRect->top);
    HRGN clipRgn = CreateRectRgnIndirect(&clipRect);
    SetWindowRgn(_inplacewnd, clipRgn, FALSE);

    //RedrawWindow(_videownd, &posRect, NULL, RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN);
};

void VLCPlugin::freezeEvents(BOOL freeze)
{
    vlcConnectionPointContainer->freezeEvents(freeze);
};

void VLCPlugin::firePropChangedEvent(DISPID dispid)
{
    vlcConnectionPointContainer->firePropChangedEvent(dispid);
};

void VLCPlugin::fireOnPlayEvent(void)
{
    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    vlcConnectionPointContainer->fireEvent(DISPID_PlayEvent, &dispparamsNoArgs);
};

void VLCPlugin::fireOnPauseEvent(void)
{
    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    vlcConnectionPointContainer->fireEvent(DISPID_PauseEvent, &dispparamsNoArgs);
};

void VLCPlugin::fireOnStopEvent(void)
{
    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    vlcConnectionPointContainer->fireEvent(DISPID_StopEvent, &dispparamsNoArgs);
};

/*
 * Async events
 */
void VLCPlugin::fireOnMediaPlayerMediaChangedEvent()
{
    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerMediaChangedEvent, &dispparamsNoArgs);
};

void VLCPlugin::fireOnMediaPlayerNothingSpecialEvent()
{
    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerNothingSpecialEvent, &dispparamsNoArgs);
};

void VLCPlugin::fireOnMediaPlayerOpeningEvent()
{
    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerOpeningEvent, &dispparamsNoArgs);
};

void VLCPlugin::fireOnMediaPlayerBufferingEvent(float cache)
{
    DISPPARAMS params;
    params.cArgs = 1;
    params.rgvarg = (VARIANTARG *) CoTaskMemAlloc(sizeof(VARIANTARG) * params.cArgs) ;
    memset(params.rgvarg, 0, sizeof(VARIANTARG) * params.cArgs);
    params.rgvarg[0].vt = VT_I4;
    params.rgvarg[0].lVal = static_cast<LONG>(cache);
    params.rgdispidNamedArgs = NULL;
    params.cNamedArgs = 0;
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerBufferingEvent, &params);
};

void VLCPlugin::fireOnMediaPlayerPlayingEvent()
{
    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerPlayingEvent, &dispparamsNoArgs);
};

void VLCPlugin::fireOnMediaPlayerPausedEvent()
{
    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerPausedEvent, &dispparamsNoArgs);
};

void VLCPlugin::fireOnMediaPlayerEncounteredErrorEvent()
{
    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerEncounteredErrorEvent, &dispparamsNoArgs);
};

void VLCPlugin::fireOnMediaPlayerEndReachedEvent()
{
    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerEndReachedEvent, &dispparamsNoArgs);
};

void VLCPlugin::fireOnMediaPlayerStoppedEvent()
{
    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerStoppedEvent, &dispparamsNoArgs);
};

void VLCPlugin::fireOnMediaPlayerForwardEvent()
{
    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerForwardEvent, &dispparamsNoArgs);
};

void VLCPlugin::fireOnMediaPlayerBackwardEvent()
{
    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerBackwardEvent, &dispparamsNoArgs);
};

static void handle_input_state_event(const libvlc_event_t* event, void *param)
{
    VLCPlugin *plugin = (VLCPlugin*)param;
    switch( event->type )
    {
        case libvlc_MediaPlayerMediaChanged:
            plugin->fireOnMediaPlayerMediaChangedEvent();
            break;        
        case libvlc_MediaPlayerNothingSpecial:
            plugin->fireOnMediaPlayerNothingSpecialEvent();
            break;
        case libvlc_MediaPlayerOpening:
            plugin->fireOnMediaPlayerOpeningEvent();
            break;
        case libvlc_MediaPlayerBuffering:
            plugin->fireOnMediaPlayerBufferingEvent(event->u.media_player_buffering.new_cache);
            break;
        case libvlc_MediaPlayerPlaying:
            plugin->fireOnMediaPlayerPlayingEvent();
            break;
        case libvlc_MediaPlayerPaused:
            plugin->fireOnMediaPlayerPausedEvent();
            break;
        case libvlc_MediaPlayerStopped:
            plugin->fireOnMediaPlayerStoppedEvent();
            break;
        case libvlc_MediaPlayerForward:
            plugin->fireOnMediaPlayerForwardEvent();
            break;
        case libvlc_MediaPlayerBackward:
            plugin->fireOnMediaPlayerBackwardEvent();
            break;
        case libvlc_MediaPlayerEndReached:
            plugin->fireOnMediaPlayerEndReachedEvent();
            break;
        case libvlc_MediaPlayerEncounteredError:
            plugin->fireOnMediaPlayerEncounteredErrorEvent();
            break;
    }
}

void VLCPlugin::fireOnMediaPlayerTimeChangedEvent(libvlc_time_t  time)
{
    DISPPARAMS params;
    params.cArgs = 1;
    params.rgvarg = (VARIANTARG *) CoTaskMemAlloc(sizeof(VARIANTARG) * params.cArgs) ;
    memset(params.rgvarg, 0, sizeof(VARIANTARG) * params.cArgs);
    params.rgvarg[0].vt = VT_I4;
    params.rgvarg[0].lVal = static_cast<LONG>(time);
    params.rgdispidNamedArgs = NULL;
    params.cNamedArgs = 0;
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerTimeChangedEvent, &params);
};

static void handle_time_changed_event(const libvlc_event_t* event, void *param)
{
    VLCPlugin *plugin = (VLCPlugin*)param;
    plugin->fireOnMediaPlayerTimeChangedEvent(event->u.media_player_time_changed.new_time);
}

void VLCPlugin::fireOnMediaPlayerPositionChangedEvent(float position)
{
    DISPPARAMS params;
    params.cArgs = 1;
    params.rgvarg = (VARIANTARG *) CoTaskMemAlloc(sizeof(VARIANTARG) * params.cArgs) ;
    memset(params.rgvarg, 0, sizeof(VARIANTARG) * params.cArgs);
    params.rgvarg[0].vt = VT_R4;
    params.rgvarg[0].fltVal = position;
    params.rgdispidNamedArgs = NULL;
    params.cNamedArgs = 0;
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerPositionChangedEvent, &params);
};

static void handle_position_changed_event(const libvlc_event_t* event, void *param)
{
    VLCPlugin *plugin = (VLCPlugin*)param;
    plugin->fireOnMediaPlayerPositionChangedEvent(event->u.media_player_position_changed.new_position);
}

#define B(val) ((val) ? 0xFFFF : 0x0000)
void VLCPlugin::fireOnMediaPlayerSeekableChangedEvent(VARIANT_BOOL seekable)
{
    DISPPARAMS params;
    params.cArgs = 1;
    params.rgvarg = (VARIANTARG *) CoTaskMemAlloc(sizeof(VARIANTARG) * params.cArgs) ;
    memset(params.rgvarg, 0, sizeof(VARIANTARG) * params.cArgs);
    params.rgvarg[0].vt = VT_BOOL;
    params.rgvarg[0].boolVal = seekable;
    params.rgdispidNamedArgs = NULL;
    params.cNamedArgs = 0;
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerSeekableChangedEvent, &params);
};

static void handle_seekable_changed_event(const libvlc_event_t* event, void *param)
{
    VLCPlugin *plugin = (VLCPlugin*)param;
    plugin->fireOnMediaPlayerSeekableChangedEvent(B(event->u.media_player_seekable_changed.new_seekable));
}

void VLCPlugin::fireOnMediaPlayerPausableChangedEvent(VARIANT_BOOL pausable)
{
    DISPPARAMS params;
    params.cArgs = 1;
    params.rgvarg = (VARIANTARG *) CoTaskMemAlloc(sizeof(VARIANTARG) * params.cArgs) ;
    memset(params.rgvarg, 0, sizeof(VARIANTARG) * params.cArgs);
    params.rgvarg[0].vt = VT_BOOL;
    params.rgvarg[0].boolVal = pausable;
    params.rgdispidNamedArgs = NULL;
    params.cNamedArgs = 0;
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerPausableChangedEvent, &params);
};

static void handle_pausable_changed_event(const libvlc_event_t* event, void *param)
{
    VLCPlugin *plugin = (VLCPlugin*)param;
    plugin->fireOnMediaPlayerPausableChangedEvent(B(event->u.media_player_pausable_changed.new_pausable));
}
#undef B

void VLCPlugin::fireOnMediaPlayerTitleChangedEvent(int title)
{
    DISPPARAMS params;
    params.cArgs = 1;
    params.rgvarg = (VARIANTARG *) CoTaskMemAlloc(sizeof(VARIANTARG) * params.cArgs) ;
    memset(params.rgvarg, 0, sizeof(VARIANTARG) * params.cArgs);
    params.rgvarg[0].vt = VT_I2;
    params.rgvarg[0].iVal = title;
    params.rgdispidNamedArgs = NULL;
    params.cNamedArgs = 0;
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerTitleChangedEvent, &params);
};

static void handle_title_changed_event(const libvlc_event_t* event, void *param)
{
    VLCPlugin *plugin = (VLCPlugin*)param;
    plugin->fireOnMediaPlayerTitleChangedEvent(event->u.media_player_title_changed.new_title);
}

void VLCPlugin::fireOnMediaPlayerLengthChangedEvent(long length)
{
    DISPPARAMS params;
    params.cArgs = 1;
    params.rgvarg = (VARIANTARG *) CoTaskMemAlloc(sizeof(VARIANTARG) * params.cArgs) ;
    memset(params.rgvarg, 0, sizeof(VARIANTARG) * params.cArgs);
    params.rgvarg[0].vt = VT_I4;
    params.rgvarg[0].lVal = length;
    params.rgdispidNamedArgs = NULL;
    params.cNamedArgs = 0;
    vlcConnectionPointContainer->fireEvent(DISPID_MediaPlayerLengthChangedEvent, &params);
};

static void handle_length_changed_event(const libvlc_event_t* event, void *param)
{
    VLCPlugin *plugin = (VLCPlugin*)param;
    plugin->fireOnMediaPlayerLengthChangedEvent(event->u.media_player_length_changed.new_length);
}

/* */

void VLCPlugin::set_player_window()
{
    _WindowsManager.LibVlcAttach( &get_player() );
}

void VLCPlugin::on_player_action(vlc_player_action_e pa)
{
    switch(pa){
    case pa_play:
        fireOnPlayEvent();
        break;
    case pa_pause:
        fireOnPauseEvent();
        break;
    case pa_stop:
        fireOnStopEvent();
        break;
    case pa_next:
    case pa_prev:
        break;
    }
}

static vlcplugin_event_t vlcevents[] = {
    { libvlc_MediaPlayerMediaChanged, handle_input_state_event },
    { libvlc_MediaPlayerNothingSpecial, handle_input_state_event },
    { libvlc_MediaPlayerOpening, handle_input_state_event },
    { libvlc_MediaPlayerBuffering, handle_input_state_event },
    { libvlc_MediaPlayerPlaying, handle_input_state_event },
    { libvlc_MediaPlayerPaused, handle_input_state_event },
    { libvlc_MediaPlayerStopped, handle_input_state_event },
    { libvlc_MediaPlayerForward, handle_input_state_event },
    { libvlc_MediaPlayerBackward, handle_input_state_event },
    { libvlc_MediaPlayerEndReached, handle_input_state_event },
    { libvlc_MediaPlayerEncounteredError, handle_input_state_event },
    { libvlc_MediaPlayerTimeChanged, handle_time_changed_event },
    { libvlc_MediaPlayerPositionChanged, handle_position_changed_event },
    { libvlc_MediaPlayerSeekableChanged, handle_seekable_changed_event },
    { libvlc_MediaPlayerPausableChanged, handle_pausable_changed_event },
    { libvlc_MediaPlayerTitleChanged, handle_title_changed_event },
    { libvlc_MediaPlayerLengthChanged, handle_length_changed_event },
};

void VLCPlugin::player_register_events()
{
    libvlc_event_manager_t *eventManager = NULL;
    assert(vlc_player::is_open());

    eventManager = libvlc_media_player_event_manager(vlc_player::get_mp());
    if(eventManager) {
        /* attach all libvlc events we care about */
        for( size_t i = 0; i < ARRAY_SIZE(vlcevents); i++ )
        {
            libvlc_event_attach( eventManager, vlcevents[i].libvlc_type,
                                               vlcevents[i].libvlc_callback,
                                               this );
        }
    }
}

void VLCPlugin::player_unregister_events()
{
    libvlc_event_manager_t *eventManager = NULL;
    assert(vlc_player::is_open());

    eventManager = libvlc_media_player_event_manager(vlc_player::get_mp());
    if(eventManager) {
        /* detach all libvlc events we cared about */
        for( size_t i = 0; i < ARRAY_SIZE(vlcevents); i++ )
        {
            libvlc_event_detach( eventManager, vlcevents[i].libvlc_type,
                                               vlcevents[i].libvlc_callback,
                                               this );
        }
    }
}
