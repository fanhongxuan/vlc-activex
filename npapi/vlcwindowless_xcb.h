/*****************************************************************************
 * vlcwindowless_XCB.h: a VLC plugin for Mozilla (XCB windowless)
 *****************************************************************************
 * Copyright © 2012 VideoLAN
 * $Id$
 *
 * Authors: Ludovic Fauvet <etix@videolan.org>
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

#ifndef __VLCWINDOWLESS_XCB_H__
#define __VLCWINDOWLESS_XCB_H__

#include "vlcwindowless_base.h"

#include <xcb/xcb.h>

class VlcWindowlessXCB : public VlcWindowlessBase
{
public:
    VlcWindowlessXCB(NPP instance, NPuint16_t mode);
    virtual ~VlcWindowlessXCB();

    bool handle_event(void *event);

protected:
    bool initXCB();
    void drawBackground(xcb_drawable_t drawable);

private:
    xcb_connection_t *m_conn;
    xcb_colormap_t m_colormap;
};


#endif /* __VLCWINDOWLESS_XCB_H__ */
