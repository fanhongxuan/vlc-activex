/*****************************************************************************
 * vlcplugin_mac.h: a VLC plugin for Mozilla (Mac interface)
 *****************************************************************************
 * Copyright (C) 2011-2015 VLC authors and VideoLAN
 * $Id$
 *
 * Authors: Felix Paul Kühne <fkuehne # videolan # org>
 *          Cheng Sun <chengsun9@gmail.com>
 *          Jean-Baptiste Kempf <jb@videolan.org>
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

#ifndef __VLCPLUGIN_MAC_H__
#define __VLCPLUGIN_MAC_H__

#include "vlcplugin_base.h"

//@class VLCPerInstanceStorage;

class VlcPluginMac : public VlcPluginBase
{
public:
    VlcPluginMac(NPP, NPuint16_t);
    virtual ~VlcPluginMac();

    void toggle_fullscreen();
    void set_fullscreen( int );
    int  get_fullscreen();

    bool create_windows();
    bool resize_windows();
    bool destroy_windows();

    void set_toolbar_visible(bool);
    bool get_toolbar_visible();
    void update_controls();
    void popup_menu()           {/* STUB */}

    bool handle_event(void *event);
    NPError get_root_layer(void *value);

    std::vector<char> m_frame_buf;
    float m_media_width;
    float m_media_height;

    void *_perInstanceStorage;
    bool runningWithinFirefox;

private:

    void set_player_window();
};

#endif /* __VLCPLUGIN_MAC_H__ */
