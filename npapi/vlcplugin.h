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

#ifndef __VLCPLUGIN_H__
#define __VLCPLUGIN_H__

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "common.h"

#if defined(XP_UNIX) && !defined(XP_MACOSX)
#   if defined(USE_GTK)
#       include "vlcplugin_gtk.h"
        typedef class VlcPluginGtk VlcPlugin;
#   else
#       include "vlcplugin_xcb.h"
        typedef class VlcPluginXcb VlcPlugin;
#   endif

#   include "vlcwindowless_xcb.h"
    typedef VlcWindowlessXCB VlcWindowless;
#elif defined(XP_WIN)
#   include "vlcplugin_win.h"
    typedef class VlcPluginWin VlcPlugin;

#   include "vlcwindowless_win.h"
    typedef class VlcWindowlessWin VlcWindowless;
#elif defined(XP_MACOSX)
#   include "vlcplugin_mac.h"
    typedef class VlcPluginMac VlcPlugin;

#   include "vlcwindowless_mac.h"
    typedef class VlcWindowlessMac VlcWindowless;
#endif


#endif /* __VLCPLUGIN_H__ */
