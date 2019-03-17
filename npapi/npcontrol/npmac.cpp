/*****************************************************************************
 * npmac.cpp: Safari/Mozilla/Firefox plugin for VLC
 *****************************************************************************
 * Copyright (C) 2009, Jean-Paul Saman <jpsaman@videolan.org>
 * Copyright (C) 2012-2015 Felix Paul Kühne <fkuehne # videolan # org>
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

#include <string.h>
#include <stddef.h>
#include <cstring>

#include "../common.h"
#include "../vlcplugin.h"
#include "../vlcshell.h"

#include <npapi.h>
#include "npfunctions.h"
#define CALL_NPN(unused, FN, ...) ((*FN)(__VA_ARGS__))

#pragma mark -
#pragma mark Globals

NPNetscapeFuncs   *gNetscapeFuncs;    /* Netscape Function table */

#pragma mark -
#pragma mark Wrapper Functions

void NPN_PluginThreadAsyncCall(NPP instance, void (*function)(void *), void *userData)
{
    CALL_NPN(CallNPN_PluginThreadAsyncCallProc, gNetscapeFuncs->pluginthreadasynccall, instance, function, userData);
}

#pragma mark -
#pragma mark Private Functions

/***********************************************************************
 *
 * Wrapper functions : Netscape Navigator -> plugin
 *
 * These functions let the plugin developer just create the APIs
 * as documented and defined in npapi.h, without needing to 
 * install those functions in the function table or worry about
 * setting up globals for 68K plugins.
 *
 ***********************************************************************/

/* Function prototypes */
NPError MacSpecific_New(NPMIMEType pluginType, NPP instance, uint16_t mode,
        int16_t argc, char* argn[], char* argv[], NPSavedData* saved);

/* function implementations */
NPError MacSpecific_New(NPMIMEType pluginType, NPP instance, uint16_t mode,
                        int16_t argc, char* argn[], char* argv[], NPSavedData* saved)
{
    /* find out, if the plugin should run in windowless mode.
     * if yes, choose the CoreGraphics drawing model */
    bool windowless = false;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argn[i], "windowless")) {
            windowless = (!strcmp(argv[i], "1") ||
                          !strcasecmp(argv[i], "true") ||
                          !strcasecmp(argv[i], "yes"));
            break;
        }
    }

    NPError err;
    if (windowless) {
        fprintf(stderr, "running in windowless\n");
        NPBool supportsCoreGraphics = FALSE;
        err = NPN_GetValue(instance, NPNVsupportsCoreGraphicsBool, &supportsCoreGraphics);
        if (err != NPERR_NO_ERROR || !supportsCoreGraphics) {
            fprintf(stderr, "Error in New: browser doesn't support CoreGraphics drawing model\n");
            return NPERR_INCOMPATIBLE_VERSION_ERROR;
        }

        err = NPN_SetValue(instance, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreGraphics);
        if (err != NPERR_NO_ERROR) {
            fprintf(stderr, "Error in New: couldn't activate CoreGraphics drawing model\n");
            return NPERR_INCOMPATIBLE_VERSION_ERROR;
        }
    } else {
        fprintf(stderr, "running windowed\n");
        NPBool supportsCoreAnimation = FALSE;
        err = NPN_GetValue(instance, NPNVsupportsCoreAnimationBool, &supportsCoreAnimation);
        if (err != NPERR_NO_ERROR || !supportsCoreAnimation) {
            fprintf(stderr, "Error in New: browser doesn't support CoreAnimation drawing model\n");
            return NPERR_INCOMPATIBLE_VERSION_ERROR;
        }

        err = NPN_SetValue(instance, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreAnimation);
        if (err != NPERR_NO_ERROR) {
            fprintf(stderr, "Error in New: couldn't activate CoreAnimation drawing model\n");
            return NPERR_INCOMPATIBLE_VERSION_ERROR;
        }
    }

    NPBool supportsCocoaEvents = FALSE;
    err = NPN_GetValue(instance, NPNVsupportsCocoaBool, &supportsCocoaEvents);
    if (err != NPERR_NO_ERROR || !supportsCocoaEvents) {
        fprintf(stderr, "Error in New: browser doesn't support Cocoa event model\n");
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }

    err = NPN_SetValue(instance, NPPVpluginEventModel, (void*)NPEventModelCocoa);
    if (err != NPERR_NO_ERROR) {
        fprintf(stderr, "Error in New: couldn't activate Cocoa event model\n");
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }

    NPError ret = NPP_New(pluginType, instance, mode, argc, argn, argv, saved);

    return ret;
}

#pragma mark -
#pragma mark Initialization & Run

extern "C" {
    NPError NP_Initialize(NPNetscapeFuncs* nsTable);
    NPError NP_GetEntryPoints(NPPluginFuncs* pluginFuncs);
    NPError NP_Shutdown(void);
}

NPError NP_Initialize(NPNetscapeFuncs* browserFuncs)
{
    /* validate input parameters */

    if (NULL == browserFuncs) {
        fprintf(stderr, "NP_Initialize error: NPERR_INVALID_FUNCTABLE_ERROR: table is null\n");
        return NPERR_INVALID_FUNCTABLE_ERROR;
    }

    /*
     * Check the major version passed in Netscape's function table.
     * We won't load if the major version is newer than what we expect.
     * Also check that the function tables passed in are big enough for
     * all the functions we need (they could be bigger, if Netscape added
     * new APIs, but that's OK with us -- we'll just ignore them).
     *
     */

    if ((browserFuncs->version >> 8) > NP_VERSION_MAJOR) {
        fprintf(stderr, "NP_Initialize error: NPERR_INCOMPATIBLE_VERSION_ERROR\n");
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }

    /* We use all functions of the nsTable up to and including pluginthreadasynccall. We therefore check that
     * reaches at least until that function. */
    if (browserFuncs->size < (offsetof(NPNetscapeFuncs, pluginthreadasynccall) + sizeof(NPN_PluginThreadAsyncCallProcPtr))) {
        fprintf(stderr, "NP_Initialize error: NPERR_INVALID_FUNCTABLE_ERROR: table too small\n");
        return NPERR_INVALID_FUNCTABLE_ERROR;
    }

    gNetscapeFuncs = browserFuncs;

    return NPP_Initialize();
}

NPError NP_GetEntryPoints(NPPluginFuncs* pluginFuncs)
{
    if (pluginFuncs == NULL)
        return NPERR_INVALID_FUNCTABLE_ERROR;

    /*
     * Set up the plugin function table that Netscape will use to
     * call us.  Netscape needs to know about our version and size
     * and have a UniversalProcPointer for every function we
     * implement.
     */

    int minor = pluginFuncs->version & 0xFF;
    if (minor >= NPVERS_HAS_NOTIFICATION)
        pluginFuncs->urlnotify = NPP_URLNotify;

    pluginFuncs->version        = (NP_VERSION_MAJOR << 8) | NP_VERSION_MINOR;
    pluginFuncs->size           = sizeof(pluginFuncs);
    pluginFuncs->newp           = (NPP_NewProcPtr)(MacSpecific_New);
    pluginFuncs->destroy        = NPP_Destroy;
    pluginFuncs->setwindow      = NPP_SetWindow;
    pluginFuncs->newstream      = NPP_NewStream;
    pluginFuncs->destroystream  = NPP_DestroyStream;
    pluginFuncs->asfile         = NPP_StreamAsFile;
    pluginFuncs->writeready     = NPP_WriteReady;
    pluginFuncs->write          = (NPP_WriteProcPtr)NPP_Write;
    pluginFuncs->print          = NPP_Print;
    pluginFuncs->event          = NPP_HandleEvent;
    pluginFuncs->getvalue       = NPP_GetValue;
    pluginFuncs->setvalue       = NPP_SetValue;

    return NPERR_NO_ERROR;
}

NPError NP_Shutdown(void)
{
    PLUGINDEBUGSTR("NP_Shutdown");
    NPP_Shutdown();
    return NPERR_NO_ERROR;
}
