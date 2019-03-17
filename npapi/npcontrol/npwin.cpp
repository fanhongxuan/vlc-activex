/*****************************************************************************
 * npunix.cpp: Unix NPAPI plugin for VLC
 *****************************************************************************
 * Copyright (C) 2009, Jean-Paul Saman <jpsaman@videolan.org>
 * Copyright (C) 2012-2013 Felix Paul Kühne <fkuehne # videolan # org>
 * $Id:$
 *
 * Authors: Jean-Paul Saman <jpsaman@videolan.org>
 *          Felix Paul Kühne <fkuehne # videolan # org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "../common.h"
#include "../vlcplugin.h"
#include "../vlcshell.h"

#include <npapi.h>
#if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
#include "npupp.h"
#else
#include <npfunctions.h>
#endif


//\\// DEFINE
#define NP_EXPORT

/***********************************************************************
 * Globals
 ***********************************************************************/
NPNetscapeFuncs  *gNetscapeFuncs;    /* Netscape Function table */
static inline int getMinorVersion() { return gNetscapeFuncs->version & 0xFF; }

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
//                      PLUGIN DLL entry points
//
// These are the Windows specific DLL entry points. They must be exported
//

// we need these to be global since we have to fill one of its field
// with a data (class) which requires knowlwdge of the navigator
// jump-table. This jump table is known at Initialize time (NP_Initialize)
// which is called after NP_GetEntryPoint
static NPPluginFuncs* g_pluginFuncs;

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// NP_GetEntryPoints
//
//  fills in the func table used by Navigator to call entry points in
//  plugin DLL.  Note that these entry points ensure that DS is loaded
//  by using the NP_LOADDS macro, when compiling for Win16
//
#ifdef __MINGW32__
extern "C" __declspec(dllexport) NPError WINAPI
#else
NPError WINAPI NP_EXPORT
#endif
NP_GetEntryPoints(NPPluginFuncs* pFuncs)
{
    // trap a NULL ptr
    if(pFuncs == NULL)
        return NPERR_INVALID_FUNCTABLE_ERROR;

    // if the plugin's function table is smaller than the plugin expects,
    // then they are incompatible, and should return an error

    pFuncs->version       = (NP_VERSION_MAJOR << 8) | NP_VERSION_MINOR;
    pFuncs->newp          = NPP_New;
    pFuncs->destroy       = NPP_Destroy;
    pFuncs->setwindow     = NPP_SetWindow;
    pFuncs->newstream     = NPP_NewStream;
    pFuncs->destroystream = NPP_DestroyStream;
    pFuncs->asfile        = NPP_StreamAsFile;
    pFuncs->writeready    = NPP_WriteReady;
    pFuncs->write         = NPP_Write;
    pFuncs->print         = NPP_Print;
    pFuncs->event         = NPP_HandleEvent;
    pFuncs->getvalue      = NPP_GetValue;
    pFuncs->setvalue      = NPP_SetValue;

    g_pluginFuncs         = pFuncs;

    return NPERR_NO_ERROR;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// NP_Initialize
//
//  called immediately after the plugin DLL is loaded
//
#ifdef __MINGW32__
extern "C" __declspec(dllexport) NPError WINAPI
#else
NPError WINAPI NP_EXPORT
#endif
NP_Initialize(NPNetscapeFuncs* pFuncs)
{
    // trap a NULL ptr
    if(pFuncs == NULL)
        return NPERR_INVALID_FUNCTABLE_ERROR;

    gNetscapeFuncs = pFuncs; // save it for future reference

    // if the plugin's major ver level is lower than the Navigator's,
    // then they are incompatible, and should return an error
    if(HIBYTE(pFuncs->version) > NP_VERSION_MAJOR)
        return NPERR_INCOMPATIBLE_VERSION_ERROR;

    // We have to defer these assignments until gNetscapeFuncs is set
    int minor = getMinorVersion();

    if( minor >= NPVERS_HAS_NOTIFICATION ) {
        g_pluginFuncs->urlnotify = NPP_URLNotify;
    }
    // NPP_Initialize is a standard (cross-platform) initialize function.
    return NPP_Initialize();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// NP_Shutdown
//
//  called immediately before the plugin DLL is unloaded.
//  This function should check for some ref count on the dll to see if it is
//  unloadable or it needs to stay in memory.
//
#ifdef __MINGW32__
extern "C" __declspec(dllexport) NPError WINAPI
#else
NPError WINAPI NP_EXPORT
#endif
NP_Shutdown()
{
    NPP_Shutdown();
    gNetscapeFuncs = NULL;
    return NPERR_NO_ERROR;
}

NPP_GET_MIME_CONST char * NP_GetMIMEDescription()
{
  return NPP_GetMIMEDescription();
}

//                      END - PLUGIN DLL entry points
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.

/*    NAVIGATOR Entry points    */

/* These entry points expect to be called from within the plugin.  The
   noteworthy assumption is that DS has already been set to point to the
   plugin's DLL data segment.  Don't call these functions from outside
   the plugin without ensuring DS is set to the DLLs data segment first,
   typically using the NP_LOADDS macro
 */

void NPN_PluginThreadAsyncCall(NPP plugin, void (*func)(void *), void *userData)
{
#if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) >= 20)
    (gNetscapeFuncs->pluginthreadasynccall)(plugin, func, userData);
#endif
}

