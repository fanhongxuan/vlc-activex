/*****************************************************************************
 * vlcplugin_base.h: a VLC plugin for Mozilla
 *****************************************************************************
 * Copyright (C) 2002-2012 VLC authors and VideoLAN
 * $Id$
 *
 * Authors: Samuel Hocevar <sam@zoy.org>
 *          Damien Fouilleul <damienf@videolan.org>
 *          Jean-Paul Saman <jpsaman@videolan.org>
 *          Sergey Radionov <rsatom@gmail.com>
 *          Cheng Sun <chengsun9@gmail.com>
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
#ifndef __VLCPLUGIN_BASE_H__
#define __VLCPLUGIN_BASE_H__

#include "common.h"
#include "events.h"

#include <vector>
#include <set>

#include "../common/vlc_player_options.h"
#include "../common/vlc_player.h"

#define MAX_PARAMS 32

typedef enum vlc_toolbar_clicked_e {
    clicked_Unknown = 0,
    clicked_Play,
    clicked_Pause,
    clicked_Stop,
    clicked_timeline,
    clicked_Time,
    clicked_Fullscreen,
    clicked_Mute,
    clicked_Unmute
} vlc_toolbar_clicked_t;

class VlcPluginBase: private vlc_player_options, private vlc_player
{
protected:

public:
    VlcPluginBase( NPP, NPuint16_t );
    virtual ~VlcPluginBase();

    vlc_player& get_player()
    {
        return *static_cast<vlc_player*>(this);
    }

    vlc_player_options& get_options()
        { return *static_cast<vlc_player_options*>(this); }
    const vlc_player_options& get_options() const
        { return *static_cast<const vlc_player_options*>(this); }

    NPError             init(int argc, char* const argn[], char* const argv[]);

    libvlc_media_player_t* getMD()
    {
        if( !is_open() )
        {
             libvlc_printerr("no mediaplayer");
        }
        return get_mp();
    }

    NPP                 getBrowser() { return p_browser; };
    char*               getAbsoluteURL(const char *url);

    NPWindow&           getWindow()  { return npwindow; };
    virtual void        setWindow(const NPWindow &window);

    NPClass*            getScriptClass() { return p_scriptClass; };

    NPuint16_t  i_npmode; /* either NP_EMBED or NP_FULL */

    /* plugin properties */
    int      b_stream;
    char *   psz_target;

    void playlist_play()
    {
        play();
    }
    void playlist_play_item(int idx)
    {
        play(idx);
    }
    void playlist_stop()
    {
        stop();
    }
    void playlist_next()
    {
        next();
    }
    void playlist_prev()
    {
        prev();
    }
    void playlist_pause()
    {
        pause();
    }
    void playlist_togglePause()
    {
        togglePause();
    }
    int playlist_isplaying()
    {
        return is_playing();
    }
    int playlist_currentitem()
    {
        return current_item();
    }
    int playlist_add( const char * mrl)
    {
        return add_item(mrl);
    }
    int playlist_add_extended_untrusted( const char *mrl, const char *,
                    int optc, const char **optv )
    {
        return add_item(mrl, optc, optv);
    }
    int playlist_delete_item( int idx)
    {
        return delete_item(idx);
    }
    void playlist_clear()
    {
        clear_items() ;
    }
    int  playlist_count()
    {
        return items_count();
    }
    bool playlist_select(int);

    void control_handler(vlc_toolbar_clicked_t);

    bool  player_has_vout();

    virtual bool create_windows() = 0;
    virtual bool resize_windows() = 0;
    virtual bool destroy_windows() = 0;

    virtual bool handle_event(void *event);

#if defined(XP_MACOSX)
    virtual NPError get_root_layer(void *value);
#endif

    virtual void toggle_fullscreen() = 0;
    virtual void set_fullscreen(int) = 0;
    virtual int get_fullscreen() = 0;

    virtual void set_toolbar_visible(bool) = 0;
    virtual bool get_toolbar_visible() = 0;

    virtual void update_controls() = 0;
    virtual void popup_menu() = 0;

    virtual void set_player_window() = 0;

    static bool canUseEventListener();

    EventObj events;
    void event_callback(const libvlc_event_t *, NPVariant *, uint32_t);

protected:
    // called after libvlc_media_player_new_from_media
    virtual void on_media_player_new()     {};
    // called before libvlc_media_player_release
    virtual void on_media_player_release() {};

    /* VLC reference */
    libvlc_instance_t   *libvlc_instance;
    NPClass             *p_scriptClass;

    /* browser reference */
    NPP     p_browser;
    char    *psz_baseURL;

    /* display settings */
    NPWindow  npwindow;

    static void eventAsync(void *);

private:
    static std::set<VlcPluginBase*> _instances;
};

#endif
