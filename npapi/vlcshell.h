/*****************************************************************************
 * vlcshell.h:
 *****************************************************************************
 * Copyright (C) 2009-2014 VLC authors and VideoLAN
 * $Id$
 *
 * Authors: Jean-Paul Saman <jpsaman@videolan.org>
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

#ifndef __VLCSHELL_H__
#define __VLCSHELL_H__

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif


NPP_GET_MIME_CONST char * NPP_GetMIMEDescription( void );

NPError NPP_Initialize( void );

void NPP_Shutdown( void );

NPError NPP_New( NPMIMEType pluginType, NPP instance, uint16_t mode, NPint16_t argc,
                 char* argn[], char* argv[], NPSavedData* saved );

NPError NPP_Destroy( NPP instance, NPSavedData** save );

NPError NPP_GetValue( NPP instance, NPPVariable variable, void *value );
NPError NPP_SetValue( NPP instance, NPNVariable variable, void *value );

NPError NPP_SetWindow( NPP instance, NPWindow* window );

NPError NPP_NewStream( NPP instance, NPMIMEType type, NPStream *stream,
                       NPBool seekable, NPuint16_t *stype );
NPError NPP_DestroyStream( NPP instance, NPStream *stream, NPError reason );
void NPP_StreamAsFile( NPP instance, NPStream *stream, const char* fname );

NPint32_t NPP_WriteReady( NPP instance, NPStream *stream );
NPint32_t NPP_Write( NPP instance, NPStream *stream, NPint32_t offset,
                 NPint32_t len, void *buffer );

void NPP_URLNotify( NPP instance, const char* url,
                    NPReason reason, void* notifyData );
void NPP_Print( NPP instance, NPPrint* printInfo );

#ifdef XP_MACOSX
NPint16_t NPP_HandleEvent( NPP instance, void * event );
#endif

/*******************************************************************************
 * Plugin properties.
 ******************************************************************************/
#define PLUGIN_NAME         "VLC Web Plugin"
#define PLUGIN_DESCRIPTION \
    "VLC media player Web Plugin %s<br />" \
    "Copyright © 2002-2014 VLC authors and VideoLAN<br />" \
    "<a href=\"http://www.videolan.org/vlc/\">http://www.videolan.org/vlc/</a>"

#endif
