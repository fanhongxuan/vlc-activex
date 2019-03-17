/*****************************************************************************
 * vlcwindowless_XCB.cpp: a VLC plugin for Mozilla (XCB windowless)
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

#include "vlcwindowless_xcb.h"

#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <cstring>
#include <cstdlib>

VlcWindowlessXCB::VlcWindowlessXCB(NPP instance, NPuint16_t mode) :
    VlcWindowlessBase(instance, mode), m_conn(0), m_colormap(0)
{
}

VlcWindowlessXCB::~VlcWindowlessXCB()
{
}

bool VlcWindowlessXCB::initXCB()
{
    NPSetWindowCallbackStruct *info =
            static_cast<NPSetWindowCallbackStruct *>(npwindow.ws_info);

    if (!info) {
        /* NPP_SetWindow has not been called yet */
        return false;
    }

    m_conn = XGetXCBConnection(info->display);
    m_colormap = info->colormap;

    return true;
}

void VlcWindowlessXCB::drawBackground(xcb_drawable_t drawable)
{
    /* Obtain the background color */
    unsigned r = 0, g = 0, b = 0;
    HTMLColor2RGB(get_options().get_bg_color().c_str(), &r, &g, &b);
    xcb_alloc_color_reply_t *reply = xcb_alloc_color_reply(m_conn,
            xcb_alloc_color(m_conn, m_colormap,
                            (uint16_t) r << 8,
                            (uint16_t) g << 8,
                            (uint16_t) b << 8), NULL);
    uint32_t colorpixel = reply->pixel;
    free(reply);

    /* Prepare to fill the background */
    xcb_gcontext_t background = xcb_generate_id(m_conn);
    uint32_t        mask       = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    uint32_t        values[2]  = {colorpixel, 0};
    xcb_create_gc(m_conn, background, drawable, mask, values);
    xcb_rectangle_t rect;
    rect.x = npwindow.x;
    rect.y = npwindow.y;
    rect.width = npwindow.width;
    rect.height = npwindow.height;

    /* Fill the background */
    xcb_poly_fill_rectangle(m_conn, drawable, background, 1, &rect);
    xcb_free_gc(m_conn, background);
}

bool VlcWindowlessXCB::handle_event(void *event)
{
    XEvent *xevent = static_cast<XEvent *>(event);
    switch (xevent->type) {
    case GraphicsExpose:

        xcb_gcontext_t gc;
        xcb_void_cookie_t cookie;
        xcb_generic_error_t *err;
        XGraphicsExposeEvent *xgeevent = reinterpret_cast<XGraphicsExposeEvent *>(xevent);

        /* Initialize xcb connection if necessary */
        if (!m_conn)
            if (!initXCB()) break;

        drawBackground(xgeevent->drawable);

        /* Validate video buffer size */
        if (m_frame_buf.empty() ||
            m_frame_buf.size() < m_media_width * m_media_height * DEF_PIXEL_BYTES)
            break;

        /* Compute the position of the video */
        int left = npwindow.x + (npwindow.width  - m_media_width)  / 2;
        int top  = npwindow.y + (npwindow.height - m_media_height) / 2;

        gc = xcb_generate_id(m_conn);
        xcb_create_gc(m_conn, gc, xgeevent->drawable, 0, NULL);

        /* Push the frame in X11 */
        cookie = xcb_put_image_checked(
                    m_conn,
                    XCB_IMAGE_FORMAT_Z_PIXMAP,
                    xgeevent->drawable,
                    gc,
                    m_media_width,
                    m_media_height,
                    left, top,
                    0, 24,
                    m_media_width * m_media_height * 4,
                    (const uint8_t *)&m_frame_buf[0]);

        if (err = xcb_request_check(m_conn, cookie))
        {
            fprintf(stderr, "Unable to put picture into drawable. Error %d\n",
                            err->error_code);
            free(err);
        }

        /* Flush the the connection */
        xcb_flush(m_conn);
        xcb_free_gc(m_conn, gc);
    }
    return VlcWindowlessBase::handle_event(event);
}
