/*****************************************************************************
 * vlcwindowless_base.h: window-less base class for the VLC plugin
 *****************************************************************************
 * Copyright (C) 2012-2013 VLC Authors and VideoLAN
 * $Id$
 *
 * Authors: Sergey Radionov <rsatom@gmail.com>
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

/*******************************************************************************
 * Instance state information about the plugin.
 ******************************************************************************/
#ifndef __VLCWINDOWLESS_BASE_H__
#define __VLCWINDOWLESS_BASE_H__

#include "vlcplugin_base.h"

#ifdef XP_MACOSX
const char DEF_CHROMA[] = "RGBA";
#else
const char DEF_CHROMA[] = "RV32";
#endif
enum{
    DEF_PIXEL_BYTES = 4
};

class VlcWindowlessBase : public VlcPluginBase
{
public:
    VlcWindowlessBase(NPP, NPuint16_t);

    //for libvlc_video_set_format_callbacks
    static unsigned video_format_proxy(void **opaque, char *chroma,
                                       unsigned *width, unsigned *height,
                                       unsigned *pitches, unsigned *lines)
        { return reinterpret_cast<VlcWindowlessBase*>(*opaque)->video_format_cb(chroma,
                                                                  width, height,
                                                                  pitches, lines); }
    static void video_cleanup_proxy(void *opaque)
        { reinterpret_cast<VlcWindowlessBase*>(opaque)->video_cleanup_cb(); };

    unsigned video_format_cb(char *chroma,
                             unsigned *width, unsigned *height,
                             unsigned *pitches, unsigned *lines);
    void video_cleanup_cb();
    //end (for libvlc_video_set_format_callbacks)

    //for libvlc_video_set_callbacks
    static void* video_lock_proxy(void *opaque, void **planes)
        { return reinterpret_cast<VlcWindowlessBase*>(opaque)->video_lock_cb(planes); }
    static void video_unlock_proxy(void *opaque, void *picture, void *const *planes)
        { reinterpret_cast<VlcWindowlessBase*>(opaque)->video_unlock_cb(picture, planes); }
    static void video_display_proxy(void *opaque, void *picture)
        { reinterpret_cast<VlcWindowlessBase*>(opaque)->video_display_cb(picture); }

    void* video_lock_cb(void **planes);
    void video_unlock_cb(void *picture, void *const *planes);
    void video_display_cb(void *picture);
    //end (for libvlc_video_set_callbacks)

    static void invalidate_window_proxy(void *opaque)
        { reinterpret_cast<VlcWindowlessBase*>(opaque)->invalidate_window(); }
    void invalidate_window();

    void set_player_window();


    bool create_windows() { return true; }
    bool resize_windows() { return true; }
    bool destroy_windows() { return true; }

    void toggle_fullscreen() { /* STUB */ }
    void set_fullscreen( int ) { /* STUB */ }
    int  get_fullscreen() { return false; }

    void set_toolbar_visible(bool)  { /* STUB */ }
    bool get_toolbar_visible()  { return false; }
    void update_controls()      {/* STUB */}
    void popup_menu()           {/* STUB */}

protected:
    std::vector<char> m_frame_buf;
    unsigned int m_media_width;
    unsigned int m_media_height;
};
#endif
