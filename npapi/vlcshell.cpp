/*****************************************************************************
 * vlcshell.cpp: a VLC plugin for Mozilla
 *****************************************************************************
 * Copyright (C) 2002-2013 VLC authors and VideoLAN
 * $Id$
 *
 * Authors: Samuel Hocevar <sam@zoy.org>
 *          Jean-Paul Saman <jpsaman@videolan.org>
 *          Jean-Baptiste Kempf <jb@videolan.org>
 *          Cheng Sun <chengsun9@gmail.com>
 *          Felix Paul Kühne <fkuehne # videolan.org>
 *          Damien Fouilleul <damienf@videolan.org>
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
# include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "vlcshell.h"
#include "vlcplugin.h"

static char mimetype[] =
    /* MPEG-1 and MPEG-2 */
    "audio/mpeg:mp2,mp3,mpga,mpega:MPEG audio;"
    "audio/x-mpeg:mp2,mp3,mpga,mpega:MPEG audio;"
    "video/mpeg:mpg,mpeg,mpe:MPEG video;"
    "video/x-mpeg:mpg,mpeg,mpe:MPEG video;"
    "video/mpeg-system:mpg,mpeg,mpe,vob:MPEG video;"
    "video/x-mpeg-system:mpg,mpeg,mpe,vob:MPEG video;"
    /* MPEG-4 */
    "audio/mp4:aac,mp4,mpg4:MPEG-4 audio;"
    "audio/x-m4a:m4a:MPEG-4 audio;"
    "audio/m4a:m4a:MPEG-4 audio;"
    /* MPEG-4 ASP */
    "video/mp4:mp4,mpg4:MPEG-4 video;"
    "application/mpeg4-iod:mp4,mpg4:MPEG-4 video;"
    "application/mpeg4-muxcodetable:mp4,mpg4:MPEG-4 video;"
    "video/x-m4v:m4v:MPEG-4 video;"
    /* AVI */
    "video/x-msvideo:avi:AVI video;"
    /* QuickTime */
    "video/quicktime:mov,qt:QuickTime video;"
    /* OGG */
    "application/ogg:ogg:Ogg stream;"
    "video/ogg:ogv:Ogg video;"
    "audio/ogg:oga:Ogg audio;"
    "application/x-ogg:ogg:Ogg stream;"
    /* Opus */
    "audio/ogg;codecs=opus:opus:Opus audio;"
    /* VLC */
    "application/x-vlc-plugin::VLC plug-in;"
    /* Windows Media */
    "video/x-ms-asf-plugin:asf,asx:Windows Media Video;"
    "video/x-ms-asf:asf,asx:Windows Media Video;"
    "application/x-mplayer2::Windows Media;"
    "video/x-ms-wmv:wmv:Windows Media;"
    "video/x-ms-wvx:wvx:Windows Media Video;"
    "audio/x-ms-wma:wma:Windows Media Audio;"
    /* Google VLC */
    "application/x-google-vlc-plugin::Google VLC plug-in;"
    /* WAV audio */
    "audio/wav:wav:WAV audio;"
    "audio/x-wav:wav:WAV audio;"
    /* 3GPP */
    "audio/3gpp:3gp,3gpp:3GPP audio;"
    "video/3gpp:3gp,3gpp:3GPP video;"
    /* 3GPP2 */
    "audio/3gpp2:3g2,3gpp2:3GPP2 audio;"
    "video/3gpp2:3g2,3gpp2:3GPP2 video;"
    /* DIVX */
    "video/divx:divx:DivX video;"
    /* FLV */
    "video/flv:flv:FLV video;"
    "video/x-flv:flv:FLV video;"
    /* Matroska */
    "application/x-matroska:mkv:Matroska video;"
    "video/x-matroska:mkv:Matroska video;"
    "audio/x-matroska:mka:Matroska audio;"
    /* XSPF */
    "application/xspf+xml:xspf:Playlist xspf;"
    /* M3U */
    "audio/x-mpegurl:m3u:MPEG audio;"
    /* Webm */
    "video/webm:webm:WebM video;"
    "audio/webm:webm:WebM audio;"
    /* Real Media */
    "application/vnd.rn-realmedia:rm:Real Media File;"
    "audio/x-realaudio:ra:Real Media Audio;"
    /* AMR */
    "audio/amr:amr:AMR audio;"
    /* FLAC */
    "audio/x-flac:flac:FLAC audio;"
    "audio/flac:flac:FLAC audio;"
    ;

/******************************************************************************
 * UNIX-only API calls
 *****************************************************************************/
NPP_GET_MIME_CONST char * NPP_GetMIMEDescription( void )
{
    return mimetype;
}

NPError NPP_GetValue( NPP instance, NPPVariable variable, void *value )
{
    static char psz_name[] = PLUGIN_NAME;
    static char psz_desc[1000];

    /* plugin class variables */
    switch( variable )
    {
        case NPPVpluginNameString:
            *((char **)value) = psz_name;
            return NPERR_NO_ERROR;

        case NPPVpluginDescriptionString:
            snprintf( psz_desc, sizeof(psz_desc), PLUGIN_DESCRIPTION,
                      libvlc_get_version() );
            *((char **)value) = psz_desc;
            return NPERR_NO_ERROR;

#if defined(XP_UNIX)
        case NPPVpluginNeedsXEmbed:
            *((bool *)value) = true;
            return NPERR_NO_ERROR;
#endif

        default:
            /* move on to instance variables ... */
            ;
    }

    if( instance == NULL )
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    /* plugin instance variables */

    VlcPluginBase *p_plugin = reinterpret_cast<VlcPluginBase *>(instance->pdata);
    if( NULL == p_plugin )
    {
        /* plugin has not been initialized yet ! */
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    switch( variable )
    {
        case NPPVpluginScriptableNPObject:
        {
            /* retrieve plugin root class */
            NPClass *scriptClass = p_plugin->getScriptClass();
            if( scriptClass )
            {
                /* create an instance and return it */
                *(NPObject**)value = NPN_CreateObject(instance, scriptClass);
                return NPERR_NO_ERROR;
            }
            break;
        }
#if defined(XP_MACOSX)
        case NPPVpluginCoreAnimationLayer:
        {
            if( instance )
                return p_plugin->get_root_layer(value);

            break;
        }
#endif

        default:
            ;
    }
    return NPERR_GENERIC_ERROR;
}

/*
 * there is some confusion in NPAPI headers regarding definition of this API
 * NPPVariable is wrongly defined as NPNVariable, which sounds incorrect.
 */

NPError NPP_SetValue( NPP, NPNVariable, void * )
{
    return NPERR_GENERIC_ERROR;
}

/******************************************************************************
 * General Plug-in Calls
 *****************************************************************************/
NPError NPP_Initialize( void )
{
#ifdef XP_UNIX
    NPError err = NPERR_NO_ERROR;
    bool supportsXEmbed = false;

    err = NPN_GetValue( NULL, NPNVSupportsXEmbedBool,
                        (void *)&supportsXEmbed );

    if ( err != NPERR_NO_ERROR || supportsXEmbed != true )
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
#endif

    return NPERR_NO_ERROR;
}

void NPP_Shutdown( void )
{
    ;
}

static bool boolValue(const char *value) {
    return ( !strcmp(value, "1") ||
             !strcasecmp(value, "true") ||
             !strcasecmp(value, "yes") );
}

NPError NPP_New( NPMIMEType, NPP instance,
                 NPuint16_t mode, NPint16_t argc,
                 char* argn[], char* argv[], NPSavedData* )
{
    NPError status;
    VlcPluginBase *p_plugin;

    if( instance == NULL )
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    bool windowless = false;
    /* we need to tell whether the plugin will be windowless
     * before it is instantiated */
    for( int i = 0; i < argc; i++ )
    {
        if( !strcmp( argn[i], "windowless" ) )
        {
            windowless = boolValue(argv[i]);
            break;
        }
    }

    if( windowless )
    {
        printf( "Using Windowless mode\n" );
        /* set windowless flag */
        status = NPN_SetValue( instance, NPPVpluginWindowBool,
                                       (void *)false);
        if( NPERR_NO_ERROR != status )
        {
            return status;
        }
        status = NPN_SetValue( instance, NPPVpluginTransparentBool,
                                       (void *)false);
        if( NPERR_NO_ERROR != status )
        {
            return status;
        }

        p_plugin = new VlcWindowless( instance, mode );
    }
    else
    {
        p_plugin = new VlcPlugin( instance, mode );
    }
    if( NULL == p_plugin )
    {
        return NPERR_OUT_OF_MEMORY_ERROR;
    }

    status = p_plugin->init(argc, argn, argv);
    if( NPERR_NO_ERROR == status )
    {
        instance->pdata = reinterpret_cast<void*>(p_plugin);
    }
    else
    {
        delete p_plugin;
    }
    return status;
}

NPError NPP_Destroy( NPP instance, NPSavedData** )
{
    if( NULL == instance )
        return NPERR_INVALID_INSTANCE_ERROR;

    VlcPluginBase *p_plugin = reinterpret_cast<VlcPluginBase *>(instance->pdata);
    if( NULL == p_plugin )
        return NPERR_NO_ERROR;

    instance->pdata = NULL;

    if( p_plugin->playlist_isplaying() )
        p_plugin->playlist_stop();

    p_plugin->destroy_windows();

    delete p_plugin;

    return NPERR_NO_ERROR;
}

NPError NPP_SetWindow( NPP instance, NPWindow* window )
{
    if( ! instance )
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    /* NPP_SetWindow may be called before NPP_New (Opera) */
    VlcPluginBase *p_plugin = reinterpret_cast<VlcPluginBase *>(instance->pdata);
    if( NULL == p_plugin )
    {
        /* we should probably show a splash screen here */
        return NPERR_NO_ERROR;
    }

    /*
     * PLUGIN DEVELOPERS:
     *  Before setting window to point to the
     *  new window, you may wish to compare the new window
     *  info to the previous window (if any) to note window
     *  size changes, etc.
     */

    /* retrieve current window */
    NPWindow& curr_window = p_plugin->getWindow();

    if (window/* && window->window */) {
        if (!curr_window.window) {
            /* we've just been created */
            p_plugin->setWindow(*window);
            p_plugin->create_windows();
            p_plugin->resize_windows();
            p_plugin->set_player_window();

            /* now set plugin state to that requested in parameters */
            bool show_toolbar = p_plugin->get_options().get_show_toolbar();
            p_plugin->set_toolbar_visible( show_toolbar );

            /* handle streams properly */
            if( !p_plugin->b_stream )
            {
                if( p_plugin->psz_target )
                {
                    if( p_plugin->playlist_add( p_plugin->psz_target ) != -1 )
                    {
                        if( p_plugin->get_options().get_autoplay() )
                        {
                            p_plugin->playlist_play();
                        }
                    }
                    p_plugin->b_stream = true;
                }
            }

            p_plugin->update_controls();
        } else {
            if (window->window == curr_window.window) {
                /* resize / move notification */
                p_plugin->setWindow(*window);
                p_plugin->resize_windows();
            } else {
                /* plugin parent window was changed, notify plugin about it */
                p_plugin->destroy_windows();
                p_plugin->setWindow(*window);
                p_plugin->create_windows();
                p_plugin->resize_windows();
            }
        }
    } else {
        /* NOTE: on Windows, Opera does not call NPP_SetWindow
         * on window destruction. */
        if (curr_window.window) {
            /* we've been destroyed */
            p_plugin->destroy_windows();
        }
    }
    return NPERR_NO_ERROR;
}

NPError NPP_NewStream( NPP instance, NPMIMEType, NPStream *stream,
                       NPBool, NPuint16_t *stype )
{
    if( NULL == instance  )
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    VlcPluginBase *p_plugin = reinterpret_cast<VlcPluginBase *>(instance->pdata);
    if( NULL == p_plugin )
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

   /*
   ** Firefox/Mozilla may decide to open a stream from the URL specified
   ** in the SRC parameter of the EMBED tag and pass it to us
   **
   ** since VLC will open the SRC URL as well, we're not interested in
   ** that stream. Otherwise, we'll take it and queue it up in the playlist
   */
    if( !p_plugin->psz_target || strcmp(stream->url, p_plugin->psz_target) )
    {
        /* TODO: use pipes !!!! */
        *stype = NP_ASFILEONLY;
        return NPERR_NO_ERROR;
    }
    return NPERR_GENERIC_ERROR;
}

NPint32_t NPP_WriteReady( NPP, NPStream *)
{
    /* TODO */
    return 8*1024;
}

NPint32_t NPP_Write( NPP, NPStream *, NPint32_t,
                 NPint32_t len, void * )
{
    /* TODO */
    return len;
}

NPError NPP_DestroyStream( NPP instance, NPStream *, NPError )
{
    if( instance == NULL )
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    return NPERR_NO_ERROR;
}

void NPP_StreamAsFile( NPP instance, NPStream *stream, const char* )
{
    if( instance == NULL )
    {
        return;
    }

    VlcPluginBase *p_plugin = reinterpret_cast<VlcPluginBase *>(instance->pdata);
    if( NULL == p_plugin )
    {
        return;
    }

    if( p_plugin->playlist_add( stream->url ) != -1 )
    {
        if( p_plugin->get_options().get_autoplay() )
        {
            p_plugin->playlist_play();
        }
    }
}

void NPP_URLNotify( NPP instance, const char* ,
                    NPReason, void* )
{
    /***** Insert NPP_URLNotify code here *****\
    PluginInstance* p_plugin;
    if (instance != NULL)
        p_plugin = (PluginInstance*) instance->pdata;
    \*********************************************/
}

void NPP_Print( NPP instance, NPPrint* printInfo )
{
    if( printInfo == NULL )
    {
        return;
    }

    if( instance != NULL )
    {
        /***** Insert NPP_Print code here *****\
        PluginInstance* p_plugin = (PluginInstance*) instance->pdata;
        \**************************************/

        if( printInfo->mode == NP_FULL )
        {
            /*
             * PLUGIN DEVELOPERS:
             *  If your plugin would like to take over
             *  printing completely when it is in full-screen mode,
             *  set printInfo->pluginPrinted to TRUE and print your
             *  plugin as you see fit.  If your plugin wants Netscape
             *  to handle printing in this case, set
             *  printInfo->pluginPrinted to FALSE (the default) and
             *  do nothing.  If you do want to handle printing
             *  yourself, printOne is true if the print button
             *  (as opposed to the print menu) was clicked.
             *  On the Macintosh, platformPrint is a THPrint; on
             *  Windows, platformPrint is a structure
             *  (defined in npapi.h) containing the printer name, port,
             *  etc.
             */

            /***** Insert NPP_Print code here *****\
            void* platformPrint =
                printInfo->print.fullPrint.platformPrint;
            NPBool printOne =
                printInfo->print.fullPrint.printOne;
            \**************************************/

            /* Do the default*/
            printInfo->print.fullPrint.pluginPrinted = false;
        }
        else
        {
            /* If not fullscreen, we must be embedded */
            /*
             * PLUGIN DEVELOPERS:
             *  If your plugin is embedded, or is full-screen
             *  but you returned false in pluginPrinted above, NPP_Print
             *  will be called with mode == NP_EMBED.  The NPWindow
             *  in the printInfo gives the location and dimensions of
             *  the embedded plugin on the printed page.  On the
             *  Macintosh, platformPrint is the printer port; on
             *  Windows, platformPrint is the handle to the printing
             *  device context.
             */

            /***** Insert NPP_Print code here *****\
            NPWindow* printWindow =
                &(printInfo->print.embedPrint.window);
            void* platformPrint =
                printInfo->print.embedPrint.platformPrint;
            \**************************************/
        }
    }
}

int16_t NPP_HandleEvent( NPP instance, void * event )
{
    if( instance == NULL )
    {
        return false;
    }

    VlcPluginBase *p_plugin = reinterpret_cast<VlcPluginBase *>(instance->pdata);
    if( p_plugin == NULL )
    {
        return false;
    }

    return p_plugin->handle_event(event);

}
