/*****************************************************************************
 * npunix.cpp: Unix NPAPI plugin for VLC
 *****************************************************************************
 * Copyright (C) 2009, Jean-Paul Saman <jpsaman@videolan.org>
 * Copyright (C) 2012-2013 Felix Paul Kühne <fkuehne # videolan # org>
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
#if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
#include "npupp.h" 
#define CALL_NPN(__CallNPNFunc__, ...) (__CallNPNFunc__(__VA_ARGS__))
#else
#include "npfunctions.h"
#define CALL_NPN(unused, FN, ...) ((*FN)(__VA_ARGS__))
#endif


#ifdef USE_GTK
# include <gtk/gtk.h>
#endif

/***********************************************************************
 * Globals
 ***********************************************************************/
#pragma mark -
#pragma mark Globals

NPNetscapeFuncs  *gNetscapeFuncs;    /* Netscape Function table */
static const char       *gUserAgent;        /* User agent string */
static inline int getMinorVersion() { return gNetscapeFuncs->version & 0xFF; }
/***********************************************************************
 *
 * Wrapper functions : plugin calling Netscape Navigator
 *
 * These functions let the plugin developer just call the APIs
 * as documented and defined in npapi.h, without needing to know
 * about the function table and call macros in npupp.h.
 *
 ***********************************************************************/

/* in many browsers NPN_PluginThreadAsyncCall is missing/broken */

/* GTK workaround */
#ifdef USE_GTK
struct AsyncCallWorkaroundData
{
    void (*func)(void *);
    void *data;
};

static gboolean AsyncCallWorkaroundCallback(void *userData)
{
    AsyncCallWorkaroundData *data = (AsyncCallWorkaroundData *) userData;
    data->func(data->data);
    delete data;
    return false;
}
#endif

void
NPN_PluginThreadAsyncCall(NPP plugin,
                          void (*func)(void *),
                          void *userData)
{
    bool workaround = false;

    const int minor = getMinorVersion();
    if (gUserAgent && (strstr(gUserAgent, "Opera")))
        workaround = true;

    if (!gNetscapeFuncs->pluginthreadasynccall)
        workaround = true;

    if (workaround) {
#       ifdef USE_GTK
            AsyncCallWorkaroundData *data = new AsyncCallWorkaroundData;
            data->func = func;
            data->data = userData;
            g_idle_add(AsyncCallWorkaroundCallback, (void *)data);
            return;
#       endif
    }

#   if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) >= 20)
        return (*gNetscapeFuncs->pluginthreadasynccall)(plugin, func, userData);
#   endif

    // otherwise nothing we can do...
    fprintf(stderr, "WARNING: could not call NPN_PluginThreadAsyncCall\n");
}

void NPN_PushPopupsEnabledState(NPP instance, NPBool enabled)
{
    CALL_NPN(CallNPN_PushPopupsEnabledStateProc, gNetscapeFuncs->pushpopupsenabledstate,
        instance, enabled);
}

void NPN_PopPopupsEnabledState(NPP instance)
{
    CALL_NPN(CallNPN_PopPopupsEnabledStateProc, gNetscapeFuncs->poppopupsenabledstate,
        instance);
}

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
NPError Private_New(NPMIMEType pluginType, NPP instance, uint16_t mode,
        int16_t argc, char* argn[], char* argv[], NPSavedData* saved);
NPError Private_Destroy(NPP instance, NPSavedData** save);
NPError Private_NewStream(NPP instance, NPMIMEType type, NPStream* stream,
                          NPBool seekable, uint16_t* stype);
int32_t Private_WriteReady(NPP instance, NPStream* stream);
int32_t Private_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer);
void    Private_StreamAsFile(NPP instance, NPStream* stream, const char* fname);
NPError Private_DestroyStream(NPP instance, NPStream* stream, NPError reason);
void    Private_Print(NPP instance, NPPrint* platformPrint);
void    Private_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData);
int16_t Private_HandleEvent(NPP instance, NPEvent *event);
NPError Private_GetValue(NPP instance, NPPVariable variable, void *r_value);
NPError Private_SetValue(NPP instance, NPNVariable variable, void *r_value);
NPError Private_SetWindow(NPP instance, NPWindow* window);


/* function implementations */
NPError
Private_New(NPMIMEType pluginType, NPP instance, uint16_t mode,
        int16_t argc, char* argn[], char* argv[], NPSavedData* saved)
{
    NPError ret;
    PLUGINDEBUGSTR("New");
    gUserAgent = NPN_UserAgent(instance);
    ret = NPP_New(pluginType, instance, mode, argc, argn, argv, saved);
    return ret;
}

NPError
Private_Destroy(NPP instance, NPSavedData** save)
{
    PLUGINDEBUGSTR("Destroy");
    return NPP_Destroy(instance, save);
}

NPError
Private_SetWindow(NPP instance, NPWindow* window)
{
    NPError err;
    PLUGINDEBUGSTR("SetWindow");
    err = NPP_SetWindow(instance, window);
    return err;
}

NPError
Private_GetValue(NPP instance, NPPVariable variable, void *r_value)
{
    PLUGINDEBUGSTR("GetValue");
    return NPP_GetValue(instance, variable, r_value);
}

NPError
Private_SetValue(NPP instance, NPNVariable variable, void *r_value)
{
    PLUGINDEBUGSTR("SetValue");
    return NPP_SetValue(instance, variable, r_value);
}

NPError
Private_NewStream(NPP instance, NPMIMEType type, NPStream* stream,
            NPBool seekable, uint16_t* stype)
{
    NPError err;
    PLUGINDEBUGSTR("NewStream");
    err = NPP_NewStream(instance, type, stream, seekable, stype);
    return err;
}

int32_t
Private_WriteReady(NPP instance, NPStream* stream)
{
    int32_t result;
    PLUGINDEBUGSTR("WriteReady");
    result = NPP_WriteReady(instance, stream);
    return result;
}

int32_t
Private_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len,
        void* buffer)
{
    int32_t result;
    PLUGINDEBUGSTR("Write");
    result = NPP_Write(instance, stream, offset, len, buffer);

    return result;
}

void
Private_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
    PLUGINDEBUGSTR("StreamAsFile");
    NPP_StreamAsFile(instance, stream, fname);
}

NPError
Private_DestroyStream(NPP instance, NPStream* stream, NPError reason)
{
    NPError err;
    PLUGINDEBUGSTR("DestroyStream");
    err = NPP_DestroyStream(instance, stream, reason);

    return err;
}

void
Private_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
    PLUGINDEBUGSTR("URLNotify");
    NPP_URLNotify(instance, url, reason, notifyData);
}

void
Private_Print(NPP instance, NPPrint* platformPrint)
{
    PLUGINDEBUGSTR("Print");
    NPP_Print(instance, platformPrint);
}

int16_t
Private_HandleEvent(NPP instance, NPEvent *event)
{
    int16_t result;
    PLUGINDEBUGSTR("HandleEvent");
    result = NPP_HandleEvent(instance, event);
    return result;
}

/*********************************************************************** 
 *
 * These functions are located automagically by netscape.
 *
 ***********************************************************************/

/*
 * NP_GetMIMEDescription
 *  - Netscape needs to know about this symbol
 *  - Netscape uses the return value to identify when an object instance
 *    of this plugin should be created.
 */
NPP_GET_MIME_CONST char * NP_GetMIMEDescription(void)
{
    return NPP_GetMIMEDescription();
}

/*
 * NP_GetValue [optional]
 *  - Netscape needs to know about this symbol.
 *  - Interfaces with plugin to get values for predefined variables
 *    that the navigator needs.
 */
NPError
NP_GetValue(void* future, NPPVariable variable, void *value)
{
    return NPP_GetValue((NPP)future, variable, value);
}

/*
 * NP_Initialize
 *  - Netscape needs to know about this symbol.
 *  - It calls this function after looking up its symbol before it
 *    is about to create the first ever object of this kind.
 *
 * PARAMETERS
 *    nsTable   - The netscape function table. If developers just use these
 *        wrappers, they don't need to worry about all these function
 *        tables.
 * RETURN
 *    pluginFuncs
 *      - This functions needs to fill the plugin function table
 *        pluginFuncs and return it. Netscape Navigator plugin
 *        library will use this function table to call the plugin.
 *
 */
NPError
NP_Initialize(NPNetscapeFuncs* nsTable, NPPluginFuncs* pluginFuncs)
{
    NPError err = NPERR_NO_ERROR;

    PLUGINDEBUGSTR("NP_Initialize");

    /* validate input parameters */
    if ((nsTable == NULL) || (pluginFuncs == NULL))
        err = NPERR_INVALID_FUNCTABLE_ERROR;

    /*
     * Check the major version passed in Netscape's function table.
     * We won't load if the major version is newer than what we expect.
     * Also check that the function tables passed in are big enough for
     * all the functions we need (they could be bigger, if Netscape added
     * new APIs, but that's OK with us -- we'll just ignore them).
     *
     */
    if (err == NPERR_NO_ERROR) {
        if ((nsTable->version >> 8) > NP_VERSION_MAJOR)
            err = NPERR_INCOMPATIBLE_VERSION_ERROR;
        if (nsTable->size < ((char *)&nsTable->posturlnotify - (char *)nsTable))
            err = NPERR_INVALID_FUNCTABLE_ERROR;
        if (pluginFuncs->size < (char *)&pluginFuncs->setvalue - (char *)pluginFuncs)
            err = NPERR_INVALID_FUNCTABLE_ERROR;
    }

    if (err == NPERR_NO_ERROR)
    {
        /*
         * Copy all the fields of Netscape function table into our
         * copy so we can call back into Netscape later.  Note that
         * we need to copy the fields one by one, rather than assigning
         * the whole structure, because the Netscape function table
         * could actually be bigger than what we expect.
         */
        gNetscapeFuncs = nsTable;


        int minor = nsTable->version & 0xFF;

        /*
         * Set up the plugin function table that Netscape will use to
         * call us.  Netscape needs to know about our version and size
         * and have a UniversalProcPointer for every function we
         * implement.
         */
        pluginFuncs->version    = (NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR;
        pluginFuncs->size       = sizeof(NPPluginFuncs);
#if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
        pluginFuncs->newp       = NewNPP_NewProc(Private_New);
        pluginFuncs->destroy    = NewNPP_DestroyProc(Private_Destroy);
        pluginFuncs->setwindow  = NewNPP_SetWindowProc(Private_SetWindow);
        pluginFuncs->newstream  = NewNPP_NewStreamProc(Private_NewStream);
        pluginFuncs->destroystream = NewNPP_DestroyStreamProc(Private_DestroyStream);
        pluginFuncs->asfile     = NewNPP_StreamAsFileProc(Private_StreamAsFile);
        pluginFuncs->writeready = NewNPP_WriteReadyProc(Private_WriteReady);
        pluginFuncs->write      = NewNPP_WriteProc(Private_Write);
        pluginFuncs->print      = NewNPP_PrintProc(Private_Print);
        pluginFuncs->event      = NewNPP_HandleEventProc(Private_HandleEvent);
        pluginFuncs->getvalue   = NewNPP_GetValueProc(Private_GetValue);
        pluginFuncs->setvalue   = NewNPP_SetValueProc(Private_SetValue);
#else
        pluginFuncs->newp       = (NPP_NewProcPtr)(Private_New);
        pluginFuncs->destroy    = (NPP_DestroyProcPtr)(Private_Destroy);
        pluginFuncs->setwindow  = (NPP_SetWindowProcPtr)(Private_SetWindow);
        pluginFuncs->newstream  = (NPP_NewStreamProcPtr)(Private_NewStream);
        pluginFuncs->destroystream = (NPP_DestroyStreamProcPtr)(Private_DestroyStream);
        pluginFuncs->asfile     = (NPP_StreamAsFileProcPtr)(Private_StreamAsFile);
        pluginFuncs->writeready = (NPP_WriteReadyProcPtr)(Private_WriteReady);
        pluginFuncs->write      = (NPP_WriteProcPtr)(Private_Write);
        pluginFuncs->print      = (NPP_PrintProcPtr)(Private_Print);
        pluginFuncs->event      = (NPP_HandleEventProcPtr)(Private_HandleEvent);
        pluginFuncs->getvalue   = (NPP_GetValueProcPtr)(Private_GetValue);
        pluginFuncs->setvalue   = (NPP_SetValueProcPtr)(Private_SetValue);
#endif
        if( minor >= NPVERS_HAS_NOTIFICATION )
        {
#if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
            pluginFuncs->urlnotify = NewNPP_URLNotifyProc(Private_URLNotify);
#else
            pluginFuncs->urlnotify = (NPP_URLNotifyProcPtr)(Private_URLNotify);
#endif
        }
        pluginFuncs->javaClass = NULL;

        err = NPP_Initialize();
    }

    return err;
}

/*
 * NP_Shutdown [optional]
 *  - Netscape needs to know about this symbol.
 *  - It calls this function after looking up its symbol after
 *    the last object of this kind has been destroyed.
 *
 */
NPError
NP_Shutdown(void)
{
    PLUGINDEBUGSTR("NP_Shutdown");
    NPP_Shutdown();
    return NPERR_NO_ERROR;
}
