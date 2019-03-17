/*****************************************************************************
 * vlcplugin_win.cpp: a VLC plugin for Mozilla (Windows interface)
 *****************************************************************************
 * Copyright (C) 2011-2012 VLC authors and VideoLAN
 * $Id$
 *
 * Authors: Cheng Sun <chengsun9@gmail.com>
 *          Sergey Radionov <RSATom@gmail.com>
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

#include "vlcplugin_win.h"

#include "../common/win32_fullscreen.h"

static HMODULE hDllModule= 0;

HMODULE DllGetModule()
{
    return hDllModule;
};

extern "C"
BOOL WINAPI DllMain(HANDLE hModule, DWORD fdwReason, LPVOID lpReserved )
{
    (void)lpReserved;
    switch( fdwReason )
    {
        case DLL_PROCESS_ATTACH:
            hDllModule = (HMODULE)hModule;
            break;
        default:
            break;
    }
    return TRUE;
};


LRESULT CALLBACK VlcPluginWin::NPWndProcR(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG_PTR ud = GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if( ud ) {
        VlcPluginWin *p_plugin = reinterpret_cast<VlcPluginWin *>(ud);

        switch( uMsg ){
            case WM_ERASEBKGND:
                RECT rect;
                GetClientRect(hWnd, &rect);
                FillRect((HDC)wParam, &rect, (HBRUSH) (COLOR_WINDOW+1));
                return 1;
            case WM_LBUTTONDBLCLK:
                /*if we get WM_LBUTTONDBLCLK here,
                * it mean that fullscreen mode is on,
                * and fullscreen window is on another display.
                * So user can see browser window and
                * plugin drawable (i.e. this) window,
                * and might want switch fullscreen off*/
                p_plugin->set_fullscreen(false);
                return 0;
            case WM_DESTROY:
                // Opera does not call NPP_SetWindow on window destruction...
                p_plugin->destroy_windows();
                break;
        }

        /* delegate to default handler */
        return CallWindowProc( p_plugin->_NPWndProc, hWnd,
                               uMsg, wParam, lParam);
    }
    else
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

VlcPluginWin::VlcPluginWin(NPP instance, NPuint16_t mode) :
    VlcPluginBase(instance, mode), _NPWndProc(0),
    _WindowsManager(DllGetModule(), _ViewRC, &get_options())
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
}

VlcPluginWin::~VlcPluginWin()
{
    destroy_windows();
}

void VlcPluginWin::toggle_fullscreen()
{
    _WindowsManager.ToggleFullScreen();
}

void VlcPluginWin::set_fullscreen(int yes)
{
    if(yes){
        _WindowsManager.StartFullScreen();
    }
    else{
        _WindowsManager.EndFullScreen();
    }
}

int  VlcPluginWin::get_fullscreen()
{
    return _WindowsManager.IsFullScreen();
}

void VlcPluginWin::set_toolbar_visible(bool)
{
    /* TODO */
}

bool VlcPluginWin::get_toolbar_visible()
{
    /* TODO */
    return false;
}

void VlcPluginWin::update_controls()
{
    /* TODO */
}

bool VlcPluginWin::create_windows()
{
    HWND drawable = (HWND) (getWindow().window);

    if( GetWindowLongPtr(drawable, GWLP_USERDATA) )
        return false;

    /* attach our plugin object */
    SetWindowLongPtr(drawable, GWLP_USERDATA, (LONG_PTR)this);

    /* Some browsers (mainly opera) do not implement NPAPI correctly,
     * so we will need track some events via native window messages,
     * rather than via NPP_SetWindow. */
    _NPWndProc = (WNDPROC) SetWindowLongPtr( drawable,
                                             GWLP_WNDPROC,
                                             (LONG_PTR)NPWndProcR );

    /* change window style to our liking */
    LONG style = GetWindowLong(drawable, GWL_STYLE);
    style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    SetWindowLong(drawable, GWL_STYLE, style);

    _WindowsManager.CreateWindows(drawable);

    if( get_player().is_open() )
        _WindowsManager.LibVlcAttach(&get_player());

    return true;
}

bool VlcPluginWin::resize_windows()
{
    HWND drawable = (HWND) (getWindow().window);
    RECT rect;
    GetClientRect(drawable, &rect);
    if(!_WindowsManager.IsFullScreen() && _WindowsManager.getHolderWnd()){
        HWND hHolderWnd = _WindowsManager.getHolderWnd()->hWnd();
        MoveWindow(hHolderWnd, 0, 0, rect.right - rect.left, rect.bottom - rect.top, TRUE);
    }
    return true;
}

bool VlcPluginWin::destroy_windows()
{
    _WindowsManager.DestroyWindows();

    HWND hWnd = (HWND)npwindow.window;
    if( hWnd && _NPWndProc){
        /* reset WNDPROC */
        SetWindowLongPtr( hWnd, GWLP_WNDPROC, (LONG_PTR)_NPWndProc );
    }
    _NPWndProc = 0;
    npwindow.window = 0;

    SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
    return true;
}

void VlcPluginWin::on_media_player_new()
{
    _WindowsManager.LibVlcAttach(&get_player());
}

void VlcPluginWin::on_media_player_release()
{
    _WindowsManager.LibVlcDetach();
}
