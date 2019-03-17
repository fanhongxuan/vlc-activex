/*****************************************************************************
 * Copyright (C) 2002-2014 VideoLAN and VLC authors
 * $Id$
 *
 * Authors: Sergey Radionov <rsatom@gmail.com>
 *          Felix Paul Kühne <fkuehne # videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef _VLC_PLAYER_OPTIONS_H_
#define _VLC_PLAYER_OPTIONS_H_

#include <string>
#include <cstring>
#include <cstdio>

static inline bool HTMLColor2RGB(const char *htmlColor, unsigned *r, unsigned *g, unsigned *b)
{
    if (!htmlColor)
        return false;
    switch (strlen(htmlColor)) {
        case 4:
            if (sscanf(htmlColor, "#%1x%1x%1x", r, g, b) != 3)
                return false;
            *r *= 0x11;
            *g *= 0x11;
            *b *= 0x11;
            return true;
        case 7:
            if (sscanf(htmlColor, "#%2x%2x%2x", r, g, b) != 3)
                return false;
            return true;
        default:
            return false;
    }
}


enum vlc_player_option_e
{
    po_autoplay,
    po_show_toolbar,
    po_enable_fullscreen,
    po_bg_text,
    po_bg_color,
    po_enable_branding
};

class vlc_player_options
{
public:
    vlc_player_options()
        :_autoplay(true), _show_toolbar(true), _enable_fullscreen(true), _enable_branding(false),
        _bg_color(/*black*/"#000000")
   {}

    void set_autoplay(bool ap){
        _autoplay = ap;
        on_option_change(po_autoplay);
    }
    bool get_autoplay() const
        {return _autoplay;}

    void set_show_toolbar(bool st){
        _show_toolbar = st;
        on_option_change(po_show_toolbar);
    }
    bool get_show_toolbar() const
        {return _show_toolbar;}

    void set_enable_fs(bool ef){
        _enable_fullscreen = ef;
        on_option_change(po_enable_fullscreen);
    }
    bool get_enable_fs() const
        {return _enable_fullscreen;}

    void set_bg_text(const std::string& bt){
        _bg_text = bt;
        on_option_change(po_bg_text);
    }
    const std::string& get_bg_text() const {
        return _bg_text;
    }

    void set_bg_color(const std::string& bc){
        _bg_color = bc;
        on_option_change(po_bg_color);
    }
    const std::string& get_bg_color() const {
        return _bg_color;
    }

    void set_enable_branding(bool st){
        _enable_branding = st;
        on_option_change(po_enable_branding);
    }
    bool get_enable_branding() const
    {return _enable_branding;}

    virtual void on_option_change(vlc_player_option_e ){};

private:
    bool         _autoplay;
    bool         _show_toolbar;
    bool         _enable_fullscreen;
    bool         _enable_branding;
    std::string  _bg_text;
    //background color format is "#rrggbb"
    std::string  _bg_color;
};

#endif //_VLC_PLAYER_OPTIONS_H_
