/*****************************************************************************
 * vlcplugin_xcb.h: a VLC plugin for Mozilla (X interface)
 *****************************************************************************
 * Copyright (C) 2011-2012 VLC authors and VideoLAN
 * $Id$
 *
 * Authors: Cheng Sun <chengsun9@gmail.com>
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

#ifndef __VLCPLUGIN_XCB_H__
#define __VLCPLUGIN_XCB_H__

#include "vlcplugin_base.h"
#include <xcb/xcb.h>

class VlcPluginXcb : public VlcPluginBase
{
public:
    VlcPluginXcb(NPP, NPuint16_t);
    virtual ~VlcPluginXcb();

    int                 setSize(unsigned width, unsigned height);

    void toggle_fullscreen();
    void set_fullscreen( int );
    int  get_fullscreen();

    bool create_windows();
    bool resize_windows();
    bool destroy_windows();

    void set_toolbar_visible(bool)  { /* STUB */ }
    bool get_toolbar_visible()  { return false; }
    void update_controls()      {/* STUB */}
    void popup_menu()           {/* STUB */}

    void set_player_window();

    unsigned int     i_width, i_height;
    xcb_connection_t *conn;
    xcb_window_t parent, video;
    xcb_colormap_t colormap;
    uint32_t colorpixel;

    pthread_t thread;

    int i_last_position;
};

#endif /* __VLCPLUGIN_XCB_H__ */
