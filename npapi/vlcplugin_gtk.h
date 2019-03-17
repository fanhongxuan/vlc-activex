/*****************************************************************************
 * vlcplugin_gtk.h: a VLC plugin for Mozilla (GTK+ interface)
 *****************************************************************************
 * Copyright (C) 2012 VLC authors and VideoLAN
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

#ifndef __VLCPLUGIN_GTK_H__
#define __VLCPLUGIN_GTK_H__

#include "vlcplugin_base.h"

#include <gtk/gtk.h>
#include <X11/Xlib.h>

#define VLCPLUGINGTK_MENU_TOOLBAR "Show toolbar"

class VlcPluginGtk : public VlcPluginBase
{
public:
    VlcPluginGtk(NPP, NPuint16_t);
    virtual ~VlcPluginGtk();

    bool create_windows();
    bool resize_windows();
    bool destroy_windows();

    void toggle_fullscreen();
    void set_fullscreen(int);
    int get_fullscreen();
    void do_set_fullscreen(bool);

    void set_toolbar_visible(bool);
    bool get_toolbar_visible();
    void update_controls();
    void popup_menu();

    void resize_video_xwindow(GdkRectangle *rect);

    GdkPixbuf *cone_icon;
    GtkWidget *time_slider, *vol_slider;
    guint time_slider_timeout_id, vol_slider_timeout_id;
private:
    void set_player_window();
    Display *get_display() { return display; }

    unsigned int     i_width, i_height;
    GtkWidget *parent, *parent_vbox, *video_container;
    GtkWidget *fullscreen_win;
    GtkWidget *toolbar;
    gulong video_container_size_handler_id;

    Display *display;
    Window video_xwindow;
    XColor bg_color;
    bool is_fullscreen, is_toolbar_visible;
};

#endif /* __VLCPLUGIN_GTK_H__ */
