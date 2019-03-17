/*****************************************************************************
 * vlcplugin_xcb.cpp: a VLC plugin for Mozilla (X interface)
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

#include "vlcplugin_xcb.h"

#include <cstring>
#include <cstdlib>

/* the latest version of XEmbed that we support */
#define PLUGIN_XEMBED_PROTOCOL_VERSION 1

/* Flags for _XEMBED_INFO */
#define XEMBED_MAPPED                   (1 << 0)


VlcPluginXcb::VlcPluginXcb(NPP instance, NPuint16_t mode) :
    VlcPluginBase(instance, mode)
{
    memset(&parent, 0, sizeof(Window));
    memset(&video, 0, sizeof(Window));
}

VlcPluginXcb::~VlcPluginXcb()
{
}

void VlcPluginXcb::set_player_window()
{
    libvlc_media_player_set_xwindow(get_player().get_mp(),
                                    (uint32_t) video);
}

void VlcPluginXcb::toggle_fullscreen()
{
    if (!get_options().get_enable_fs()) return;
    if (playlist_isplaying())
        libvlc_toggle_fullscreen(get_player().get_mp());
}

void VlcPluginXcb::set_fullscreen(int yes)
{
    if (!get_options().get_enable_fs()) return;
    if (playlist_isplaying())
        libvlc_set_fullscreen(get_player().get_mp(),yes);
}

int  VlcPluginXcb::get_fullscreen()
{
    int r = 0;
    if (playlist_isplaying())
        r = libvlc_get_fullscreen(get_player().get_mp());
    return r;
}

bool VlcPluginXcb::create_windows()
{
    Display *npdisplay = ( (NPSetWindowCallbackStruct *) npwindow.ws_info )->display;
    Window socket = (Window) npwindow.window;
    conn = xcb_connect(XDisplayString(npdisplay), NULL);

    const xcb_setup_t *setup = xcb_get_setup(conn);
    xcb_screen_t *screen = xcb_setup_roots_iterator(setup).data;

    uint32_t xembed_info_buf[2] =
            { PLUGIN_XEMBED_PROTOCOL_VERSION, XEMBED_MAPPED };
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(conn, 0, strlen("_XEMBED_INFO"), "_XEMBED_INFO");
    xcb_atom_t xembed_info_atom = xcb_intern_atom_reply(conn, cookie, NULL)->atom;

    /* create windows */
    const uint32_t parent_values[] = {0x0FFFFF};
    parent = xcb_generate_id(conn);
    xcb_create_window(conn, XCB_COPY_FROM_PARENT, parent, socket,
                     /* FIXME: figure out why the window refuses to be resized larger than initial size */
                      0, 0, 20000, 20000, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual, XCB_CW_EVENT_MASK, parent_values);
    xcb_change_property(conn, XCB_PROP_MODE_REPLACE, parent,
                        xembed_info_atom, xembed_info_atom,
                        32, 2, (void *) xembed_info_buf);

    colormap = screen->default_colormap;
    unsigned r = 0, g = 0, b = 0;
    HTMLColor2RGB(get_options().get_bg_color().c_str(), &r, &g, &b);
    xcb_alloc_color_reply_t *reply = xcb_alloc_color_reply(conn,
            xcb_alloc_color(conn, colormap,
                            (uint16_t) r << 8,
                            (uint16_t) g << 8,
                            (uint16_t) b << 8), NULL);
    colorpixel = reply->pixel;
    free(reply);

    const uint32_t video_values[] = {colorpixel, 0x0FFFFF};
    video = xcb_generate_id(conn);
    xcb_create_window(conn, 0, video, parent,
                      0, 0, 1, 1, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual, XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, video_values);
    xcb_map_window(conn, video);
    xcb_flush(conn);

    return true;
}

bool VlcPluginXcb::resize_windows()
{
    const uint32_t dims[] = {npwindow.width, npwindow.height};
    xcb_configure_window(conn, video, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, dims);
    xcb_configure_window(conn, parent, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, dims);

    xcb_query_tree_cookie_t query_cookie = xcb_query_tree(conn, video);
    xcb_query_tree_reply_t *query_reply  = xcb_query_tree_reply(conn, query_cookie, NULL);

    if (query_reply) {
        /* XXX: Make assumptions related to the window parenting structure in
           vlc/modules/video_output/x11/xcommon.c */
        xcb_window_t *children = xcb_query_tree_children(query_reply);
        xcb_configure_window(conn, children[query_reply->children_len - 1], XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, dims);
    }
    xcb_flush(conn);

    free(query_reply);
    return true;
}

bool VlcPluginXcb::destroy_windows()
{
    xcb_destroy_window(conn, parent);
    xcb_free_colors(conn, colormap, 0, 1, &colorpixel);
    xcb_disconnect(conn);
}
