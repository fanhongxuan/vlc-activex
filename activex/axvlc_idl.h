

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Sun Mar 17 22:42:11 2019
 */
/* Compiler settings for ..\..\..\activex\axvlc.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0595 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __axvlc_idl_h__
#define __axvlc_idl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVLCControl_FWD_DEFINED__
#define __IVLCControl_FWD_DEFINED__
typedef interface IVLCControl IVLCControl;

#endif 	/* __IVLCControl_FWD_DEFINED__ */


#ifndef __IVLCAudio_FWD_DEFINED__
#define __IVLCAudio_FWD_DEFINED__
typedef interface IVLCAudio IVLCAudio;

#endif 	/* __IVLCAudio_FWD_DEFINED__ */


#ifndef __IVLCInput_FWD_DEFINED__
#define __IVLCInput_FWD_DEFINED__
typedef interface IVLCInput IVLCInput;

#endif 	/* __IVLCInput_FWD_DEFINED__ */


#ifndef __IVLCLogo_FWD_DEFINED__
#define __IVLCLogo_FWD_DEFINED__
typedef interface IVLCLogo IVLCLogo;

#endif 	/* __IVLCLogo_FWD_DEFINED__ */


#ifndef __IVLCDeinterlace_FWD_DEFINED__
#define __IVLCDeinterlace_FWD_DEFINED__
typedef interface IVLCDeinterlace IVLCDeinterlace;

#endif 	/* __IVLCDeinterlace_FWD_DEFINED__ */


#ifndef __IVLCMarquee_FWD_DEFINED__
#define __IVLCMarquee_FWD_DEFINED__
typedef interface IVLCMarquee IVLCMarquee;

#endif 	/* __IVLCMarquee_FWD_DEFINED__ */


#ifndef __IVLCPlaylist_FWD_DEFINED__
#define __IVLCPlaylist_FWD_DEFINED__
typedef interface IVLCPlaylist IVLCPlaylist;

#endif 	/* __IVLCPlaylist_FWD_DEFINED__ */


#ifndef __IVLCSubtitle_FWD_DEFINED__
#define __IVLCSubtitle_FWD_DEFINED__
typedef interface IVLCSubtitle IVLCSubtitle;

#endif 	/* __IVLCSubtitle_FWD_DEFINED__ */


#ifndef __IVLCVideo_FWD_DEFINED__
#define __IVLCVideo_FWD_DEFINED__
typedef interface IVLCVideo IVLCVideo;

#endif 	/* __IVLCVideo_FWD_DEFINED__ */


#ifndef __IVLCControl2_FWD_DEFINED__
#define __IVLCControl2_FWD_DEFINED__
typedef interface IVLCControl2 IVLCControl2;

#endif 	/* __IVLCControl2_FWD_DEFINED__ */


#ifndef __DVLCEvents_FWD_DEFINED__
#define __DVLCEvents_FWD_DEFINED__
typedef interface DVLCEvents DVLCEvents;

#endif 	/* __DVLCEvents_FWD_DEFINED__ */


#ifndef __IVLCPlaylistItems_FWD_DEFINED__
#define __IVLCPlaylistItems_FWD_DEFINED__
typedef interface IVLCPlaylistItems IVLCPlaylistItems;

#endif 	/* __IVLCPlaylistItems_FWD_DEFINED__ */


#ifndef __IVLCMediaDescription_FWD_DEFINED__
#define __IVLCMediaDescription_FWD_DEFINED__
typedef interface IVLCMediaDescription IVLCMediaDescription;

#endif 	/* __IVLCMediaDescription_FWD_DEFINED__ */


#ifndef __VLCPlugin_FWD_DEFINED__
#define __VLCPlugin_FWD_DEFINED__

#ifdef __cplusplus
typedef class VLCPlugin VLCPlugin;
#else
typedef struct VLCPlugin VLCPlugin;
#endif /* __cplusplus */

#endif 	/* __VLCPlugin_FWD_DEFINED__ */


#ifndef __VLCPlugin2_FWD_DEFINED__
#define __VLCPlugin2_FWD_DEFINED__

#ifdef __cplusplus
typedef class VLCPlugin2 VLCPlugin2;
#else
typedef struct VLCPlugin2 VLCPlugin2;
#endif /* __cplusplus */

#endif 	/* __VLCPlugin2_FWD_DEFINED__ */


/* header files for imported files */
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __AXVLC_LIBRARY_DEFINED__
#define __AXVLC_LIBRARY_DEFINED__

/* library AXVLC */
/* [helpstring][version][uuid] */ 












typedef /* [public] */ 
enum VLCPlaylistMode
    {
        VLCPlayListInsert	= 1,
        VLCPlayListInsertAndGo	= 9,
        VLCPlayListReplace	= 2,
        VLCPlayListReplaceAndGo	= 10,
        VLCPlayListAppend	= 4,
        VLCPlayListAppendAndGo	= 12,
        VLCPlayListCheckInsert	= 16
    } 	eVLCPlaylistMode;

#define	VLCPlayListEnd	( -666 )

#define	DISPID_BackColor	( -501 )

#define	DISPID_Visible	( 100 )

#define	DISPID_Playing	( 101 )

#define	DISPID_Position	( 102 )

#define	DISPID_Time	( 103 )

#define	DISPID_Length	( 104 )

#define	DISPID_Volume	( 105 )

#define	DISPID_MRL	( 106 )

#define	DISPID_AutoPlay	( 107 )

#define	DISPID_AutoLoop	( 108 )

#define	DISPID_StartTime	( 109 )

#define	DISPID_BaseURL	( 110 )

#define	DISPID_Toolbar	( 111 )

#define	DISPID_FSEnabled	( 112 )

#define	DISPID_PlayEvent	( 100 )

#define	DISPID_PauseEvent	( 101 )

#define	DISPID_StopEvent	( 102 )

#define	DISPID_MediaPlayerNothingSpecialEvent	( 200 )

#define	DISPID_MediaPlayerOpeningEvent	( 201 )

#define	DISPID_MediaPlayerBufferingEvent	( 202 )

#define	DISPID_MediaPlayerPlayingEvent	( 203 )

#define	DISPID_MediaPlayerPausedEvent	( 204 )

#define	DISPID_MediaPlayerForwardEvent	( 205 )

#define	DISPID_MediaPlayerBackwardEvent	( 206 )

#define	DISPID_MediaPlayerEncounteredErrorEvent	( 207 )

#define	DISPID_MediaPlayerEndReachedEvent	( 208 )

#define	DISPID_MediaPlayerStoppedEvent	( 209 )

#define	DISPID_MediaPlayerTimeChangedEvent	( 210 )

#define	DISPID_MediaPlayerPositionChangedEvent	( 211 )

#define	DISPID_MediaPlayerSeekableChangedEvent	( 212 )

#define	DISPID_MediaPlayerPausableChangedEvent	( 213 )

#define	DISPID_MediaPlayerMediaChangedEvent	( 214 )

#define	DISPID_MediaPlayerTitleChangedEvent	( 215 )

#define	DISPID_MediaPlayerLengthChangedEvent	( 216 )


EXTERN_C const IID LIBID_AXVLC;

#ifndef __IVLCControl_INTERFACE_DEFINED__
#define __IVLCControl_INTERFACE_DEFINED__

/* interface IVLCControl */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IVLCControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C2FA41D0-B113-476e-AC8C-9BD14999C1C1")
    IVLCControl : public IDispatch
    {
    public:
        virtual /* [helpstring][bindable][propget][id] */ HRESULT STDMETHODCALLTYPE get_Visible( 
            /* [retval][out] */ VARIANT_BOOL *visible) = 0;
        
        virtual /* [helpstring][bindable][propput][id] */ HRESULT STDMETHODCALLTYPE put_Visible( 
            /* [in] */ VARIANT_BOOL visible) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE play( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE pause( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE stop( void) = 0;
        
        virtual /* [helpstring][propget][hidden][id] */ HRESULT STDMETHODCALLTYPE get_Playing( 
            /* [retval][out] */ VARIANT_BOOL *isPlaying) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Position( 
            /* [retval][out] */ float *position) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Position( 
            /* [in] */ float position) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Time( 
            /* [retval][out] */ int *seconds) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Time( 
            /* [in] */ int seconds) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE shuttle( 
            /* [in] */ int seconds) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE fullscreen( void) = 0;
        
        virtual /* [helpstring][hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_Length( 
            /* [retval][out] */ int *seconds) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE playFaster( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE playSlower( void) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Volume( 
            /* [retval][out] */ int *volume) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Volume( 
            /* [in] */ int volume) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE toggleMute( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setVariable( 
            /* [in] */ BSTR name,
            /* [in] */ VARIANT value) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE getVariable( 
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT *value) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE addTarget( 
            /* [in] */ BSTR uri,
            /* [in] */ VARIANT options,
            /* [in] */ enum VLCPlaylistMode mode,
            /* [in] */ int position) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PlaylistIndex( 
            /* [retval][out] */ int *index) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PlaylistCount( 
            /* [retval][out] */ int *index) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE playlistNext( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE playlistPrev( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE playlistClear( void) = 0;
        
        virtual /* [helpstring][hidden][propget] */ HRESULT STDMETHODCALLTYPE get_VersionInfo( 
            /* [retval][out] */ BSTR *version) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MRL( 
            /* [retval][out] */ BSTR *mrl) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_MRL( 
            /* [in] */ BSTR mrl) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AutoPlay( 
            /* [retval][out] */ VARIANT_BOOL *autoplay) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_AutoPlay( 
            /* [in] */ VARIANT_BOOL autoplay) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AutoLoop( 
            /* [retval][out] */ VARIANT_BOOL *autoloop) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_AutoLoop( 
            /* [in] */ VARIANT_BOOL autoloop) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVLCControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVLCControl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVLCControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVLCControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVLCControl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVLCControl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVLCControl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVLCControl * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][bindable][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Visible )( 
            IVLCControl * This,
            /* [retval][out] */ VARIANT_BOOL *visible);
        
        /* [helpstring][bindable][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Visible )( 
            IVLCControl * This,
            /* [in] */ VARIANT_BOOL visible);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *play )( 
            IVLCControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *pause )( 
            IVLCControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *stop )( 
            IVLCControl * This);
        
        /* [helpstring][propget][hidden][id] */ HRESULT ( STDMETHODCALLTYPE *get_Playing )( 
            IVLCControl * This,
            /* [retval][out] */ VARIANT_BOOL *isPlaying);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Position )( 
            IVLCControl * This,
            /* [retval][out] */ float *position);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Position )( 
            IVLCControl * This,
            /* [in] */ float position);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Time )( 
            IVLCControl * This,
            /* [retval][out] */ int *seconds);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Time )( 
            IVLCControl * This,
            /* [in] */ int seconds);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *shuttle )( 
            IVLCControl * This,
            /* [in] */ int seconds);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *fullscreen )( 
            IVLCControl * This);
        
        /* [helpstring][hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Length )( 
            IVLCControl * This,
            /* [retval][out] */ int *seconds);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *playFaster )( 
            IVLCControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *playSlower )( 
            IVLCControl * This);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Volume )( 
            IVLCControl * This,
            /* [retval][out] */ int *volume);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Volume )( 
            IVLCControl * This,
            /* [in] */ int volume);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *toggleMute )( 
            IVLCControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *setVariable )( 
            IVLCControl * This,
            /* [in] */ BSTR name,
            /* [in] */ VARIANT value);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *getVariable )( 
            IVLCControl * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *addTarget )( 
            IVLCControl * This,
            /* [in] */ BSTR uri,
            /* [in] */ VARIANT options,
            /* [in] */ enum VLCPlaylistMode mode,
            /* [in] */ int position);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PlaylistIndex )( 
            IVLCControl * This,
            /* [retval][out] */ int *index);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PlaylistCount )( 
            IVLCControl * This,
            /* [retval][out] */ int *index);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *playlistNext )( 
            IVLCControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *playlistPrev )( 
            IVLCControl * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *playlistClear )( 
            IVLCControl * This);
        
        /* [helpstring][hidden][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VersionInfo )( 
            IVLCControl * This,
            /* [retval][out] */ BSTR *version);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_MRL )( 
            IVLCControl * This,
            /* [retval][out] */ BSTR *mrl);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_MRL )( 
            IVLCControl * This,
            /* [in] */ BSTR mrl);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AutoPlay )( 
            IVLCControl * This,
            /* [retval][out] */ VARIANT_BOOL *autoplay);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AutoPlay )( 
            IVLCControl * This,
            /* [in] */ VARIANT_BOOL autoplay);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AutoLoop )( 
            IVLCControl * This,
            /* [retval][out] */ VARIANT_BOOL *autoloop);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AutoLoop )( 
            IVLCControl * This,
            /* [in] */ VARIANT_BOOL autoloop);
        
        END_INTERFACE
    } IVLCControlVtbl;

    interface IVLCControl
    {
        CONST_VTBL struct IVLCControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVLCControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVLCControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVLCControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVLCControl_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVLCControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVLCControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVLCControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVLCControl_get_Visible(This,visible)	\
    ( (This)->lpVtbl -> get_Visible(This,visible) ) 

#define IVLCControl_put_Visible(This,visible)	\
    ( (This)->lpVtbl -> put_Visible(This,visible) ) 

#define IVLCControl_play(This)	\
    ( (This)->lpVtbl -> play(This) ) 

#define IVLCControl_pause(This)	\
    ( (This)->lpVtbl -> pause(This) ) 

#define IVLCControl_stop(This)	\
    ( (This)->lpVtbl -> stop(This) ) 

#define IVLCControl_get_Playing(This,isPlaying)	\
    ( (This)->lpVtbl -> get_Playing(This,isPlaying) ) 

#define IVLCControl_get_Position(This,position)	\
    ( (This)->lpVtbl -> get_Position(This,position) ) 

#define IVLCControl_put_Position(This,position)	\
    ( (This)->lpVtbl -> put_Position(This,position) ) 

#define IVLCControl_get_Time(This,seconds)	\
    ( (This)->lpVtbl -> get_Time(This,seconds) ) 

#define IVLCControl_put_Time(This,seconds)	\
    ( (This)->lpVtbl -> put_Time(This,seconds) ) 

#define IVLCControl_shuttle(This,seconds)	\
    ( (This)->lpVtbl -> shuttle(This,seconds) ) 

#define IVLCControl_fullscreen(This)	\
    ( (This)->lpVtbl -> fullscreen(This) ) 

#define IVLCControl_get_Length(This,seconds)	\
    ( (This)->lpVtbl -> get_Length(This,seconds) ) 

#define IVLCControl_playFaster(This)	\
    ( (This)->lpVtbl -> playFaster(This) ) 

#define IVLCControl_playSlower(This)	\
    ( (This)->lpVtbl -> playSlower(This) ) 

#define IVLCControl_get_Volume(This,volume)	\
    ( (This)->lpVtbl -> get_Volume(This,volume) ) 

#define IVLCControl_put_Volume(This,volume)	\
    ( (This)->lpVtbl -> put_Volume(This,volume) ) 

#define IVLCControl_toggleMute(This)	\
    ( (This)->lpVtbl -> toggleMute(This) ) 

#define IVLCControl_setVariable(This,name,value)	\
    ( (This)->lpVtbl -> setVariable(This,name,value) ) 

#define IVLCControl_getVariable(This,name,value)	\
    ( (This)->lpVtbl -> getVariable(This,name,value) ) 

#define IVLCControl_addTarget(This,uri,options,mode,position)	\
    ( (This)->lpVtbl -> addTarget(This,uri,options,mode,position) ) 

#define IVLCControl_get_PlaylistIndex(This,index)	\
    ( (This)->lpVtbl -> get_PlaylistIndex(This,index) ) 

#define IVLCControl_get_PlaylistCount(This,index)	\
    ( (This)->lpVtbl -> get_PlaylistCount(This,index) ) 

#define IVLCControl_playlistNext(This)	\
    ( (This)->lpVtbl -> playlistNext(This) ) 

#define IVLCControl_playlistPrev(This)	\
    ( (This)->lpVtbl -> playlistPrev(This) ) 

#define IVLCControl_playlistClear(This)	\
    ( (This)->lpVtbl -> playlistClear(This) ) 

#define IVLCControl_get_VersionInfo(This,version)	\
    ( (This)->lpVtbl -> get_VersionInfo(This,version) ) 

#define IVLCControl_get_MRL(This,mrl)	\
    ( (This)->lpVtbl -> get_MRL(This,mrl) ) 

#define IVLCControl_put_MRL(This,mrl)	\
    ( (This)->lpVtbl -> put_MRL(This,mrl) ) 

#define IVLCControl_get_AutoPlay(This,autoplay)	\
    ( (This)->lpVtbl -> get_AutoPlay(This,autoplay) ) 

#define IVLCControl_put_AutoPlay(This,autoplay)	\
    ( (This)->lpVtbl -> put_AutoPlay(This,autoplay) ) 

#define IVLCControl_get_AutoLoop(This,autoloop)	\
    ( (This)->lpVtbl -> get_AutoLoop(This,autoloop) ) 

#define IVLCControl_put_AutoLoop(This,autoloop)	\
    ( (This)->lpVtbl -> put_AutoLoop(This,autoloop) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVLCControl_INTERFACE_DEFINED__ */


#ifndef __IVLCAudio_INTERFACE_DEFINED__
#define __IVLCAudio_INTERFACE_DEFINED__

/* interface IVLCAudio */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IVLCAudio;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9E0BD17B-2D3C-4656-B94D-03084F3FD9D4")
    IVLCAudio : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_mute( 
            /* [retval][out] */ VARIANT_BOOL *muted) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_mute( 
            /* [in] */ VARIANT_BOOL muted) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_volume( 
            /* [retval][out] */ long *volume) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_volume( 
            /* [in] */ long volume) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE toggleMute( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_track( 
            /* [retval][out] */ long *track) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_track( 
            /* [in] */ long track) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_count( 
            /* [retval][out] */ long *trackNumber) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE description( 
            /* [in] */ long trackID,
            /* [retval][out] */ BSTR *name) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_channel( 
            /* [retval][out] */ long *channel) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_channel( 
            /* [in] */ long channel) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVLCAudioVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVLCAudio * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVLCAudio * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVLCAudio * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVLCAudio * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVLCAudio * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVLCAudio * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVLCAudio * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_mute )( 
            IVLCAudio * This,
            /* [retval][out] */ VARIANT_BOOL *muted);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_mute )( 
            IVLCAudio * This,
            /* [in] */ VARIANT_BOOL muted);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_volume )( 
            IVLCAudio * This,
            /* [retval][out] */ long *volume);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_volume )( 
            IVLCAudio * This,
            /* [in] */ long volume);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *toggleMute )( 
            IVLCAudio * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_track )( 
            IVLCAudio * This,
            /* [retval][out] */ long *track);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_track )( 
            IVLCAudio * This,
            /* [in] */ long track);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_count )( 
            IVLCAudio * This,
            /* [retval][out] */ long *trackNumber);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *description )( 
            IVLCAudio * This,
            /* [in] */ long trackID,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_channel )( 
            IVLCAudio * This,
            /* [retval][out] */ long *channel);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_channel )( 
            IVLCAudio * This,
            /* [in] */ long channel);
        
        END_INTERFACE
    } IVLCAudioVtbl;

    interface IVLCAudio
    {
        CONST_VTBL struct IVLCAudioVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVLCAudio_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVLCAudio_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVLCAudio_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVLCAudio_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVLCAudio_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVLCAudio_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVLCAudio_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVLCAudio_get_mute(This,muted)	\
    ( (This)->lpVtbl -> get_mute(This,muted) ) 

#define IVLCAudio_put_mute(This,muted)	\
    ( (This)->lpVtbl -> put_mute(This,muted) ) 

#define IVLCAudio_get_volume(This,volume)	\
    ( (This)->lpVtbl -> get_volume(This,volume) ) 

#define IVLCAudio_put_volume(This,volume)	\
    ( (This)->lpVtbl -> put_volume(This,volume) ) 

#define IVLCAudio_toggleMute(This)	\
    ( (This)->lpVtbl -> toggleMute(This) ) 

#define IVLCAudio_get_track(This,track)	\
    ( (This)->lpVtbl -> get_track(This,track) ) 

#define IVLCAudio_put_track(This,track)	\
    ( (This)->lpVtbl -> put_track(This,track) ) 

#define IVLCAudio_get_count(This,trackNumber)	\
    ( (This)->lpVtbl -> get_count(This,trackNumber) ) 

#define IVLCAudio_description(This,trackID,name)	\
    ( (This)->lpVtbl -> description(This,trackID,name) ) 

#define IVLCAudio_get_channel(This,channel)	\
    ( (This)->lpVtbl -> get_channel(This,channel) ) 

#define IVLCAudio_put_channel(This,channel)	\
    ( (This)->lpVtbl -> put_channel(This,channel) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVLCAudio_INTERFACE_DEFINED__ */


#ifndef __IVLCInput_INTERFACE_DEFINED__
#define __IVLCInput_INTERFACE_DEFINED__

/* interface IVLCInput */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IVLCInput;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("49E0DBD1-9440-466C-9C97-95C67190C603")
    IVLCInput : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_length( 
            /* [retval][out] */ double *length) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_position( 
            /* [retval][out] */ double *position) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_position( 
            /* [in] */ double position) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_time( 
            /* [retval][out] */ double *time) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_time( 
            /* [in] */ double time) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_state( 
            /* [retval][out] */ long *state) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_rate( 
            /* [retval][out] */ double *rate) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_rate( 
            /* [in] */ double rate) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_fps( 
            /* [retval][out] */ double *fps) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_hasVout( 
            /* [retval][out] */ VARIANT_BOOL *hasVout) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVLCInputVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVLCInput * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVLCInput * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVLCInput * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVLCInput * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVLCInput * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVLCInput * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVLCInput * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IVLCInput * This,
            /* [retval][out] */ double *length);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_position )( 
            IVLCInput * This,
            /* [retval][out] */ double *position);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_position )( 
            IVLCInput * This,
            /* [in] */ double position);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_time )( 
            IVLCInput * This,
            /* [retval][out] */ double *time);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_time )( 
            IVLCInput * This,
            /* [in] */ double time);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_state )( 
            IVLCInput * This,
            /* [retval][out] */ long *state);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_rate )( 
            IVLCInput * This,
            /* [retval][out] */ double *rate);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_rate )( 
            IVLCInput * This,
            /* [in] */ double rate);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_fps )( 
            IVLCInput * This,
            /* [retval][out] */ double *fps);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_hasVout )( 
            IVLCInput * This,
            /* [retval][out] */ VARIANT_BOOL *hasVout);
        
        END_INTERFACE
    } IVLCInputVtbl;

    interface IVLCInput
    {
        CONST_VTBL struct IVLCInputVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVLCInput_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVLCInput_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVLCInput_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVLCInput_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVLCInput_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVLCInput_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVLCInput_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVLCInput_get_length(This,length)	\
    ( (This)->lpVtbl -> get_length(This,length) ) 

#define IVLCInput_get_position(This,position)	\
    ( (This)->lpVtbl -> get_position(This,position) ) 

#define IVLCInput_put_position(This,position)	\
    ( (This)->lpVtbl -> put_position(This,position) ) 

#define IVLCInput_get_time(This,time)	\
    ( (This)->lpVtbl -> get_time(This,time) ) 

#define IVLCInput_put_time(This,time)	\
    ( (This)->lpVtbl -> put_time(This,time) ) 

#define IVLCInput_get_state(This,state)	\
    ( (This)->lpVtbl -> get_state(This,state) ) 

#define IVLCInput_get_rate(This,rate)	\
    ( (This)->lpVtbl -> get_rate(This,rate) ) 

#define IVLCInput_put_rate(This,rate)	\
    ( (This)->lpVtbl -> put_rate(This,rate) ) 

#define IVLCInput_get_fps(This,fps)	\
    ( (This)->lpVtbl -> get_fps(This,fps) ) 

#define IVLCInput_get_hasVout(This,hasVout)	\
    ( (This)->lpVtbl -> get_hasVout(This,hasVout) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVLCInput_INTERFACE_DEFINED__ */


#ifndef __IVLCLogo_INTERFACE_DEFINED__
#define __IVLCLogo_INTERFACE_DEFINED__

/* interface IVLCLogo */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IVLCLogo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8a4a20c2-93f3-44e8-8644-beb2e3487e84")
    IVLCLogo : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE enable( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE disable( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE file( 
            /* [in] */ BSTR fname) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_delay( 
            /* [retval][out] */ long *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_delay( 
            /* [in] */ long val) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_repeat( 
            /* [retval][out] */ long *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_repeat( 
            /* [in] */ long val) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_opacity( 
            /* [retval][out] */ long *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_opacity( 
            /* [in] */ long val) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_position( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_position( 
            /* [in] */ BSTR val) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_x( 
            /* [retval][out] */ long *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_x( 
            /* [in] */ long val) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_y( 
            /* [retval][out] */ long *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_y( 
            /* [in] */ long val) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVLCLogoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVLCLogo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVLCLogo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVLCLogo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVLCLogo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVLCLogo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVLCLogo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVLCLogo * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *enable )( 
            IVLCLogo * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *disable )( 
            IVLCLogo * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *file )( 
            IVLCLogo * This,
            /* [in] */ BSTR fname);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_delay )( 
            IVLCLogo * This,
            /* [retval][out] */ long *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_delay )( 
            IVLCLogo * This,
            /* [in] */ long val);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_repeat )( 
            IVLCLogo * This,
            /* [retval][out] */ long *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_repeat )( 
            IVLCLogo * This,
            /* [in] */ long val);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_opacity )( 
            IVLCLogo * This,
            /* [retval][out] */ long *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_opacity )( 
            IVLCLogo * This,
            /* [in] */ long val);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_position )( 
            IVLCLogo * This,
            /* [retval][out] */ BSTR *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_position )( 
            IVLCLogo * This,
            /* [in] */ BSTR val);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_x )( 
            IVLCLogo * This,
            /* [retval][out] */ long *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_x )( 
            IVLCLogo * This,
            /* [in] */ long val);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_y )( 
            IVLCLogo * This,
            /* [retval][out] */ long *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_y )( 
            IVLCLogo * This,
            /* [in] */ long val);
        
        END_INTERFACE
    } IVLCLogoVtbl;

    interface IVLCLogo
    {
        CONST_VTBL struct IVLCLogoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVLCLogo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVLCLogo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVLCLogo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVLCLogo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVLCLogo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVLCLogo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVLCLogo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVLCLogo_enable(This)	\
    ( (This)->lpVtbl -> enable(This) ) 

#define IVLCLogo_disable(This)	\
    ( (This)->lpVtbl -> disable(This) ) 

#define IVLCLogo_file(This,fname)	\
    ( (This)->lpVtbl -> file(This,fname) ) 

#define IVLCLogo_get_delay(This,val)	\
    ( (This)->lpVtbl -> get_delay(This,val) ) 

#define IVLCLogo_put_delay(This,val)	\
    ( (This)->lpVtbl -> put_delay(This,val) ) 

#define IVLCLogo_get_repeat(This,val)	\
    ( (This)->lpVtbl -> get_repeat(This,val) ) 

#define IVLCLogo_put_repeat(This,val)	\
    ( (This)->lpVtbl -> put_repeat(This,val) ) 

#define IVLCLogo_get_opacity(This,val)	\
    ( (This)->lpVtbl -> get_opacity(This,val) ) 

#define IVLCLogo_put_opacity(This,val)	\
    ( (This)->lpVtbl -> put_opacity(This,val) ) 

#define IVLCLogo_get_position(This,val)	\
    ( (This)->lpVtbl -> get_position(This,val) ) 

#define IVLCLogo_put_position(This,val)	\
    ( (This)->lpVtbl -> put_position(This,val) ) 

#define IVLCLogo_get_x(This,val)	\
    ( (This)->lpVtbl -> get_x(This,val) ) 

#define IVLCLogo_put_x(This,val)	\
    ( (This)->lpVtbl -> put_x(This,val) ) 

#define IVLCLogo_get_y(This,val)	\
    ( (This)->lpVtbl -> get_y(This,val) ) 

#define IVLCLogo_put_y(This,val)	\
    ( (This)->lpVtbl -> put_y(This,val) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVLCLogo_INTERFACE_DEFINED__ */


#ifndef __IVLCDeinterlace_INTERFACE_DEFINED__
#define __IVLCDeinterlace_INTERFACE_DEFINED__

/* interface IVLCDeinterlace */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IVLCDeinterlace;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("bc97469f-cb11-4037-8dce-5fc9f5f85307")
    IVLCDeinterlace : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE enable( 
            /* [in] */ BSTR mode) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE disable( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVLCDeinterlaceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVLCDeinterlace * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVLCDeinterlace * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVLCDeinterlace * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVLCDeinterlace * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVLCDeinterlace * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVLCDeinterlace * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVLCDeinterlace * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *enable )( 
            IVLCDeinterlace * This,
            /* [in] */ BSTR mode);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *disable )( 
            IVLCDeinterlace * This);
        
        END_INTERFACE
    } IVLCDeinterlaceVtbl;

    interface IVLCDeinterlace
    {
        CONST_VTBL struct IVLCDeinterlaceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVLCDeinterlace_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVLCDeinterlace_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVLCDeinterlace_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVLCDeinterlace_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVLCDeinterlace_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVLCDeinterlace_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVLCDeinterlace_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVLCDeinterlace_enable(This,mode)	\
    ( (This)->lpVtbl -> enable(This,mode) ) 

#define IVLCDeinterlace_disable(This)	\
    ( (This)->lpVtbl -> disable(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVLCDeinterlace_INTERFACE_DEFINED__ */


#ifndef __IVLCMarquee_INTERFACE_DEFINED__
#define __IVLCMarquee_INTERFACE_DEFINED__

/* interface IVLCMarquee */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IVLCMarquee;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8D076AD6-9B6F-4150-A0FD-5D7E8C8CB02C")
    IVLCMarquee : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE enable( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE disable( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_text( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_text( 
            /* [in] */ BSTR val) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_color( 
            /* [retval][out] */ LONG *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_color( 
            /* [in] */ LONG val) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_opacity( 
            /* [retval][out] */ LONG *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_opacity( 
            /* [in] */ LONG val) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_position( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_position( 
            /* [in] */ BSTR val) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_refresh( 
            /* [retval][out] */ LONG *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_refresh( 
            /* [in] */ LONG val) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_size( 
            /* [retval][out] */ LONG *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_size( 
            /* [in] */ LONG val) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_timeout( 
            /* [retval][out] */ LONG *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_timeout( 
            /* [in] */ LONG val) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_x( 
            /* [retval][out] */ LONG *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_x( 
            /* [in] */ LONG val) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_y( 
            /* [retval][out] */ LONG *val) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_y( 
            /* [in] */ LONG val) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVLCMarqueeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVLCMarquee * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVLCMarquee * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVLCMarquee * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVLCMarquee * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVLCMarquee * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVLCMarquee * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVLCMarquee * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *enable )( 
            IVLCMarquee * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *disable )( 
            IVLCMarquee * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IVLCMarquee * This,
            /* [retval][out] */ BSTR *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IVLCMarquee * This,
            /* [in] */ BSTR val);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_color )( 
            IVLCMarquee * This,
            /* [retval][out] */ LONG *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_color )( 
            IVLCMarquee * This,
            /* [in] */ LONG val);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_opacity )( 
            IVLCMarquee * This,
            /* [retval][out] */ LONG *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_opacity )( 
            IVLCMarquee * This,
            /* [in] */ LONG val);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_position )( 
            IVLCMarquee * This,
            /* [retval][out] */ BSTR *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_position )( 
            IVLCMarquee * This,
            /* [in] */ BSTR val);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_refresh )( 
            IVLCMarquee * This,
            /* [retval][out] */ LONG *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_refresh )( 
            IVLCMarquee * This,
            /* [in] */ LONG val);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_size )( 
            IVLCMarquee * This,
            /* [retval][out] */ LONG *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_size )( 
            IVLCMarquee * This,
            /* [in] */ LONG val);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_timeout )( 
            IVLCMarquee * This,
            /* [retval][out] */ LONG *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_timeout )( 
            IVLCMarquee * This,
            /* [in] */ LONG val);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_x )( 
            IVLCMarquee * This,
            /* [retval][out] */ LONG *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_x )( 
            IVLCMarquee * This,
            /* [in] */ LONG val);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_y )( 
            IVLCMarquee * This,
            /* [retval][out] */ LONG *val);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_y )( 
            IVLCMarquee * This,
            /* [in] */ LONG val);
        
        END_INTERFACE
    } IVLCMarqueeVtbl;

    interface IVLCMarquee
    {
        CONST_VTBL struct IVLCMarqueeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVLCMarquee_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVLCMarquee_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVLCMarquee_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVLCMarquee_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVLCMarquee_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVLCMarquee_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVLCMarquee_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVLCMarquee_enable(This)	\
    ( (This)->lpVtbl -> enable(This) ) 

#define IVLCMarquee_disable(This)	\
    ( (This)->lpVtbl -> disable(This) ) 

#define IVLCMarquee_get_text(This,val)	\
    ( (This)->lpVtbl -> get_text(This,val) ) 

#define IVLCMarquee_put_text(This,val)	\
    ( (This)->lpVtbl -> put_text(This,val) ) 

#define IVLCMarquee_get_color(This,val)	\
    ( (This)->lpVtbl -> get_color(This,val) ) 

#define IVLCMarquee_put_color(This,val)	\
    ( (This)->lpVtbl -> put_color(This,val) ) 

#define IVLCMarquee_get_opacity(This,val)	\
    ( (This)->lpVtbl -> get_opacity(This,val) ) 

#define IVLCMarquee_put_opacity(This,val)	\
    ( (This)->lpVtbl -> put_opacity(This,val) ) 

#define IVLCMarquee_get_position(This,val)	\
    ( (This)->lpVtbl -> get_position(This,val) ) 

#define IVLCMarquee_put_position(This,val)	\
    ( (This)->lpVtbl -> put_position(This,val) ) 

#define IVLCMarquee_get_refresh(This,val)	\
    ( (This)->lpVtbl -> get_refresh(This,val) ) 

#define IVLCMarquee_put_refresh(This,val)	\
    ( (This)->lpVtbl -> put_refresh(This,val) ) 

#define IVLCMarquee_get_size(This,val)	\
    ( (This)->lpVtbl -> get_size(This,val) ) 

#define IVLCMarquee_put_size(This,val)	\
    ( (This)->lpVtbl -> put_size(This,val) ) 

#define IVLCMarquee_get_timeout(This,val)	\
    ( (This)->lpVtbl -> get_timeout(This,val) ) 

#define IVLCMarquee_put_timeout(This,val)	\
    ( (This)->lpVtbl -> put_timeout(This,val) ) 

#define IVLCMarquee_get_x(This,val)	\
    ( (This)->lpVtbl -> get_x(This,val) ) 

#define IVLCMarquee_put_x(This,val)	\
    ( (This)->lpVtbl -> put_x(This,val) ) 

#define IVLCMarquee_get_y(This,val)	\
    ( (This)->lpVtbl -> get_y(This,val) ) 

#define IVLCMarquee_put_y(This,val)	\
    ( (This)->lpVtbl -> put_y(This,val) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVLCMarquee_INTERFACE_DEFINED__ */


#ifndef __IVLCPlaylist_INTERFACE_DEFINED__
#define __IVLCPlaylist_INTERFACE_DEFINED__

/* interface IVLCPlaylist */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IVLCPlaylist;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("54613049-40BF-4035-9E70-0A9312C0188D")
    IVLCPlaylist : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_itemCount( 
            /* [retval][out] */ long *count) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_isPlaying( 
            /* [retval][out] */ VARIANT_BOOL *playing) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_currentItem( 
            /* [retval][out] */ long *index) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE add( 
            /* [in] */ BSTR uri,
            /* [optional][in] */ VARIANT name,
            /* [optional][in] */ VARIANT options,
            /* [retval][out] */ long *itemId) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE play( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE playItem( 
            /* [in] */ long itemId) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE pause( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE togglePause( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE stop( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE next( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE prev( void) = 0;
        
        virtual /* [helpstring][hidden] */ HRESULT STDMETHODCALLTYPE clear( void) = 0;
        
        virtual /* [helpstring][hidden] */ HRESULT STDMETHODCALLTYPE removeItem( 
            /* [in] */ long item) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_items( 
            /* [retval][out] */ IVLCPlaylistItems **obj) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVLCPlaylistVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVLCPlaylist * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVLCPlaylist * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVLCPlaylist * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVLCPlaylist * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVLCPlaylist * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVLCPlaylist * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVLCPlaylist * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_itemCount )( 
            IVLCPlaylist * This,
            /* [retval][out] */ long *count);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_isPlaying )( 
            IVLCPlaylist * This,
            /* [retval][out] */ VARIANT_BOOL *playing);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_currentItem )( 
            IVLCPlaylist * This,
            /* [retval][out] */ long *index);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *add )( 
            IVLCPlaylist * This,
            /* [in] */ BSTR uri,
            /* [optional][in] */ VARIANT name,
            /* [optional][in] */ VARIANT options,
            /* [retval][out] */ long *itemId);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *play )( 
            IVLCPlaylist * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *playItem )( 
            IVLCPlaylist * This,
            /* [in] */ long itemId);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *pause )( 
            IVLCPlaylist * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *togglePause )( 
            IVLCPlaylist * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *stop )( 
            IVLCPlaylist * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *next )( 
            IVLCPlaylist * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *prev )( 
            IVLCPlaylist * This);
        
        /* [helpstring][hidden] */ HRESULT ( STDMETHODCALLTYPE *clear )( 
            IVLCPlaylist * This);
        
        /* [helpstring][hidden] */ HRESULT ( STDMETHODCALLTYPE *removeItem )( 
            IVLCPlaylist * This,
            /* [in] */ long item);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_items )( 
            IVLCPlaylist * This,
            /* [retval][out] */ IVLCPlaylistItems **obj);
        
        END_INTERFACE
    } IVLCPlaylistVtbl;

    interface IVLCPlaylist
    {
        CONST_VTBL struct IVLCPlaylistVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVLCPlaylist_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVLCPlaylist_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVLCPlaylist_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVLCPlaylist_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVLCPlaylist_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVLCPlaylist_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVLCPlaylist_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVLCPlaylist_get_itemCount(This,count)	\
    ( (This)->lpVtbl -> get_itemCount(This,count) ) 

#define IVLCPlaylist_get_isPlaying(This,playing)	\
    ( (This)->lpVtbl -> get_isPlaying(This,playing) ) 

#define IVLCPlaylist_get_currentItem(This,index)	\
    ( (This)->lpVtbl -> get_currentItem(This,index) ) 

#define IVLCPlaylist_add(This,uri,name,options,itemId)	\
    ( (This)->lpVtbl -> add(This,uri,name,options,itemId) ) 

#define IVLCPlaylist_play(This)	\
    ( (This)->lpVtbl -> play(This) ) 

#define IVLCPlaylist_playItem(This,itemId)	\
    ( (This)->lpVtbl -> playItem(This,itemId) ) 

#define IVLCPlaylist_pause(This)	\
    ( (This)->lpVtbl -> pause(This) ) 

#define IVLCPlaylist_togglePause(This)	\
    ( (This)->lpVtbl -> togglePause(This) ) 

#define IVLCPlaylist_stop(This)	\
    ( (This)->lpVtbl -> stop(This) ) 

#define IVLCPlaylist_next(This)	\
    ( (This)->lpVtbl -> next(This) ) 

#define IVLCPlaylist_prev(This)	\
    ( (This)->lpVtbl -> prev(This) ) 

#define IVLCPlaylist_clear(This)	\
    ( (This)->lpVtbl -> clear(This) ) 

#define IVLCPlaylist_removeItem(This,item)	\
    ( (This)->lpVtbl -> removeItem(This,item) ) 

#define IVLCPlaylist_get_items(This,obj)	\
    ( (This)->lpVtbl -> get_items(This,obj) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVLCPlaylist_INTERFACE_DEFINED__ */


#ifndef __IVLCSubtitle_INTERFACE_DEFINED__
#define __IVLCSubtitle_INTERFACE_DEFINED__

/* interface IVLCSubtitle */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IVLCSubtitle;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("465E787A-0556-452F-9477-954E4A940003")
    IVLCSubtitle : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_track( 
            /* [retval][out] */ long *spu) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_track( 
            /* [in] */ long spu) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_count( 
            /* [retval][out] */ long *spuNumber) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE description( 
            /* [in] */ long nameID,
            /* [retval][out] */ BSTR *name) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVLCSubtitleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVLCSubtitle * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVLCSubtitle * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVLCSubtitle * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVLCSubtitle * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVLCSubtitle * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVLCSubtitle * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVLCSubtitle * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_track )( 
            IVLCSubtitle * This,
            /* [retval][out] */ long *spu);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_track )( 
            IVLCSubtitle * This,
            /* [in] */ long spu);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_count )( 
            IVLCSubtitle * This,
            /* [retval][out] */ long *spuNumber);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *description )( 
            IVLCSubtitle * This,
            /* [in] */ long nameID,
            /* [retval][out] */ BSTR *name);
        
        END_INTERFACE
    } IVLCSubtitleVtbl;

    interface IVLCSubtitle
    {
        CONST_VTBL struct IVLCSubtitleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVLCSubtitle_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVLCSubtitle_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVLCSubtitle_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVLCSubtitle_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVLCSubtitle_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVLCSubtitle_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVLCSubtitle_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVLCSubtitle_get_track(This,spu)	\
    ( (This)->lpVtbl -> get_track(This,spu) ) 

#define IVLCSubtitle_put_track(This,spu)	\
    ( (This)->lpVtbl -> put_track(This,spu) ) 

#define IVLCSubtitle_get_count(This,spuNumber)	\
    ( (This)->lpVtbl -> get_count(This,spuNumber) ) 

#define IVLCSubtitle_description(This,nameID,name)	\
    ( (This)->lpVtbl -> description(This,nameID,name) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVLCSubtitle_INTERFACE_DEFINED__ */


#ifndef __IVLCVideo_INTERFACE_DEFINED__
#define __IVLCVideo_INTERFACE_DEFINED__

/* interface IVLCVideo */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IVLCVideo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0AAEDF0B-D333-4B27-A0C6-BBF31413A42E")
    IVLCVideo : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_fullscreen( 
            /* [retval][out] */ VARIANT_BOOL *fullscreen) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_fullscreen( 
            /* [in] */ VARIANT_BOOL fullscreen) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_width( 
            /* [retval][out] */ long *width) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_height( 
            /* [retval][out] */ long *height) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_aspectRatio( 
            /* [retval][out] */ BSTR *aspect) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_aspectRatio( 
            /* [in] */ BSTR aspect) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_subtitle( 
            /* [retval][out] */ long *spu) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_subtitle( 
            /* [in] */ long spu) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_crop( 
            /* [retval][out] */ BSTR *geometry) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_crop( 
            /* [in] */ BSTR geometry) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_teletext( 
            /* [retval][out] */ long *page) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_teletext( 
            /* [in] */ long page) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE toggleFullscreen( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE takeSnapshot( 
            /* [retval][out] */ IPictureDisp **picture) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE toggleTeletext( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_marquee( 
            /* [retval][out] */ IVLCMarquee **obj) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_logo( 
            /* [retval][out] */ IVLCLogo **obj) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_deinterlace( 
            /* [retval][out] */ IVLCDeinterlace **obj) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVLCVideoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVLCVideo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVLCVideo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVLCVideo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVLCVideo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVLCVideo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVLCVideo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVLCVideo * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_fullscreen )( 
            IVLCVideo * This,
            /* [retval][out] */ VARIANT_BOOL *fullscreen);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_fullscreen )( 
            IVLCVideo * This,
            /* [in] */ VARIANT_BOOL fullscreen);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_width )( 
            IVLCVideo * This,
            /* [retval][out] */ long *width);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_height )( 
            IVLCVideo * This,
            /* [retval][out] */ long *height);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_aspectRatio )( 
            IVLCVideo * This,
            /* [retval][out] */ BSTR *aspect);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_aspectRatio )( 
            IVLCVideo * This,
            /* [in] */ BSTR aspect);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_subtitle )( 
            IVLCVideo * This,
            /* [retval][out] */ long *spu);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_subtitle )( 
            IVLCVideo * This,
            /* [in] */ long spu);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_crop )( 
            IVLCVideo * This,
            /* [retval][out] */ BSTR *geometry);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_crop )( 
            IVLCVideo * This,
            /* [in] */ BSTR geometry);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_teletext )( 
            IVLCVideo * This,
            /* [retval][out] */ long *page);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_teletext )( 
            IVLCVideo * This,
            /* [in] */ long page);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *toggleFullscreen )( 
            IVLCVideo * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *takeSnapshot )( 
            IVLCVideo * This,
            /* [retval][out] */ IPictureDisp **picture);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *toggleTeletext )( 
            IVLCVideo * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_marquee )( 
            IVLCVideo * This,
            /* [retval][out] */ IVLCMarquee **obj);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_logo )( 
            IVLCVideo * This,
            /* [retval][out] */ IVLCLogo **obj);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_deinterlace )( 
            IVLCVideo * This,
            /* [retval][out] */ IVLCDeinterlace **obj);
        
        END_INTERFACE
    } IVLCVideoVtbl;

    interface IVLCVideo
    {
        CONST_VTBL struct IVLCVideoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVLCVideo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVLCVideo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVLCVideo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVLCVideo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVLCVideo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVLCVideo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVLCVideo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVLCVideo_get_fullscreen(This,fullscreen)	\
    ( (This)->lpVtbl -> get_fullscreen(This,fullscreen) ) 

#define IVLCVideo_put_fullscreen(This,fullscreen)	\
    ( (This)->lpVtbl -> put_fullscreen(This,fullscreen) ) 

#define IVLCVideo_get_width(This,width)	\
    ( (This)->lpVtbl -> get_width(This,width) ) 

#define IVLCVideo_get_height(This,height)	\
    ( (This)->lpVtbl -> get_height(This,height) ) 

#define IVLCVideo_get_aspectRatio(This,aspect)	\
    ( (This)->lpVtbl -> get_aspectRatio(This,aspect) ) 

#define IVLCVideo_put_aspectRatio(This,aspect)	\
    ( (This)->lpVtbl -> put_aspectRatio(This,aspect) ) 

#define IVLCVideo_get_subtitle(This,spu)	\
    ( (This)->lpVtbl -> get_subtitle(This,spu) ) 

#define IVLCVideo_put_subtitle(This,spu)	\
    ( (This)->lpVtbl -> put_subtitle(This,spu) ) 

#define IVLCVideo_get_crop(This,geometry)	\
    ( (This)->lpVtbl -> get_crop(This,geometry) ) 

#define IVLCVideo_put_crop(This,geometry)	\
    ( (This)->lpVtbl -> put_crop(This,geometry) ) 

#define IVLCVideo_get_teletext(This,page)	\
    ( (This)->lpVtbl -> get_teletext(This,page) ) 

#define IVLCVideo_put_teletext(This,page)	\
    ( (This)->lpVtbl -> put_teletext(This,page) ) 

#define IVLCVideo_toggleFullscreen(This)	\
    ( (This)->lpVtbl -> toggleFullscreen(This) ) 

#define IVLCVideo_takeSnapshot(This,picture)	\
    ( (This)->lpVtbl -> takeSnapshot(This,picture) ) 

#define IVLCVideo_toggleTeletext(This)	\
    ( (This)->lpVtbl -> toggleTeletext(This) ) 

#define IVLCVideo_get_marquee(This,obj)	\
    ( (This)->lpVtbl -> get_marquee(This,obj) ) 

#define IVLCVideo_get_logo(This,obj)	\
    ( (This)->lpVtbl -> get_logo(This,obj) ) 

#define IVLCVideo_get_deinterlace(This,obj)	\
    ( (This)->lpVtbl -> get_deinterlace(This,obj) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVLCVideo_INTERFACE_DEFINED__ */


#ifndef __IVLCControl2_INTERFACE_DEFINED__
#define __IVLCControl2_INTERFACE_DEFINED__

/* interface IVLCControl2 */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IVLCControl2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2D719729-5333-406C-BF12-8DE787FD65E3")
    IVLCControl2 : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AutoLoop( 
            /* [retval][out] */ VARIANT_BOOL *autoloop) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_AutoLoop( 
            /* [in] */ VARIANT_BOOL autoloop) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AutoPlay( 
            /* [retval][out] */ VARIANT_BOOL *autoplay) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_AutoPlay( 
            /* [in] */ VARIANT_BOOL autoplay) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_BaseURL( 
            /* [retval][out] */ BSTR *url) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_BaseURL( 
            /* [in] */ BSTR url) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_StartTime( 
            /* [retval][out] */ long *seconds) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_StartTime( 
            /* [in] */ long seconds) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MRL( 
            /* [retval][out] */ BSTR *mrl) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_MRL( 
            /* [in] */ BSTR mrl) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_VersionInfo( 
            /* [retval][out] */ BSTR *version) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Visible( 
            /* [retval][out] */ VARIANT_BOOL *visible) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Visible( 
            /* [in] */ VARIANT_BOOL visible) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Volume( 
            /* [retval][out] */ long *volume) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Volume( 
            /* [in] */ long volume) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_BackColor( 
            /* [retval][out] */ OLE_COLOR *backcolor) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_BackColor( 
            /* [in] */ OLE_COLOR backcolor) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Toolbar( 
            /* [retval][out] */ VARIANT_BOOL *visible) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Toolbar( 
            /* [in] */ VARIANT_BOOL visible) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_audio( 
            /* [retval][out] */ IVLCAudio **obj) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_input( 
            /* [retval][out] */ IVLCInput **obj) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_playlist( 
            /* [retval][out] */ IVLCPlaylist **obj) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_subtitle( 
            /* [retval][out] */ IVLCSubtitle **obj) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_video( 
            /* [retval][out] */ IVLCVideo **obj) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_FullscreenEnabled( 
            /* [retval][out] */ VARIANT_BOOL *enabled) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_FullscreenEnabled( 
            /* [in] */ VARIANT_BOOL enabled) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_mediaDescription( 
            /* [retval][out] */ IVLCMediaDescription **obj) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVLCControl2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVLCControl2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVLCControl2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVLCControl2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVLCControl2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVLCControl2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVLCControl2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVLCControl2 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AutoLoop )( 
            IVLCControl2 * This,
            /* [retval][out] */ VARIANT_BOOL *autoloop);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AutoLoop )( 
            IVLCControl2 * This,
            /* [in] */ VARIANT_BOOL autoloop);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AutoPlay )( 
            IVLCControl2 * This,
            /* [retval][out] */ VARIANT_BOOL *autoplay);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AutoPlay )( 
            IVLCControl2 * This,
            /* [in] */ VARIANT_BOOL autoplay);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_BaseURL )( 
            IVLCControl2 * This,
            /* [retval][out] */ BSTR *url);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_BaseURL )( 
            IVLCControl2 * This,
            /* [in] */ BSTR url);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_StartTime )( 
            IVLCControl2 * This,
            /* [retval][out] */ long *seconds);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_StartTime )( 
            IVLCControl2 * This,
            /* [in] */ long seconds);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_MRL )( 
            IVLCControl2 * This,
            /* [retval][out] */ BSTR *mrl);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_MRL )( 
            IVLCControl2 * This,
            /* [in] */ BSTR mrl);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VersionInfo )( 
            IVLCControl2 * This,
            /* [retval][out] */ BSTR *version);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Visible )( 
            IVLCControl2 * This,
            /* [retval][out] */ VARIANT_BOOL *visible);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Visible )( 
            IVLCControl2 * This,
            /* [in] */ VARIANT_BOOL visible);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Volume )( 
            IVLCControl2 * This,
            /* [retval][out] */ long *volume);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Volume )( 
            IVLCControl2 * This,
            /* [in] */ long volume);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_BackColor )( 
            IVLCControl2 * This,
            /* [retval][out] */ OLE_COLOR *backcolor);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_BackColor )( 
            IVLCControl2 * This,
            /* [in] */ OLE_COLOR backcolor);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Toolbar )( 
            IVLCControl2 * This,
            /* [retval][out] */ VARIANT_BOOL *visible);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Toolbar )( 
            IVLCControl2 * This,
            /* [in] */ VARIANT_BOOL visible);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_audio )( 
            IVLCControl2 * This,
            /* [retval][out] */ IVLCAudio **obj);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_input )( 
            IVLCControl2 * This,
            /* [retval][out] */ IVLCInput **obj);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_playlist )( 
            IVLCControl2 * This,
            /* [retval][out] */ IVLCPlaylist **obj);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_subtitle )( 
            IVLCControl2 * This,
            /* [retval][out] */ IVLCSubtitle **obj);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_video )( 
            IVLCControl2 * This,
            /* [retval][out] */ IVLCVideo **obj);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_FullscreenEnabled )( 
            IVLCControl2 * This,
            /* [retval][out] */ VARIANT_BOOL *enabled);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_FullscreenEnabled )( 
            IVLCControl2 * This,
            /* [in] */ VARIANT_BOOL enabled);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_mediaDescription )( 
            IVLCControl2 * This,
            /* [retval][out] */ IVLCMediaDescription **obj);
        
        END_INTERFACE
    } IVLCControl2Vtbl;

    interface IVLCControl2
    {
        CONST_VTBL struct IVLCControl2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVLCControl2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVLCControl2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVLCControl2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVLCControl2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVLCControl2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVLCControl2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVLCControl2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVLCControl2_get_AutoLoop(This,autoloop)	\
    ( (This)->lpVtbl -> get_AutoLoop(This,autoloop) ) 

#define IVLCControl2_put_AutoLoop(This,autoloop)	\
    ( (This)->lpVtbl -> put_AutoLoop(This,autoloop) ) 

#define IVLCControl2_get_AutoPlay(This,autoplay)	\
    ( (This)->lpVtbl -> get_AutoPlay(This,autoplay) ) 

#define IVLCControl2_put_AutoPlay(This,autoplay)	\
    ( (This)->lpVtbl -> put_AutoPlay(This,autoplay) ) 

#define IVLCControl2_get_BaseURL(This,url)	\
    ( (This)->lpVtbl -> get_BaseURL(This,url) ) 

#define IVLCControl2_put_BaseURL(This,url)	\
    ( (This)->lpVtbl -> put_BaseURL(This,url) ) 

#define IVLCControl2_get_StartTime(This,seconds)	\
    ( (This)->lpVtbl -> get_StartTime(This,seconds) ) 

#define IVLCControl2_put_StartTime(This,seconds)	\
    ( (This)->lpVtbl -> put_StartTime(This,seconds) ) 

#define IVLCControl2_get_MRL(This,mrl)	\
    ( (This)->lpVtbl -> get_MRL(This,mrl) ) 

#define IVLCControl2_put_MRL(This,mrl)	\
    ( (This)->lpVtbl -> put_MRL(This,mrl) ) 

#define IVLCControl2_get_VersionInfo(This,version)	\
    ( (This)->lpVtbl -> get_VersionInfo(This,version) ) 

#define IVLCControl2_get_Visible(This,visible)	\
    ( (This)->lpVtbl -> get_Visible(This,visible) ) 

#define IVLCControl2_put_Visible(This,visible)	\
    ( (This)->lpVtbl -> put_Visible(This,visible) ) 

#define IVLCControl2_get_Volume(This,volume)	\
    ( (This)->lpVtbl -> get_Volume(This,volume) ) 

#define IVLCControl2_put_Volume(This,volume)	\
    ( (This)->lpVtbl -> put_Volume(This,volume) ) 

#define IVLCControl2_get_BackColor(This,backcolor)	\
    ( (This)->lpVtbl -> get_BackColor(This,backcolor) ) 

#define IVLCControl2_put_BackColor(This,backcolor)	\
    ( (This)->lpVtbl -> put_BackColor(This,backcolor) ) 

#define IVLCControl2_get_Toolbar(This,visible)	\
    ( (This)->lpVtbl -> get_Toolbar(This,visible) ) 

#define IVLCControl2_put_Toolbar(This,visible)	\
    ( (This)->lpVtbl -> put_Toolbar(This,visible) ) 

#define IVLCControl2_get_audio(This,obj)	\
    ( (This)->lpVtbl -> get_audio(This,obj) ) 

#define IVLCControl2_get_input(This,obj)	\
    ( (This)->lpVtbl -> get_input(This,obj) ) 

#define IVLCControl2_get_playlist(This,obj)	\
    ( (This)->lpVtbl -> get_playlist(This,obj) ) 

#define IVLCControl2_get_subtitle(This,obj)	\
    ( (This)->lpVtbl -> get_subtitle(This,obj) ) 

#define IVLCControl2_get_video(This,obj)	\
    ( (This)->lpVtbl -> get_video(This,obj) ) 

#define IVLCControl2_get_FullscreenEnabled(This,enabled)	\
    ( (This)->lpVtbl -> get_FullscreenEnabled(This,enabled) ) 

#define IVLCControl2_put_FullscreenEnabled(This,enabled)	\
    ( (This)->lpVtbl -> put_FullscreenEnabled(This,enabled) ) 

#define IVLCControl2_get_mediaDescription(This,obj)	\
    ( (This)->lpVtbl -> get_mediaDescription(This,obj) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVLCControl2_INTERFACE_DEFINED__ */


#ifndef __DVLCEvents_DISPINTERFACE_DEFINED__
#define __DVLCEvents_DISPINTERFACE_DEFINED__

/* dispinterface DVLCEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID_DVLCEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("DF48072F-5EF8-434e-9B40-E2F3AE759B5F")
    DVLCEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct DVLCEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            DVLCEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            DVLCEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            DVLCEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            DVLCEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            DVLCEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            DVLCEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            DVLCEvents * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } DVLCEventsVtbl;

    interface DVLCEvents
    {
        CONST_VTBL struct DVLCEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define DVLCEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define DVLCEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define DVLCEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define DVLCEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define DVLCEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define DVLCEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define DVLCEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __DVLCEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IVLCPlaylistItems_INTERFACE_DEFINED__
#define __IVLCPlaylistItems_INTERFACE_DEFINED__

/* interface IVLCPlaylistItems */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IVLCPlaylistItems;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FD37FE32-82BC-4A25-B056-315F4DBB194D")
    IVLCPlaylistItems : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_count( 
            /* [retval][out] */ long *count) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE clear( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE remove( 
            /* [in] */ long itemId) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVLCPlaylistItemsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVLCPlaylistItems * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVLCPlaylistItems * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVLCPlaylistItems * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVLCPlaylistItems * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVLCPlaylistItems * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVLCPlaylistItems * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVLCPlaylistItems * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_count )( 
            IVLCPlaylistItems * This,
            /* [retval][out] */ long *count);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *clear )( 
            IVLCPlaylistItems * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *remove )( 
            IVLCPlaylistItems * This,
            /* [in] */ long itemId);
        
        END_INTERFACE
    } IVLCPlaylistItemsVtbl;

    interface IVLCPlaylistItems
    {
        CONST_VTBL struct IVLCPlaylistItemsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVLCPlaylistItems_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVLCPlaylistItems_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVLCPlaylistItems_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVLCPlaylistItems_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVLCPlaylistItems_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVLCPlaylistItems_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVLCPlaylistItems_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVLCPlaylistItems_get_count(This,count)	\
    ( (This)->lpVtbl -> get_count(This,count) ) 

#define IVLCPlaylistItems_clear(This)	\
    ( (This)->lpVtbl -> clear(This) ) 

#define IVLCPlaylistItems_remove(This,itemId)	\
    ( (This)->lpVtbl -> remove(This,itemId) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVLCPlaylistItems_INTERFACE_DEFINED__ */


#ifndef __IVLCMediaDescription_INTERFACE_DEFINED__
#define __IVLCMediaDescription_INTERFACE_DEFINED__

/* interface IVLCMediaDescription */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IVLCMediaDescription;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("796A2C2D-5B11-4fb5-9077-56D5E674972B")
    IVLCMediaDescription : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_title( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_artist( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_genre( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_copyright( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_album( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_trackNumber( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_description( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_rating( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_date( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_setting( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_URL( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_language( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_nowPlaying( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_publisher( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_encodedBy( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_artworkURL( 
            /* [retval][out] */ BSTR *val) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_trackID( 
            /* [retval][out] */ BSTR *val) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVLCMediaDescriptionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVLCMediaDescription * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVLCMediaDescription * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVLCMediaDescription * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVLCMediaDescription * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVLCMediaDescription * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVLCMediaDescription * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVLCMediaDescription * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_title )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_artist )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_genre )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_copyright )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_album )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_trackNumber )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_description )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_rating )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_date )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_setting )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_URL )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_language )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_nowPlaying )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_publisher )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_encodedBy )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_artworkURL )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_trackID )( 
            IVLCMediaDescription * This,
            /* [retval][out] */ BSTR *val);
        
        END_INTERFACE
    } IVLCMediaDescriptionVtbl;

    interface IVLCMediaDescription
    {
        CONST_VTBL struct IVLCMediaDescriptionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVLCMediaDescription_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVLCMediaDescription_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVLCMediaDescription_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVLCMediaDescription_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVLCMediaDescription_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVLCMediaDescription_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVLCMediaDescription_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVLCMediaDescription_get_title(This,val)	\
    ( (This)->lpVtbl -> get_title(This,val) ) 

#define IVLCMediaDescription_get_artist(This,val)	\
    ( (This)->lpVtbl -> get_artist(This,val) ) 

#define IVLCMediaDescription_get_genre(This,val)	\
    ( (This)->lpVtbl -> get_genre(This,val) ) 

#define IVLCMediaDescription_get_copyright(This,val)	\
    ( (This)->lpVtbl -> get_copyright(This,val) ) 

#define IVLCMediaDescription_get_album(This,val)	\
    ( (This)->lpVtbl -> get_album(This,val) ) 

#define IVLCMediaDescription_get_trackNumber(This,val)	\
    ( (This)->lpVtbl -> get_trackNumber(This,val) ) 

#define IVLCMediaDescription_get_description(This,val)	\
    ( (This)->lpVtbl -> get_description(This,val) ) 

#define IVLCMediaDescription_get_rating(This,val)	\
    ( (This)->lpVtbl -> get_rating(This,val) ) 

#define IVLCMediaDescription_get_date(This,val)	\
    ( (This)->lpVtbl -> get_date(This,val) ) 

#define IVLCMediaDescription_get_setting(This,val)	\
    ( (This)->lpVtbl -> get_setting(This,val) ) 

#define IVLCMediaDescription_get_URL(This,val)	\
    ( (This)->lpVtbl -> get_URL(This,val) ) 

#define IVLCMediaDescription_get_language(This,val)	\
    ( (This)->lpVtbl -> get_language(This,val) ) 

#define IVLCMediaDescription_get_nowPlaying(This,val)	\
    ( (This)->lpVtbl -> get_nowPlaying(This,val) ) 

#define IVLCMediaDescription_get_publisher(This,val)	\
    ( (This)->lpVtbl -> get_publisher(This,val) ) 

#define IVLCMediaDescription_get_encodedBy(This,val)	\
    ( (This)->lpVtbl -> get_encodedBy(This,val) ) 

#define IVLCMediaDescription_get_artworkURL(This,val)	\
    ( (This)->lpVtbl -> get_artworkURL(This,val) ) 

#define IVLCMediaDescription_get_trackID(This,val)	\
    ( (This)->lpVtbl -> get_trackID(This,val) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVLCMediaDescription_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_VLCPlugin;

#ifdef __cplusplus

class DECLSPEC_UUID("E23FE9C6-778E-49D4-B537-38FCDE4887D8")
VLCPlugin;
#endif

EXTERN_C const CLSID CLSID_VLCPlugin2;

#ifdef __cplusplus

class DECLSPEC_UUID("9BE31822-FDAD-461B-AD51-BE1D1C159921")
VLCPlugin2;
#endif
#endif /* __AXVLC_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


