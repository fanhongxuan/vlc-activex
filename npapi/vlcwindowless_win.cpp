/*****************************************************************************
 * vlcwindowless_win.cpp: a VLC plugin for Mozilla (Windows windowless)
 *****************************************************************************
 * Copyright (C) 2011-2013 VLC authors and VideoLAN
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

#include "vlcwindowless_win.h"
#include <cstdio>

VlcWindowlessWin::VlcWindowlessWin(NPP instance, NPuint16_t mode) :
    VlcWindowlessBase(instance, mode)
{
    unsigned r = 0, g = 0, b = 0;
    HTMLColor2RGB(get_options().get_bg_color().c_str(), &r, &g, &b);
    m_hBgBrush = CreateSolidBrush( RGB(r,g,b) );
}

VlcWindowlessWin::~VlcWindowlessWin()
{
    DeleteObject(m_hBgBrush);
}

bool VlcWindowlessWin::handle_event(void *event)
{
    NPEvent *npevent = reinterpret_cast<NPEvent *> (event);
    switch (npevent->event) {
    case WM_PAINT:
        BOOL ret;
        HDC hDC = reinterpret_cast<HDC> (npevent->wParam);
        if (!hDC) {
            fprintf(stderr, "NULL HDC given by browser\n");
            return false;
        }
        int savedID = SaveDC(hDC);

        RECT *clipRect = reinterpret_cast<RECT *> (npevent->lParam);

        ret = FillRect(hDC, clipRect, m_hBgBrush);
        if (!ret) {
            LPVOID lpMsgBuf;
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                GetLastError(),
                0, // Default language
                (LPTSTR) &lpMsgBuf,
                0,
                NULL
            );

            fprintf(stderr, "Error in FillRect: %s\n", lpMsgBuf);
            LocalFree( lpMsgBuf );
        }

        if ( m_frame_buf.size() &&
             m_frame_buf.size() >= m_media_width * m_media_height * DEF_PIXEL_BYTES)
        {
            BITMAPINFO BmpInfo; ZeroMemory(&BmpInfo, sizeof(BmpInfo));
            BITMAPINFOHEADER& BmpH = BmpInfo.bmiHeader;
            BmpH.biSize = sizeof(BITMAPINFOHEADER);
            BmpH.biWidth = m_media_width;
            BmpH.biHeight = -((int)m_media_height);
            BmpH.biPlanes = 1;
            BmpH.biBitCount = DEF_PIXEL_BYTES*8;
            BmpH.biCompression = BI_RGB;
            //following members are already zeroed
            //BmpH.biSizeImage = 0;
            //BmpH.biXPelsPerMeter = 0;
            //BmpH.biYPelsPerMeter = 0;
            //BmpH.biClrUsed = 0;
            //BmpH.biClrImportant = 0;


            ret = SetDIBitsToDevice(hDC,
                            npwindow.x + (npwindow.width - m_media_width)/2,
                            npwindow.y + (npwindow.height - m_media_height)/2,
                            m_media_width, m_media_height,
                            0, 0,
                            0, m_media_height,
                            &m_frame_buf[0],
                            &BmpInfo, DIB_RGB_COLORS);
            if (!ret) {
                LPVOID lpMsgBuf;
                FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    GetLastError(),
                    0, // Default language
                    (LPTSTR) &lpMsgBuf,
                    0,
                    NULL
                );

                fprintf(stderr, "Error in SetDIBitsToDevice: %s\n", lpMsgBuf);
                LocalFree( lpMsgBuf );
            }

        }
        RestoreDC(hDC, savedID);
        return true;
    }
    return VlcWindowlessBase::handle_event(event);
}
