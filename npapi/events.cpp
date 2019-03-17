/*****************************************************************************
 * events.cpp: events for the VLC Plugin
 *****************************************************************************
 * Copyright (C) 2002-2012 VLC authors and VideoLAN
 * $Id$
 *
 * Authors: Samuel Hocevar <sam@zoy.org>
 *          Jean-Paul Saman <jpsaman@videolan.org>
 *          Sergey Radionov <rsatom@gmail.com>
 *          Cheng Sun <chengsun9@gmail.com>
 *          Yannick Brehon <y.brehon@qiplay.com>
 *          Jean-Baptiste Kempf <jb@videolan.org>
 *          JP Dinger <jpd@videolan.org>
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

#include "vlcplugin.h"
#include "events.h"

/*****************************************************************************
 * Event Object
 *****************************************************************************/
void handle_input_event(const libvlc_event_t* event, void *param)
{
    VlcPluginBase *plugin = (VlcPluginBase*)param;
    switch( event->type )
    {
        case libvlc_MediaPlayerMediaChanged:
        case libvlc_MediaPlayerNothingSpecial:
        case libvlc_MediaPlayerOpening:
        case libvlc_MediaPlayerPlaying:
        case libvlc_MediaPlayerPaused:
        case libvlc_MediaPlayerStopped:
        case libvlc_MediaPlayerForward:
        case libvlc_MediaPlayerBackward:
        case libvlc_MediaPlayerEndReached:
        case libvlc_MediaPlayerEncounteredError:
            plugin->event_callback(event, NULL, 0);
            break;
        default: /* ignore all other libvlc_event_type_t */
            break;
    }
}

void handle_changed_event(const libvlc_event_t* event, void *param)
{
    uint32_t   npcount = 1;
    NPVariant *npparam = (NPVariant *) NPN_MemAlloc( sizeof(NPVariant) * npcount );

    VlcPluginBase *plugin = (VlcPluginBase*)param;
    switch( event->type )
    {
        case libvlc_MediaPlayerBuffering:
            DOUBLE_TO_NPVARIANT(event->u.media_player_buffering.new_cache, npparam[0]);
            break;
        case libvlc_MediaPlayerTimeChanged:
            DOUBLE_TO_NPVARIANT(event->u.media_player_time_changed.new_time, npparam[0]);
            break;
        case libvlc_MediaPlayerPositionChanged:
            DOUBLE_TO_NPVARIANT(event->u.media_player_position_changed.new_position, npparam[0]);
            break;
        case libvlc_MediaPlayerSeekableChanged:
            BOOLEAN_TO_NPVARIANT(event->u.media_player_seekable_changed.new_seekable, npparam[0]);
            break;
        case libvlc_MediaPlayerPausableChanged:
            BOOLEAN_TO_NPVARIANT(event->u.media_player_pausable_changed.new_pausable, npparam[0]);
            break;
        case libvlc_MediaPlayerTitleChanged:
            INT32_TO_NPVARIANT(event->u.media_player_title_changed.new_title, npparam[0]);
            break;
        case libvlc_MediaPlayerLengthChanged:
            DOUBLE_TO_NPVARIANT(event->u.media_player_length_changed.new_length, npparam[0]);
            break;
        default: /* ignore all other libvlc_event_type_t */
            NPN_MemFree( npparam );
            return;
    }
    plugin->event_callback(event, npparam, npcount);
}

static vlcplugin_event_t vlcevents[] = {
    { "MediaPlayerMediaChanged", libvlc_MediaPlayerMediaChanged, handle_input_event },
    { "MediaPlayerNothingSpecial", libvlc_MediaPlayerNothingSpecial, handle_input_event },
    { "MediaPlayerOpening", libvlc_MediaPlayerOpening, handle_input_event },
    { "MediaPlayerBuffering", libvlc_MediaPlayerBuffering, handle_changed_event },
    { "MediaPlayerPlaying", libvlc_MediaPlayerPlaying, handle_input_event },
    { "MediaPlayerPaused", libvlc_MediaPlayerPaused, handle_input_event },
    { "MediaPlayerStopped", libvlc_MediaPlayerStopped, handle_input_event },
    { "MediaPlayerForward", libvlc_MediaPlayerForward, handle_input_event },
    { "MediaPlayerBackward", libvlc_MediaPlayerBackward, handle_input_event },
    { "MediaPlayerEndReached", libvlc_MediaPlayerEndReached, handle_input_event },
    { "MediaPlayerEncounteredError", libvlc_MediaPlayerEncounteredError, handle_input_event },
    { "MediaPlayerTimeChanged", libvlc_MediaPlayerTimeChanged, handle_changed_event },
    { "MediaPlayerPositionChanged", libvlc_MediaPlayerPositionChanged, handle_changed_event },
    { "MediaPlayerSeekableChanged", libvlc_MediaPlayerSeekableChanged, handle_changed_event },
    { "MediaPlayerPausableChanged", libvlc_MediaPlayerPausableChanged, handle_changed_event },
    { "MediaPlayerTitleChanged", libvlc_MediaPlayerTitleChanged, handle_changed_event },
    { "MediaPlayerLengthChanged", libvlc_MediaPlayerLengthChanged, handle_changed_event },
};

EventObj::EventObj() : _em(NULL), _already_in_deliver(false)
{
    plugin_lock_init(&lock);
}

EventObj::~EventObj()
{
    plugin_lock_destroy(&lock);
}

void EventObj::deliver(NPP browser)
{
    if(_already_in_deliver)
        return;

    plugin_lock(&lock);
    _already_in_deliver = true;

    for( ev_l::iterator iter = _elist.begin(); iter != _elist.end(); ++iter )
    {
        NPVariant *params = iter->params();
        uint32_t   count  = iter->count();

        for( lr_l::iterator j = _llist.begin(); j != _llist.end(); ++j )
        {
            if( j->event_type() == iter->event_type() )
            {
                NPVariant result;
                NPObject *listener = j->listener();
                assert( listener );

                NPN_InvokeDefault( browser, listener, params, count, &result );
                NPN_ReleaseVariantValue( &result );

                for( uint32_t n = 0; n < count; n++ )
                {
                    if( NPVARIANT_IS_STRING(params[n]) )
                    {
                        NPN_MemFree( (void*) NPVARIANT_TO_STRING(params[n]).UTF8Characters );
                    }
                    else if( NPVARIANT_IS_OBJECT(params[n]) )
                    {
                        NPN_ReleaseObject( NPVARIANT_TO_OBJECT(params[n]) );
                        NPN_MemFree( (void*)NPVARIANT_TO_OBJECT(params[n]) );
                    }
                }
                if (params) NPN_MemFree( params );
            }
        }
    }
    _elist.clear();

    _already_in_deliver = false;
    plugin_unlock(&lock);
}

void EventObj::callback(const libvlc_event_t* event,
                        NPVariant *npparams, uint32_t count)
{
    plugin_lock(&lock);
    _elist.push_back(VLCEvent(event->type, npparams, count));
    plugin_unlock(&lock);
}

vlcplugin_event_t *EventObj::find_event(const NPString &name) const
{
    for( size_t i = 0; i < ARRAY_SIZE(vlcevents); i++ )
    {
        if( strncmp(vlcevents[i].name, name.UTF8Characters, strlen(vlcevents[i].name)) == 0 )
            return &vlcevents[i];
    }
    return NULL;
}

bool EventObj::insert(const NPString &name, NPObject *listener, bool bubble)
{
    vlcplugin_event_t *event = find_event(name);
    if( !event )
        return false;

    for( lr_l::iterator iter = _llist.begin(); iter != _llist.end(); ++iter )
    {
        if( iter->listener() == listener &&
            event->libvlc_type == iter->event_type() &&
            iter->bubble() == bubble )
        {
            return false;
        }
    }

    _llist.push_back( Listener(event, listener, bubble) );
    return true;
}

bool EventObj::remove(const NPString &name, NPObject *listener, bool bubble)
{
    vlcplugin_event_t *event = find_event(name);
    if( !event )
        return false;

    for( lr_l::iterator iter = _llist.begin(); iter !=_llist.end(); iter++ )
    {
        if( iter->event_type() == event->libvlc_type &&
            iter->listener() == listener &&
            iter->bubble() == bubble )
        {
            iter = _llist.erase(iter);
            return true;
        }
    }

    return false;
}

void EventObj::hook_manager( libvlc_event_manager_t *em, void *userdata )
{
    if( !em )
        return;

    _em = em;

    /* attach all libvlc events we need */
    for( size_t i = 0; i < ARRAY_SIZE(vlcevents); i++ )
    {
        libvlc_event_attach( _em, vlcevents[i].libvlc_type,
                vlcevents[i].libvlc_callback,
                userdata );
    }
}

void EventObj::unhook_manager( void *userdata )
{
    if( !_em )
        return;

    /* detach all libvlc events */
    for( size_t i = 0; i < ARRAY_SIZE(vlcevents); i++ )
    {
        libvlc_event_detach( _em, vlcevents[i].libvlc_type,
                vlcevents[i].libvlc_callback,
                userdata );
    }
}


