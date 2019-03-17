/*****************************************************************************
 * vlcplugin_win.h: a VLC plugin for Mozilla (Windows interface)
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

#ifndef __VLCPLUGIN_WIN_H__
#define __VLCPLUGIN_WIN_H__

#include "vlcplugin_base.h"

#include <windows.h>
#include <winbase.h>

HMODULE DllGetModule();
#include "../common/win32_fullscreen.h"

class VlcPluginWin : public VlcPluginBase
{
public:
    VlcPluginWin(NPP, NPuint16_t);
    virtual ~VlcPluginWin();

    void toggle_fullscreen();
    void set_fullscreen( int );
    int  get_fullscreen();

    bool create_windows();
    bool resize_windows();
    bool destroy_windows();

    void show_toolbar();
    void hide_toolbar();

    void set_toolbar_visible(bool);
    bool get_toolbar_visible();

    void update_controls();
    void popup_menu(){};

protected:
    virtual void on_media_player_new();
    virtual void on_media_player_release();

private:
    static LRESULT CALLBACK NPWndProcR(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void set_player_window(){};

    WNDPROC _NPWndProc;
    VLCViewResources  _ViewRC;
    VLCWindowsManager _WindowsManager;
};

#endif /* __VLCPLUGIN_WIN_H__ */
