/*****************************************************************************
 * vlcplugin.h: a VLC plugin for Mozilla
 *****************************************************************************
 * Copyright (C) 2002-2012 VideoLAN
 * $Id$
 *
 * Authors: Samuel Hocevar <sam@zoy.org>
 *          Damien Fouilleul <damienf.fouilleul@laposte.net>
 *          Jean-Paul Saman <jpsaman@videolan.org>
 *          Sergey Radionov <rsatom@gmail.com>
 *          Jean-Baptiste Kempf <jb@videolan.org>
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

#ifndef __VLCPLUGIN_EVENTS_H__
#define __VLCPLUGIN_EVENTS_H__

#include <assert.h>
#include <vector>
#include "common.h"
#include "../common/vlc_player.h"

typedef struct {
    const char *name;                      /* event name */
    const libvlc_event_type_t libvlc_type; /* libvlc event type */
    libvlc_callback_t libvlc_callback;     /* libvlc callback function */
} vlcplugin_event_t;

class EventObj
{
private:

    class Listener
    {
    public:
        Listener(vlcplugin_event_t *event, NPObject *p_object, bool b_bubble):
            _event(event), _listener(p_object), _bubble(b_bubble)
        {
                assert(event);
                assert(p_object);
        }

        libvlc_event_type_t event_type() const { return _event->libvlc_type; }
        NPObject *listener() const { return _listener; }
        bool bubble() const { return _bubble; }
    private:
        vlcplugin_event_t *_event;
        NPObject *_listener;
        bool _bubble;
    };

    class VLCEvent
    {
    public:
        VLCEvent(libvlc_event_type_t libvlc_event_type, NPVariant *npparams, uint32_t npcount):
            _libvlc_event_type(libvlc_event_type), _npparams(npparams), _npcount(npcount)
         {}

        libvlc_event_type_t event_type() const { return _libvlc_event_type; }
        NPVariant *params() const { return _npparams; }
        uint32_t count() const { return _npcount; }
    private:
        libvlc_event_type_t _libvlc_event_type;
        NPVariant *_npparams;
        uint32_t _npcount;
    };

public:
    EventObj();
    virtual ~EventObj();

    void unhook_manager(void *);
    void hook_manager(libvlc_event_manager_t *, void *);

    void deliver(NPP browser);
    void callback(const libvlc_event_t *event, NPVariant *npparams, uint32_t count);
    bool insert(const NPString &name, NPObject *listener, bool bubble);
    bool remove(const NPString &name, NPObject *listener, bool bubble);

private:
    libvlc_event_manager_t *_em; /* libvlc media_player event manager */
    vlcplugin_event_t *find_event(const NPString &name) const;

    typedef std::vector<Listener> lr_l;
    typedef std::vector<VLCEvent> ev_l;
    lr_l _llist; /* list of registered listeners with 'addEventListener' method */
    ev_l _elist; /* scheduled events list for delivery to browser */

    plugin_lock_t lock;
    bool _already_in_deliver;
};

#endif
