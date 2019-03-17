/*****************************************************************************
 * vlcplugin_gtk.cpp: a VLC plugin for Mozilla (GTK+ interface)
 *****************************************************************************
 * Copyright (C) 2002-2012 VLC authors and VideoLAN
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

#include "vlcplugin_gtk.h"
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>
#include <cstring>

static uint32_t get_xid(GtkWidget *widget)
{
    GdkDrawable *video_drawable = gtk_widget_get_window(widget);
    return (uint32_t)gdk_x11_drawable_get_xid(video_drawable);
}

VlcPluginGtk::VlcPluginGtk(NPP instance, NPuint16_t mode) :
    VlcPluginBase(instance, mode),
    parent(NULL),
    parent_vbox(NULL),
    video_container(NULL),
    toolbar(NULL),
    time_slider(NULL),
    vol_slider(NULL),
    fullscreen_win(NULL),
    is_fullscreen(false),
    is_toolbar_visible(false),
    time_slider_timeout_id(0),
    vol_slider_timeout_id(0)
{
    memset(&video_xwindow, 0, sizeof(Window));
    GtkIconTheme *icon_theme = gtk_icon_theme_get_default();
    cone_icon = gdk_pixbuf_copy(gtk_icon_theme_load_icon(
                    icon_theme, "vlc", 128, GTK_ICON_LOOKUP_FORCE_SIZE, NULL));
    if (!cone_icon) {
        fprintf(stderr, "WARNING: could not load VLC icon\n");
    }
}

VlcPluginGtk::~VlcPluginGtk()
{
}

void VlcPluginGtk::set_player_window()
{
    libvlc_media_player_set_xwindow(get_player().get_mp(),
                                    video_xwindow);
    libvlc_video_set_mouse_input(get_player().get_mp(), 0);
}

void VlcPluginGtk::toggle_fullscreen()
{
    set_fullscreen(!get_fullscreen());
}

void VlcPluginGtk::do_set_fullscreen(bool yes)
{
    /* we have to reparent windows */
    /* note that the xid of video_container changes after reparenting */
    Display *display = get_display();
    g_signal_handler_block(video_container, video_container_size_handler_id);

    XUnmapWindow(display, video_xwindow);
    XReparentWindow(display, video_xwindow,
                    gdk_x11_get_default_root_xwindow(), 0, 0);
    if (yes) {
        g_object_ref(G_OBJECT(parent_vbox));
        gtk_container_remove(GTK_CONTAINER(parent), parent_vbox);
        gtk_container_add(GTK_CONTAINER(fullscreen_win), parent_vbox);
        g_object_unref(G_OBJECT(parent_vbox));
        gtk_widget_show_all(fullscreen_win);
        gtk_window_fullscreen(GTK_WINDOW(fullscreen_win));
    } else {
        gtk_widget_hide(fullscreen_win);
        g_object_ref(G_OBJECT(parent_vbox));
        gtk_container_remove(GTK_CONTAINER(fullscreen_win), parent_vbox);
        gtk_container_add(GTK_CONTAINER(parent), parent_vbox);
        g_object_unref(G_OBJECT(parent_vbox));
        gtk_widget_show_all(GTK_WIDGET(parent));
    }
    XSync(get_display(), false);
    XReparentWindow(display, video_xwindow, get_xid(video_container), 0, 0);

//    libvlc_set_fullscreen(libvlc_media_player, yes);
    g_signal_handler_unblock(video_container, video_container_size_handler_id);
    gtk_widget_queue_resize(video_container);
    update_controls();

    is_fullscreen = yes;
}

void VlcPluginGtk::set_fullscreen(int yes)
{
    if (!get_options().get_enable_fs()) return;
    if (yes == is_fullscreen) return;
    if (yes) {
        gtk_widget_show(fullscreen_win);
    } else {
        gtk_widget_hide(fullscreen_win);
    }
}

int VlcPluginGtk::get_fullscreen()
{
    return is_fullscreen;
}

void VlcPluginGtk::set_toolbar_visible(bool yes)
{
    if (yes == is_toolbar_visible) return;

    if (yes) {
        gtk_box_pack_start(GTK_BOX(parent_vbox), toolbar, false, false, 0);
        gtk_widget_show_all(toolbar);
        update_controls();
        g_object_unref(G_OBJECT(toolbar));
    } else {
        g_object_ref(G_OBJECT(toolbar));
        gtk_widget_hide(toolbar);
        gtk_container_remove(GTK_CONTAINER(parent_vbox), toolbar);
    }
    resize_windows();
    gtk_container_resize_children(GTK_CONTAINER(parent));
    is_toolbar_visible = yes;
}

bool VlcPluginGtk::get_toolbar_visible()
{
    return is_toolbar_visible;
}

void VlcPluginGtk::resize_video_xwindow(GdkRectangle *rect)
{
    Display *display = get_display();
    XResizeWindow(display, video_xwindow,
                  rect->width, rect->height);
    XSync(display, false);
}

struct tool_actions_t
{
    const gchar *stock_id;
    vlc_toolbar_clicked_t clicked;
};
static const tool_actions_t tool_actions[] = {
    {GTK_STOCK_MEDIA_PLAY, clicked_Play},
    {GTK_STOCK_MEDIA_PAUSE, clicked_Pause},
    {GTK_STOCK_MEDIA_STOP, clicked_Stop},
    {"gtk-volume-muted", clicked_Mute},
    {"gtk-volume-unmuted", clicked_Unmute},
    {GTK_STOCK_FULLSCREEN, clicked_Fullscreen}
};

static void toolbar_handler(GtkToolButton *btn, gpointer user_data)
{
    VlcPluginGtk *plugin = (VlcPluginGtk *) user_data;
    const gchar *stock_id = gtk_tool_button_get_stock_id(btn);
    for (int i = 0; i < sizeof(tool_actions)/sizeof(tool_actions_t); ++i) {
        if (!strcmp(stock_id, tool_actions[i].stock_id)) {
            plugin->control_handler(tool_actions[i].clicked);
            return;
        }
    }
    fprintf(stderr, "WARNING: No idea what toolbar button you just clicked on (%s)\n", stock_id?stock_id:"NULL");
}

static void menu_handler(GtkMenuItem *menuitem, gpointer user_data)
{
    VlcPluginGtk *plugin = (VlcPluginGtk *) user_data;
    const gchar *stock_id = gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));
    if (!strcmp(stock_id, VLCPLUGINGTK_MENU_TOOLBAR)) {
        plugin->set_toolbar_visible(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem)));
        return;
    }
    for (int i = 0; i < sizeof(tool_actions)/sizeof(tool_actions_t); ++i) {
        if (!strcmp(stock_id, tool_actions[i].stock_id)) {
            plugin->control_handler(tool_actions[i].clicked);
            return;
        }
    }
    fprintf(stderr, "WARNING: No idea what menu item you just clicked on (%s)\n", stock_id?stock_id:"NULL");
}

void VlcPluginGtk::popup_menu()
{
    /* construct menu */
    GtkWidget *popupmenu = gtk_menu_new();
    GtkWidget *menuitem;

    /* play/pause */
    menuitem = gtk_image_menu_item_new_from_stock(
                        playlist_isplaying() ?
                        GTK_STOCK_MEDIA_PAUSE :
                        GTK_STOCK_MEDIA_PLAY, NULL);
    g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(menu_handler), this);
    gtk_menu_shell_append(GTK_MENU_SHELL(popupmenu), menuitem);
    /* stop */
    menuitem = gtk_image_menu_item_new_from_stock(
                                GTK_STOCK_MEDIA_STOP, NULL);
    g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(menu_handler), this);
    gtk_menu_shell_append(GTK_MENU_SHELL(popupmenu), menuitem);
    /* set fullscreen */
    if (get_options().get_enable_fs()) {
        menuitem = gtk_image_menu_item_new_from_stock(
                                    GTK_STOCK_FULLSCREEN, NULL);
        g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(menu_handler), this);
        gtk_menu_shell_append(GTK_MENU_SHELL(popupmenu), menuitem);
    }
    /* toolbar */
    menuitem = gtk_check_menu_item_new_with_label(
                                VLCPLUGINGTK_MENU_TOOLBAR);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menuitem), get_toolbar_visible());
    g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(menu_handler), this);
    gtk_menu_shell_append(GTK_MENU_SHELL(popupmenu), menuitem);

    /* show menu */
    gtk_widget_show_all(popupmenu);
    gtk_menu_attach_to_widget(GTK_MENU(popupmenu), video_container, NULL);
    gtk_menu_popup(GTK_MENU(popupmenu), NULL, NULL, NULL, NULL,
                   0, gtk_get_current_event_time());
}

static bool video_button_handler(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    VlcPluginGtk *plugin = (VlcPluginGtk *) user_data;
    if (event->button == 3 && event->type == GDK_BUTTON_PRESS) {
        plugin->popup_menu();
        return true;
    }
    if (event->button == 1 && event->type == GDK_2BUTTON_PRESS) {
        plugin->toggle_fullscreen();
    }
    return false;
}

static bool video_popup_handler(GtkWidget *widget, gpointer user_data)
{
    VlcPluginGtk *plugin = (VlcPluginGtk *) user_data;
    plugin->popup_menu();
    return true;
}

static bool video_size_handler(GtkWidget *widget, GdkRectangle *rect, gpointer user_data)
{
    VlcPluginGtk *plugin = (VlcPluginGtk *) user_data;
    plugin->resize_video_xwindow(rect);
    return true;
}

static bool video_expose_handler(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    VlcPluginGtk *plugin = (VlcPluginGtk *) user_data;
    GdkEventExpose *event_expose = (GdkEventExpose *) event;
    GdkWindow *window = event_expose->window;
    GdkPixbuf *cone_icon = plugin->cone_icon;
    if (!cone_icon) return false;

    int winwidth, winheight;
#   if GTK_CHECK_VERSION(2, 24, 0)
        winwidth  = gdk_window_get_width(window);
        winheight = gdk_window_get_height(window);
#   else
        gdk_drawable_get_size(GDK_DRAWABLE(window), &winwidth, &winheight);
#   endif

    int iconwidth  = gdk_pixbuf_get_width(cone_icon),
        iconheight = gdk_pixbuf_get_height(cone_icon);
    double widthratio  = (double) winwidth / iconwidth,
           heightratio = (double) winheight / iconheight;
    double sizeratio = widthratio < heightratio ? widthratio : heightratio;
    if (sizeratio < 1.0) {
        cone_icon = gdk_pixbuf_scale_simple(cone_icon, iconwidth * sizeratio, iconheight * sizeratio, GDK_INTERP_BILINEAR);
        if (!cone_icon) return false;
        iconwidth  = gdk_pixbuf_get_width(cone_icon);
        iconheight = gdk_pixbuf_get_height(cone_icon);
    }

    cairo_t *cr = gdk_cairo_create(window);
    gdk_cairo_set_source_pixbuf(cr, cone_icon,
            (winwidth-iconwidth)/2.0, (winheight-iconheight)/2.0);
    gdk_cairo_region(cr, event_expose->region);
    cairo_fill(cr);
    cairo_destroy(cr);

    return true;
}

static gboolean do_time_slider_handler(gpointer user_data)
{
    VlcPluginGtk *plugin = (VlcPluginGtk *) user_data;
    libvlc_media_player_t *md = plugin->getMD();
    if (md) {
        gdouble value = gtk_range_get_value(GTK_RANGE(plugin->time_slider));
        libvlc_media_player_set_position(md, value/100.0);
    }

    plugin->time_slider_timeout_id = 0;
    return FALSE;
}

static bool time_slider_handler(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data)
{
    VlcPluginGtk *plugin = (VlcPluginGtk *) user_data;
    if (plugin->time_slider_timeout_id != 0)
        return false;

    plugin->time_slider_timeout_id = g_timeout_add(500,
                                                  do_time_slider_handler,
                                                  user_data);
    return false;
}

static gboolean do_vol_slider_handler(gpointer user_data)
{
    VlcPluginGtk *plugin = (VlcPluginGtk *) user_data;
    libvlc_media_player_t *md = plugin->getMD();
    if (md) {
        gdouble value = gtk_range_get_value(GTK_RANGE(plugin->vol_slider));
        libvlc_audio_set_volume(md, value);
    }

    plugin->vol_slider_timeout_id = 0;
    return FALSE;
}

static bool vol_slider_handler(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data)
{
    VlcPluginGtk *plugin = (VlcPluginGtk *) user_data;
    if (plugin->vol_slider_timeout_id != 0)
        return false;

    plugin->vol_slider_timeout_id = g_timeout_add(100,
                                                  do_vol_slider_handler,
                                                  user_data);
    return false;
}

static void fullscreen_win_visibility_handler(GtkWidget *widget, gpointer user_data)
{
    VlcPluginGtk *plugin = (VlcPluginGtk *) user_data;
    plugin->do_set_fullscreen(gtk_widget_get_visible(widget));
}

static gboolean fullscreen_win_keypress_handler(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    VlcPluginGtk *plugin = (VlcPluginGtk *) user_data;
    switch (event->keyval)
    {
    case GDK_KEY_space:
        plugin->playlist_togglePause();
        return True;
    case GDK_KEY_Escape:
        plugin->set_fullscreen(false);
        return True;
    default:
        return False;
    }
}

void VlcPluginGtk::update_controls()
{
    if (get_player().is_open()) {
        libvlc_state_t state = libvlc_media_player_get_state(get_player().get_mp());
        bool is_stopped = (state == libvlc_Stopped) ||
                          (state == libvlc_Ended) ||
                          (state == libvlc_Error);
        if (is_stopped) {
            XUnmapWindow(display, video_xwindow);
        } else {
            XMapWindow(display, video_xwindow);
        }
    }

    if (get_toolbar_visible()) {
        GtkToolItem *toolbutton;

        /* play/pause button */
        const gchar *stock_id = playlist_isplaying() ? GTK_STOCK_MEDIA_PAUSE : GTK_STOCK_MEDIA_PLAY;
        toolbutton = gtk_toolbar_get_nth_item(GTK_TOOLBAR(toolbar), 0);
        if (strcmp(gtk_tool_button_get_stock_id(GTK_TOOL_BUTTON(toolbutton)), stock_id)) {
            gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(toolbutton), stock_id);
            /* work around firefox not displaying the icon properly after change */
            g_object_ref(toolbutton);
            gtk_container_remove(GTK_CONTAINER(toolbar), GTK_WIDGET(toolbutton));
            gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbutton, 0);
            g_object_unref(toolbutton);
        }

        /* toolbar sensitivity */
        gtk_widget_set_sensitive(toolbar, get_player().is_open() );

        /* time slider */
        if (!get_player().is_open() ||
                !libvlc_media_player_is_seekable(get_player().get_mp())) {
            gtk_widget_set_sensitive(time_slider, false);
            gtk_range_set_value(GTK_RANGE(time_slider), 0);
        } else {
            gtk_widget_set_sensitive(time_slider, true);
            gdouble timepos = 100*libvlc_media_player_get_position(get_player().get_mp());
            if (time_slider_timeout_id == 0) {
                /* only set the time if the user is not dragging the slider */
                gtk_range_set_value(GTK_RANGE(time_slider), timepos);
            }
        }

        gtk_widget_show_all(toolbar);
    }
}

bool VlcPluginGtk::create_windows()
{
    display = ( (NPSetWindowCallbackStruct *) npwindow.ws_info )->display;

    Window socket = (Window) npwindow.window;
    GdkColor color_bg;
    gdk_color_parse(get_options().get_bg_color().c_str(), &color_bg);

    parent = gtk_plug_new(socket);
    gtk_widget_modify_bg(parent, GTK_STATE_NORMAL, &color_bg);
    gtk_widget_add_events(parent,
            GDK_BUTTON_PRESS_MASK
          | GDK_BUTTON_RELEASE_MASK);
    g_signal_connect(G_OBJECT(parent), "button-press-event", G_CALLBACK(video_button_handler), this);

    parent_vbox = gtk_vbox_new(false, 0);
    gtk_container_add(GTK_CONTAINER(parent), parent_vbox);

    video_container = gtk_drawing_area_new();
    gtk_widget_modify_bg(video_container, GTK_STATE_NORMAL, &color_bg);
    gtk_widget_add_events(video_container,
            GDK_BUTTON_PRESS_MASK
          | GDK_BUTTON_RELEASE_MASK);
    g_signal_connect(G_OBJECT(video_container), "expose-event", G_CALLBACK(video_expose_handler), this);
    g_signal_connect(G_OBJECT(video_container), "button-press-event", G_CALLBACK(video_button_handler), this);
    g_signal_connect(G_OBJECT(video_container), "popup-menu", G_CALLBACK(video_popup_handler), this);
    gtk_box_pack_start(GTK_BOX(parent_vbox), video_container, true, true, 0);

    gtk_widget_show_all(parent);

    /* fullscreen top-level */
    fullscreen_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_modify_bg(fullscreen_win, GTK_STATE_NORMAL, &color_bg);
    gtk_window_set_decorated(GTK_WINDOW(fullscreen_win), false);
    g_signal_connect(G_OBJECT(fullscreen_win), "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), this);
    g_signal_connect(G_OBJECT(fullscreen_win), "show", G_CALLBACK(fullscreen_win_visibility_handler), this);
    g_signal_connect(G_OBJECT(fullscreen_win), "hide", G_CALLBACK(fullscreen_win_visibility_handler), this);
    g_signal_connect(G_OBJECT(fullscreen_win), "key_press_event", G_CALLBACK(fullscreen_win_keypress_handler), this);

    /* actual video window */
    /* libvlc is handed this window's xid. A raw X window is used because
     * GTK+ is incapable of reparenting without changing xid
     */
    Display *display = get_display();
    Colormap colormap = DefaultColormap(display, DefaultScreen(display));
    bg_color.red   = color_bg.red;
    bg_color.green = color_bg.green;
    bg_color.blue  = color_bg.blue;
    XAllocColor(display, colormap, &bg_color);
    video_xwindow = XCreateSimpleWindow(display, get_xid(video_container), 0, 0,
                   1, 1,
                   0, bg_color.pixel, bg_color.pixel);

    /* connect video_container resizes to video_xwindow */
    video_container_size_handler_id = g_signal_connect(
                G_OBJECT(video_container), "size-allocate",
                G_CALLBACK(video_size_handler), this);
    gtk_widget_queue_resize_no_redraw(video_container);

    /*** TOOLBAR ***/

    toolbar = gtk_toolbar_new();
    g_object_ref(G_OBJECT(toolbar));
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
    gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_SMALL_TOOLBAR);
    GtkToolItem *toolitem;
    /* play/pause */
    toolitem = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);
    g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(toolbar_handler), this);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitem, -1);
    /* stop */
    toolitem = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_STOP);
    g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(toolbar_handler), this);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitem, -1);

    /* time slider */
    toolitem = gtk_tool_item_new();
    time_slider = gtk_hscale_new_with_range(0, 100, 10);
    gtk_scale_set_draw_value(GTK_SCALE(time_slider), false);
    gtk_range_set_increments(GTK_RANGE(time_slider), 2, 10);
    g_signal_connect(G_OBJECT(time_slider), "change-value", G_CALLBACK(time_slider_handler), this);
    gtk_container_add(GTK_CONTAINER(toolitem), time_slider);
    gtk_tool_item_set_expand(toolitem, true);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitem, -1);

    /* volume slider */
    toolitem = gtk_tool_item_new();
    vol_slider = gtk_hscale_new_with_range(0, 200, 10);
    gtk_range_set_increments(GTK_RANGE(vol_slider), 5, 20);
    gtk_scale_set_draw_value(GTK_SCALE(vol_slider), false);
    g_signal_connect(G_OBJECT(vol_slider), "change-value", G_CALLBACK(vol_slider_handler), this);
    gtk_range_set_value(GTK_RANGE(vol_slider), 100);
    gtk_widget_set_size_request(vol_slider, 100, -1);
    gtk_container_add(GTK_CONTAINER(toolitem), vol_slider);
    gtk_tool_item_set_expand(toolitem, false);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitem, -1);

    return true;
}

bool VlcPluginGtk::resize_windows()
{
    GtkRequisition req;
    req.width = npwindow.width;
    req.height = npwindow.height;
    gtk_widget_size_request(parent, &req);
    return true;
}

bool VlcPluginGtk::destroy_windows()
{
    Display *display = get_display();

    /* destroy x window */
    XDestroyWindow(display, video_xwindow);

    /* destroy GTK top-levels */
    gtk_widget_destroy(parent);
    gtk_widget_destroy(fullscreen_win);

    /* free colors */
    Colormap colormap = DefaultColormap(display, DefaultScreen(display));
    XFreeColors(display, colormap, &bg_color.pixel, 1, 0);
    return true;
}
