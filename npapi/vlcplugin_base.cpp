/*****************************************************************************
 * vlcplugin_base.cpp: a VLC plugin for Mozilla
 *****************************************************************************
 * Copyright (C) 2002-2012 VLC authors and VideoLAN
 * $Id$
 *
 * Authors: Damien Fouilleul <damienf.fouilleul@laposte.net>
 *          Jean-Paul Saman <jpsaman@videolan.org>
 *          Sergey Radionov <rsatom@gmail.com>
 *          Cheng Sun <chengsun9@gmail.com>
 *          Yannick Brehon <y.brehon@qiplay.com>
 *          Felix Paul Kühne <fkuehne # videolan.org>
 *          Ludovic Fauvet <etix@videolan.org>
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

/*****************************************************************************
 * Preamble
 *****************************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "vlcplugin_base.h"
#include "vlcplugin.h"

#include "npruntime/npolibvlc.h"

#include <cctype>

#include <cstdio>
#include <cstdlib>
#include <cstring>

/*****************************************************************************
 * VlcPluginBase constructor and destructor
 *****************************************************************************/
VlcPluginBase::VlcPluginBase( NPP instance, NPuint16_t mode ) :
    i_npmode(mode),
    b_stream(0),
    psz_target(NULL),
    libvlc_instance(NULL),
    p_scriptClass(NULL),
    p_browser(instance),
    psz_baseURL(NULL)
{
    memset(&npwindow, 0, sizeof(NPWindow));
    _instances.insert(this);
}

static bool boolValue(const char *value) {
    return ( *value == '\0' ||
             !strcmp(value, "1") ||
             !strcasecmp(value, "true") ||
             !strcasecmp(value, "yes") );
}

std::set<VlcPluginBase*> VlcPluginBase::_instances;

void VlcPluginBase::eventAsync(void *param)
{
    VlcPluginBase *plugin = (VlcPluginBase*)param;
    if( _instances.find(plugin) == _instances.end() )
        return;

    plugin->events.deliver(plugin->getBrowser());
    plugin->update_controls();
}

void VlcPluginBase::event_callback(const libvlc_event_t* event,
                NPVariant *npparams, uint32_t npcount)
{
#if defined(XP_UNIX) || defined(XP_WIN) || defined (XP_MACOSX)
    events.callback(event, npparams, npcount);
    NPN_PluginThreadAsyncCall(getBrowser(), eventAsync, this);
#else
#   warning NPN_PluginThreadAsyncCall not implemented yet.
    printf("No NPN_PluginThreadAsyncCall(), doing nothing.\n");
#endif
}

NPError VlcPluginBase::init(int argc, char* const argn[], char* const argv[])
{
    /* prepare VLC command line */
    const char *ppsz_argv[MAX_PARAMS];
    int ppsz_argc = 0;

#ifndef NDEBUG
    ppsz_argv[ppsz_argc++] = "--no-plugins-cache";
#endif

    /* locate VLC module path */
#ifdef XP_WIN
    HKEY h_key;
    DWORD i_type, i_data = MAX_PATH + 1;
    char p_data[MAX_PATH + 1];
    if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, "Software\\VideoLAN\\VLC",
                      0, KEY_READ, &h_key ) == ERROR_SUCCESS )
    {
         if( RegQueryValueEx( h_key, "InstallDir", 0, &i_type,
                              (LPBYTE)p_data, &i_data ) == ERROR_SUCCESS )
         {
             if( i_type == REG_SZ )
             {
                 strcat( p_data, "\\plugins" );
                 ppsz_argv[ppsz_argc++] = "--plugin-path";
                 ppsz_argv[ppsz_argc++] = p_data;
             }
         }
         RegCloseKey( h_key );
    }
    ppsz_argv[ppsz_argc++] = "--no-one-instance";

#endif
#ifdef XP_MACOSX
    ppsz_argv[ppsz_argc++] = "--vout=caopengllayer";
    ppsz_argv[ppsz_argc++] = "--scaletempo-stride=30";
    ppsz_argv[ppsz_argc++] = "--scaletempo-overlap=0,2";
    ppsz_argv[ppsz_argc++] = "--scaletempo-search=14";
    ppsz_argv[ppsz_argc++] = "--auhal-volume=256";
    ppsz_argv[ppsz_argc++] = "--auhal-audio-device=0";
    ppsz_argv[ppsz_argc++] = "--no-volume-save";
#endif

    /* common settings */
    ppsz_argv[ppsz_argc++] = "-vv";
    ppsz_argv[ppsz_argc++] = "--no-stats";
    ppsz_argv[ppsz_argc++] = "--no-media-library";
    ppsz_argv[ppsz_argc++] = "--intf=dummy";
    ppsz_argv[ppsz_argc++] = "--no-video-title-show";
    ppsz_argv[ppsz_argc++] = "--no-xlib";

    bool b_autoloop = false;

    /* parse plugin arguments */
    for( int i = 0; (i < argc) && (ppsz_argc < MAX_PARAMS); i++ )
    {
       /* fprintf(stderr, "argn=%s, argv=%s\n", argn[i], argv[i]); */

        if( !strcmp( argn[i], "target" )
         || !strcmp( argn[i], "mrl")
         || !strcmp( argn[i], "filename")
         || !strcmp( argn[i], "src") )
        {
            psz_target = argv[i];
        }
        else if( !strcmp( argn[i], "text" ) )
        {
            set_bg_text( argv[i] );
        }
        else if( !strcmp( argn[i], "autoplay")
              || !strcmp( argn[i], "autostart") )
        {
            set_autoplay(boolValue(argv[i]));
        }
        else if( !strcmp( argn[i], "fullscreen" )
              || !strcmp( argn[i], "allowfullscreen" )
              || !strcmp( argn[i], "fullscreenenabled" ) )
        {
            set_enable_fs( boolValue(argv[i]) );
        }
        else if( !strcmp( argn[i], "mute" ) )
        {
            if( boolValue(argv[i]) )
            {
                ppsz_argv[ppsz_argc++] = "--volume=0";
            }
        }
        else if( !strcmp( argn[i], "loop")
              || !strcmp( argn[i], "autoloop") )
        {
            b_autoloop = boolValue(argv[i]);
        }
        else if( !strcmp( argn[i], "toolbar" )
              || !strcmp( argn[i], "controls") )
        {
            set_show_toolbar( boolValue(argv[i]) );
        }
        else if( !strcmp( argn[i], "bgcolor" ) )
        {
            set_bg_color( argv[i] );
        }
        else if( !strcmp( argn[i], "branding" ) )
        {
            set_enable_branding( boolValue(argv[i]) );
        }
    }

    libvlc_instance = libvlc_new(ppsz_argc, ppsz_argv);
    if( !libvlc_instance )
        return NPERR_GENERIC_ERROR;

    vlc_player::open(libvlc_instance);

    vlc_player::set_mode(b_autoloop ? libvlc_playback_mode_loop :
                                      libvlc_playback_mode_default);

    /*
    ** fetch plugin base URL, which is the URL of the page containing the plugin
    ** this URL is used for making absolute URL from relative URL that may be
    ** passed as an MRL argument
    */
    NPObject *plugin = NULL;

    if( NPERR_NO_ERROR == NPN_GetValue(p_browser, NPNVWindowNPObject, &plugin) )
    {
        /*
        ** is there a better way to get that info ?
        */
        static const char docLocHref[] = "document.location.href";
        NPString script;
        NPVariant result;

        script.UTF8Characters = docLocHref;
        script.UTF8Length = sizeof(docLocHref)-1;

        if( NPN_Evaluate(p_browser, plugin, &script, &result) )
        {
            if( NPVARIANT_IS_STRING(result) )
            {
                NPString &location = NPVARIANT_TO_STRING(result);

                psz_baseURL = (char *) malloc(location.UTF8Length+1);
                if( psz_baseURL )
                {
                    strncpy(psz_baseURL, location.UTF8Characters, location.UTF8Length);
                    psz_baseURL[location.UTF8Length] = '\0';
                }
            }
            NPN_ReleaseVariantValue(&result);
        }
        NPN_ReleaseObject(plugin);
    }

    if( psz_target )
    {
        // get absolute URL from src
        char *psz_absurl = getAbsoluteURL(psz_target);
        psz_target = psz_absurl ? psz_absurl : strdup(psz_target);
    }

    /* assign plugin script root class */
    /* new APIs */
    p_scriptClass = RuntimeNPClass<LibvlcRootNPObject>::getClass();

    libvlc_media_player_t *p_md = getMD();
    if( p_md ) {
      libvlc_event_manager_t *p_em;
      p_em = libvlc_media_player_event_manager( getMD() );
      events.hook_manager( p_em, this );
    }

    return NPERR_NO_ERROR;
}

VlcPluginBase::~VlcPluginBase()
{
    free(psz_baseURL);
    free(psz_target);

    if( vlc_player::is_open() )
    {

        if( playlist_isplaying() )
            playlist_stop();
        events.unhook_manager( this );
        vlc_player::close();
    }
    if( libvlc_instance )
        libvlc_release( libvlc_instance );

    _instances.erase(this);
}

void VlcPluginBase::setWindow(const NPWindow &window)
{
    npwindow = window;
}

#if defined(XP_MACOSX)
NPError VlcPluginBase::get_root_layer(void *value)
{
    return NPERR_GENERIC_ERROR;
}
#endif

bool VlcPluginBase::handle_event(void *event)
{
    return false;
}

/*****************************************************************************
 * VlcPluginBase playlist replacement methods
 *****************************************************************************/
bool  VlcPluginBase::player_has_vout()
{
    bool r = false;
    if( playlist_isplaying() )
        r = libvlc_media_player_has_vout(get_mp())!=0;
    return r;
}

/*****************************************************************************
 * VlcPluginBase methods
 *****************************************************************************/

char *VlcPluginBase::getAbsoluteURL(const char *url)
{
    if( NULL != url )
    {
        // check whether URL is already absolute
        const char *end=strchr(url, ':');
        if( (NULL != end) && (end != url) )
        {
            // validate protocol header
            const char *start = url;
            char c = *start;
            if( isalpha(c) )
            {
                ++start;
                while( start != end )
                {
                    c  = *start;
                    if( ! (isalnum(c)
                       || ('-' == c)
                       || ('+' == c)
                       || ('.' == c)
                       || ('/' == c)) ) /* VLC uses / to allow user to specify a demuxer */
                        // not valid protocol header, assume relative URL
                        goto relativeurl;
                    ++start;
                }
                /* we have a protocol header, therefore URL is absolute */
                return strdup(url);
            }
            // not a valid protocol header, assume relative URL
        }

relativeurl:

        if( psz_baseURL )
        {
            size_t baseLen = strlen(psz_baseURL);
            char *href = (char *) malloc(baseLen+strlen(url)+1);
            if( href )
            {
                /* prepend base URL */
                memcpy(href, psz_baseURL, baseLen+1);

                /*
                ** relative url could be empty,
                ** in which case return base URL
                */
                if( '\0' == *url )
                    return href;

                /*
                ** locate pathname part of base URL
                */

                /* skip over protocol part  */
                char *pathstart = strchr(href, ':');
                char *pathend = href+baseLen;
                if( pathstart )
                {
                    if( '/' == *(++pathstart) )
                    {
                        if( '/' == *(++pathstart) )
                        {
                            ++pathstart;
                        }
                    }
                    /* skip over host part */
                    pathstart = strchr(pathstart, '/');
                    if( ! pathstart )
                    {
                        // no path, add a / past end of url (over '\0')
                        pathstart = pathend;
                        *pathstart = '/';
                    }
                }
                else
                {
                    /* baseURL is just a UNIX path */
                    if( '/' != *href )
                    {
                        /* baseURL is not an absolute path */
                        free(href);
                        return NULL;
                    }
                    pathstart = href;
                }

                /* relative URL made of an absolute path ? */
                if( '/' == *url )
                {
                    /* replace path completely */
                    strcpy(pathstart, url);
                    return href;
                }

                /* find last path component and replace it */
                while( '/' != *pathend)
                    --pathend;

                /*
                ** if relative url path starts with one or more '../',
                ** factor them out of href so that we return a
                ** normalized URL
                */
                while( pathend != pathstart )
                {
                    const char *p = url;
                    if( '.' != *p )
                        break;
                    ++p;
                    if( '\0' == *p  )
                    {
                        /* relative url is just '.' */
                        url = p;
                        break;
                    }
                    if( '/' == *p  )
                    {
                        /* relative url starts with './' */
                        url = ++p;
                        continue;
                    }
                    if( '.' != *p )
                        break;
                    ++p;
                    if( '\0' == *p )
                    {
                        /* relative url is '..' */
                    }
                    else
                    {
                        if( '/' != *p )
                            break;
                        /* relative url starts with '../' */
                        ++p;
                    }
                    url = p;
                    do
                    {
                        --pathend;
                    }
                    while( '/' != *pathend );
                }
                /* skip over '/' separator */
                ++pathend;
                /* concatenate remaining base URL and relative URL */
                strcpy(pathend, url);
            }
            return href;
        }
    }
    return NULL;
}

void VlcPluginBase::control_handler(vlc_toolbar_clicked_t clicked)
{
    switch( clicked )
    {
        case clicked_Play:
        {
            playlist_play();
        }
        break;

        case clicked_Pause:
        {
            playlist_pause();
        }
        break;

        case clicked_Stop:
        {
            playlist_stop();
        }
        break;

        case clicked_Fullscreen:
        {
            toggle_fullscreen();
        }
        break;

        case clicked_Mute:
        case clicked_Unmute:
#if 0
        {
            if( p_md )
                libvlc_audio_toggle_mute( p_md );
        }
#endif
        break;

        case clicked_timeline:
#if 0
        {
            /* if a movie is loaded */
            if( p_md )
            {
                int64_t f_length;
                f_length = libvlc_media_player_get_length( p_md ) / 100;

                f_length = (float)f_length *
                        ( ((float)i_xPos-4.0 ) / ( ((float)i_width-8.0)/100) );

                libvlc_media_player_set_time( p_md, f_length );
            }
        }
#endif
        break;

        case clicked_Time:
        {
            /* Not implemented yet*/
        }
        break;

        default: /* button_Unknown */
            fprintf(stderr, "button Unknown!\n");
        break;
    }
}

// Verifies the version of the NPAPI.
// The eventListeners use a NPAPI function available
// since Gecko 1.9.
bool VlcPluginBase::canUseEventListener()
{
    int plugin_major, plugin_minor;
    int browser_major, browser_minor;

    NPN_Version(&plugin_major, &plugin_minor,
                &browser_major, &browser_minor);

    if (browser_minor >= 19 || browser_major > 0)
        return true;
    return false;
}

