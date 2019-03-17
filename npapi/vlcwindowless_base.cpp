/*****************************************************************************
 * vlcwindowless_base.cpp: window-less base class for the VLC plugin
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

#include "vlcwindowless_base.h"

VlcWindowlessBase::VlcWindowlessBase(NPP instance, NPuint16_t mode) :
    VlcPluginBase(instance, mode), m_media_width(0), m_media_height(0)
{
}

unsigned VlcWindowlessBase::video_format_cb(char *chroma,
                                unsigned *width, unsigned *height,
                                unsigned *pitches, unsigned *lines)
{
    if ( p_browser ) {
        float src_aspect = (float)(*width) / (*height);
        float dst_aspect = (float)npwindow.width/npwindow.height;
        if ( src_aspect > dst_aspect ) {
            if( npwindow.width != (*width) ) { //don't scale if size equal
                (*width) = npwindow.width;
                (*height) = static_cast<unsigned>( (*width) / src_aspect + 0.5);
            }
        }
        else {
            if( npwindow.height != (*height) ) { //don't scale if size equal
                (*height) = npwindow.height;
                (*width) = static_cast<unsigned>( (*height) * src_aspect + 0.5);
            }
        }
    }

    m_media_width = (*width);
    m_media_height = (*height);

    memcpy(chroma, DEF_CHROMA, sizeof(DEF_CHROMA)-1);
    (*pitches) = m_media_width * DEF_PIXEL_BYTES;
    (*lines) = m_media_height;

    //+1 for vlc 2.0.3/2.1 bug workaround.
    //They writes after buffer end boundary by some reason unknown to me...
    m_frame_buf.resize( (*pitches) * ((*lines)+1) );

    return 1;
}

void VlcWindowlessBase::video_cleanup_cb()
{
    m_frame_buf.resize(0);
    m_media_width = 0;
    m_media_height = 0;
}

void* VlcWindowlessBase::video_lock_cb(void **planes)
{
    (*planes) = m_frame_buf.empty()? 0 : &m_frame_buf[0];
    return 0;
}

void VlcWindowlessBase::video_unlock_cb(void* /*picture*/, void *const * /*planes*/)
{
}

void VlcWindowlessBase::invalidate_window()
{
    NPRect rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = npwindow.width;
    rect.bottom = npwindow.height;
    NPN_InvalidateRect(p_browser, &rect);
    NPN_ForceRedraw(p_browser);
}

void VlcWindowlessBase::video_display_cb(void * /*picture*/)
{
    if (p_browser) {
        NPN_PluginThreadAsyncCall(p_browser,
                                  VlcWindowlessBase::invalidate_window_proxy,
                                  this);
    }
}

void VlcWindowlessBase::set_player_window() {
    libvlc_video_set_format_callbacks(getMD(),
                                      video_format_proxy,
                                      video_cleanup_proxy);
    libvlc_video_set_callbacks(getMD(),
                               video_lock_proxy,
                               video_unlock_proxy,
                               video_display_proxy,
                               this);
}

