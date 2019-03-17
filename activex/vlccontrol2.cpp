/*****************************************************************************
 * vlccontrol2.cpp: ActiveX control for VLC
 *****************************************************************************
 * Copyright (C) 2006 the VideoLAN team
 * Copyright (C) 2010 M2X BV
 *
 * Authors: Damien Fouilleul <Damien.Fouilleul@laposte.net>
 *          Jean-Paul Saman <jpsaman _at_ m2x _dot_ nl>
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
#include "config.h"

#include <stdio.h>
#include <shlwapi.h>
#include <wininet.h>
#include <tchar.h>

#include "utils.h"
#include "plugin.h"
#include "vlccontrol2.h"
#include "vlccontrol.h"

#include "../common/position.h"

// ---------

HRESULT VLCInterfaceBase::loadTypeInfo(REFIID riid)
{
    // if( _ti ) return NOERROR; // unnecessairy

    ITypeLib *p_typelib;
    HRESULT hr = _plug->getTypeLib(LOCALE_USER_DEFAULT, &p_typelib);
    if( SUCCEEDED(hr) )
    {
        hr = p_typelib->GetTypeInfoOfGuid(riid, &_ti);
        if( FAILED(hr) ) _ti = NULL;
        p_typelib->Release();
    }
    return hr;
}

#define BIND_INTERFACE( class ) \
    template<> REFIID VLCInterface<class, I##class>::_riid = IID_I##class;

BIND_INTERFACE( VLCAudio )
BIND_INTERFACE( VLCInput )
BIND_INTERFACE( VLCMarquee )
BIND_INTERFACE( VLCLogo )
BIND_INTERFACE( VLCDeinterlace )
BIND_INTERFACE( VLCPlaylistItems )
BIND_INTERFACE( VLCPlaylist )
BIND_INTERFACE( VLCVideo )
BIND_INTERFACE( VLCSubtitle )
BIND_INTERFACE( VLCMediaDescription )

#undef  BIND_INTERFACE

template<class I> static inline
HRESULT object_get(I **dst, I *src)
{
    if( NULL == dst )
        return E_POINTER;

    *dst = src;
    if( NULL != src )
    {
        src->AddRef();
        return NOERROR;
    }
    return E_OUTOFMEMORY;
}

static inline
VARIANT_BOOL varbool(bool b) { return b ? VARIANT_TRUE : VARIANT_FALSE; }

// ---------

STDMETHODIMP VLCAudio::get_mute(VARIANT_BOOL* mute)
{
    if( NULL == mute )
        return E_POINTER;

    *mute = varbool( Instance()->get_player().is_muted() );

    return S_OK;
};

STDMETHODIMP VLCAudio::put_mute(VARIANT_BOOL mute)
{
    Instance()->get_player().set_mute( VARIANT_FALSE != mute );

    return S_OK;
};

STDMETHODIMP VLCAudio::get_volume(long* volume)
{
    if( NULL == volume )
        return E_POINTER;

    *volume = Instance()->get_player().get_volume();

    return S_OK;
};

STDMETHODIMP VLCAudio::put_volume(long volume)
{
    Instance()->get_player().set_volume( volume );

    return S_OK;
};

STDMETHODIMP VLCAudio::get_track(long* track)
{
    if( NULL == track )
        return E_POINTER;

    libvlc_media_player_t* p_md;

    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        // get the current internal audio track ID
        int i_actualTrack = libvlc_audio_get_track(p_md);

        //get the number of available track
        int i_audioTrackCount = libvlc_audio_get_track_count(p_md);
        if( i_audioTrackCount < 0 )
        {
            *track = i_actualTrack;
            return S_OK;
        }

        // get tracks description
        libvlc_track_description_t *p_currentTrack = libvlc_audio_get_track_description(p_md);
        if( !p_currentTrack )
            return E_FAIL;

        int i_fakeTrackIndex = 0;
        while( p_currentTrack )
        {
            if (i_actualTrack == p_currentTrack->i_id)
                break;

            p_currentTrack = p_currentTrack->p_next;
            i_fakeTrackIndex++;
        }
        libvlc_track_description_list_release(p_currentTrack);

        *track = i_fakeTrackIndex;
    }
    return hr;
};

STDMETHODIMP VLCAudio::put_track(long track)
{
    libvlc_media_player_t* p_md;

    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        // bounds checking
        int i_count = libvlc_audio_get_track_count(p_md);
        if (track >= i_count || i_count == 0)
            return E_INVALIDARG;

        libvlc_track_description_t *p_currentTrack = libvlc_audio_get_track_description(p_md);

        for ( int x = 0; x < track+1; x++ )
        {
            if (x == track)
                break;

            if (p_currentTrack->p_next)
                p_currentTrack = p_currentTrack->p_next;
            else
            {
                libvlc_track_description_list_release(p_currentTrack);
                return E_INVALIDARG;
            }
        }
        int actualTrack = p_currentTrack->i_id;
        libvlc_track_description_list_release(p_currentTrack);

        libvlc_audio_set_track(p_md, actualTrack);
    }
    return hr;
};

STDMETHODIMP VLCAudio::get_count(long* trackNumber)
{
    if( NULL == trackNumber )
        return E_POINTER;

    *trackNumber = Instance()->get_player().track_count();

    return S_OK;
};

STDMETHODIMP VLCAudio::description(long trackID, BSTR* name)
{
    if( NULL == name )
        return E_POINTER;

    libvlc_media_player_t* p_md;

    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        int i, i_limit;
        const char *psz_name;
        libvlc_track_description_t *p_trackDesc;

        // get tracks description
        p_trackDesc = libvlc_audio_get_track_description(p_md);
        if( !p_trackDesc )
        {
            libvlc_track_description_list_release(p_trackDesc);
            return E_FAIL;
        }

        //get the number of available track
        i_limit = libvlc_audio_get_track_count(p_md);
        if( i_limit < 0 )
        {
            libvlc_track_description_list_release(p_trackDesc);
            return E_FAIL;
        }

        // check if the number given is a good one
        if ( ( trackID > ( i_limit -1 ) ) || ( trackID < 0 ) )
        {
            libvlc_track_description_list_release(p_trackDesc);
            return E_INVALIDARG;
        }

        // get the good trackDesc
        for( i = 0 ; i < trackID ; i++ )
        {
            p_trackDesc = p_trackDesc->p_next;
        }
        // get the track name
        psz_name = p_trackDesc->psz_name;
        libvlc_track_description_list_release(p_trackDesc);

        // return it
        if( psz_name != NULL )
        {
            *name = BSTRFromCStr(CP_UTF8, psz_name);
            return (NULL == *name) ? E_OUTOFMEMORY : NOERROR;
        }
        *name = NULL;
        return E_FAIL;
    }
    return hr;
};

STDMETHODIMP VLCAudio::get_channel(long *channel)
{
    if( NULL == channel )
        return E_POINTER;

    *channel = Instance()->get_player().get_channel();

    return S_OK;
};

STDMETHODIMP VLCAudio::put_channel(long channel)
{
    Instance()->get_player().set_channel(channel);

    return S_OK;
};

STDMETHODIMP VLCAudio::toggleMute()
{
    Instance()->get_player().toggle_mute();

    return S_OK;
};

/****************************************************************************/

STDMETHODIMP VLCDeinterlace::disable()
{
    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        libvlc_video_set_deinterlace(p_md, "");
    }
    return hr;
}

STDMETHODIMP VLCDeinterlace::enable(BSTR mode)
{
    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        char *psz_mode = CStrFromBSTR(CP_UTF8, mode);
        libvlc_video_set_deinterlace(p_md, psz_mode);
        CoTaskMemFree(psz_mode);
    }
    return hr;
}

/****************************************************************************/

STDMETHODIMP VLCInput::get_length(double* length)
{
    if( NULL == length )
        return E_POINTER;

    *length = static_cast<double>(Instance()->get_player().get_length());

    return S_OK;
};

STDMETHODIMP VLCInput::get_position(double* position)
{
    if( NULL == position )
        return E_POINTER;

    *position = Instance()->get_player().get_position();

    return S_OK;
};

STDMETHODIMP VLCInput::put_position(double position)
{
    Instance()->get_player().set_position( static_cast<float>(position) );

    return S_OK;
};

STDMETHODIMP VLCInput::get_time(double* time)
{
    if( NULL == time )
        return E_POINTER;

    *time = static_cast<double>(Instance()->get_player().get_time());

    return S_OK;
};

STDMETHODIMP VLCInput::put_time(double time)
{
    Instance()->get_player().set_time(static_cast<libvlc_time_t>(time));

    return S_OK;
};

STDMETHODIMP VLCInput::get_state(long* state)
{
    if( NULL == state )
        return E_POINTER;

    *state = Instance()->get_player().get_state();

    return S_OK;
};

STDMETHODIMP VLCInput::get_rate(double* rate)
{
    if( NULL == rate )
        return E_POINTER;

    *rate = Instance()->get_player().get_rate();

    return S_OK;
};

STDMETHODIMP VLCInput::put_rate(double rate)
{
    Instance()->get_player().set_rate(static_cast<float>(rate));

    return S_OK;
};

STDMETHODIMP VLCInput::get_fps(double* fps)
{
    if( NULL == fps )
        return E_POINTER;

    *fps = static_cast<double>(Instance()->get_player().get_fps());

    return S_OK;
};

STDMETHODIMP VLCInput::get_hasVout(VARIANT_BOOL* hasVout)
{
    if( NULL == hasVout )
        return E_POINTER;

    *hasVout = varbool( Instance()->get_player().has_vout() );

    return S_OK;
};

/****************************************************************************/

HRESULT VLCMarquee::do_put_int(unsigned idx, LONG val)
{
    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        libvlc_video_set_marquee_int(p_md, idx, val);
    }
    return hr;
}

HRESULT VLCMarquee::do_get_int(unsigned idx, LONG *val)
{
    if( NULL == val )
        return E_POINTER;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        *val = libvlc_video_get_marquee_int(p_md, idx);
    }
    return hr;
}

STDMETHODIMP VLCMarquee::get_position(BSTR* val)
{
    if( NULL == val )
        return E_POINTER;

    LONG i;
    HRESULT hr = do_get_int(libvlc_marquee_Position, &i);
    if(SUCCEEDED(hr))
        *val = BSTRFromCStr(CP_UTF8, position_bynumber(i));

    return hr;
}

STDMETHODIMP VLCMarquee::put_position(BSTR val)
{
    char *n = CStrFromBSTR(CP_UTF8, val);
    if( !n ) return E_OUTOFMEMORY;

    size_t i;
    HRESULT hr;
    if( position_byname( n, i ) )
        hr = do_put_int(libvlc_marquee_Position,i);
    else
        hr = E_INVALIDARG;

    CoTaskMemFree(n);
    return hr;
}

STDMETHODIMP VLCMarquee::get_text(BSTR *val)
{
    char *psz;
    if( NULL == val )
        return E_POINTER;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        psz = libvlc_video_get_marquee_string(p_md, libvlc_marquee_Text);
        if( psz )
            *val = BSTRFromCStr(CP_UTF8, psz);
    }
    return hr;
}

STDMETHODIMP VLCMarquee::put_text(BSTR val)
{
    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        char *psz_text = CStrFromBSTR(CP_UTF8, val);
        libvlc_video_set_marquee_string(p_md, libvlc_marquee_Text, psz_text);
        CoTaskMemFree(psz_text);
    }
    return hr;
}

/****************************************************************************/

STDMETHODIMP VLCPlaylistItems::get_count(long* count)
{
    if( NULL == count )
        return E_POINTER;

    *count = Instance()->get_player().items_count();
    return S_OK;
};

STDMETHODIMP VLCPlaylistItems::clear()
{
    Instance()->get_player().clear_items();
    return S_OK;
};

STDMETHODIMP VLCPlaylistItems::remove(long item)
{
    Instance()->get_player().delete_item(item);
    return S_OK;
};

/****************************************************************************/

STDMETHODIMP VLCPlaylist::get_itemCount(long* count)
{
    if( NULL == count )
        return E_POINTER;

    *count = Instance()->get_player().items_count();

    return S_OK;
};

STDMETHODIMP VLCPlaylist::get_isPlaying(VARIANT_BOOL* isPlaying)
{
    if( NULL == isPlaying )
        return E_POINTER;

    *isPlaying = varbool( Instance()->get_player().is_playing()!=0 );

    return S_OK;
};

STDMETHODIMP VLCPlaylist::get_currentItem(long* index)
{
    if( NULL == index )
        return E_POINTER;

    *index = Instance()->get_player().current_item();

    return S_OK;
};

STDMETHODIMP VLCPlaylist::add(BSTR uri, VARIANT name, VARIANT options, long* item)
{
    if( NULL == item )
        return E_POINTER;

    if( 0 == SysStringLen(uri) )
        return E_INVALIDARG;

    libvlc_instance_t* p_libvlc;
    HRESULT hr = getVLC(&p_libvlc);
    if( SUCCEEDED(hr) )
    {
        char *psz_uri = NULL;
        if( SysStringLen(Instance()->getBaseURL()) > 0 )
        {
            /*
            ** if the MRL a relative URL, we should end up with an absolute URL
            */
            LPWSTR abs_url = CombineURL(Instance()->getBaseURL(), uri);
            if( NULL != abs_url )
            {
                psz_uri = CStrFromWSTR(CP_UTF8, abs_url, wcslen(abs_url));
                CoTaskMemFree(abs_url);
            }
            else
            {
                psz_uri = CStrFromBSTR(CP_UTF8, uri);
            }
        }
        else
        {
            /*
            ** baseURL is empty, assume MRL is absolute
            */
            psz_uri = CStrFromBSTR(CP_UTF8, uri);
        }

        if( NULL == psz_uri )
        {
            return E_OUTOFMEMORY;
        }

        int i_options;
        char **ppsz_options;

        hr = VLCControl::CreateTargetOptions(CP_UTF8, &options, &ppsz_options, &i_options);
        if( FAILED(hr) )
        {
            CoTaskMemFree(psz_uri);
            return hr;
        }

        char *psz_name = NULL;
        VARIANT v_name;
        VariantInit(&v_name);
        if( SUCCEEDED(VariantChangeType(&v_name, &name, 0, VT_BSTR)) )
        {
            if( SysStringLen(V_BSTR(&v_name)) > 0 )
                psz_name = CStrFromBSTR(CP_UTF8, V_BSTR(&v_name));

            VariantClear(&v_name);
        }

        *item = Instance()->playlist_add_extended_untrusted(psz_uri,
                    i_options, const_cast<const char **>(ppsz_options));

        VLCControl::FreeTargetOptions(ppsz_options, i_options);
        CoTaskMemFree(psz_uri);
        if( psz_name ) /* XXX Do we even need to check? */
            CoTaskMemFree(psz_name);
    }
    return hr;
};

STDMETHODIMP VLCPlaylist::play()
{
    Instance()->get_player().play();
    return S_OK;
};

STDMETHODIMP VLCPlaylist::playItem(long item)
{
    Instance()->get_player().play(item);
    return S_OK;
};

STDMETHODIMP VLCPlaylist::pause()
{
    Instance()->get_player().pause();
    return S_OK;
};

STDMETHODIMP VLCPlaylist::togglePause()
{
    Instance()->get_player().togglePause();
    return S_OK;
};

STDMETHODIMP VLCPlaylist::stop()
{
    Instance()->get_player().stop();
    return S_OK;
};

STDMETHODIMP VLCPlaylist::next()
{
    Instance()->get_player().next();
    return S_OK;
};

STDMETHODIMP VLCPlaylist::prev()
{
    Instance()->get_player().prev();
    return S_OK;
};

STDMETHODIMP VLCPlaylist::clear()
{
    Instance()->get_player().clear_items();
    return S_OK;
};

STDMETHODIMP VLCPlaylist::removeItem(long item)
{
    Instance()->get_player().delete_item(item);
    return S_OK;
};

STDMETHODIMP VLCPlaylist::get_items(IVLCPlaylistItems** obj)
{
    if( NULL == obj )
        return E_POINTER;

    *obj = _p_vlcplaylistitems;
    if( NULL != _p_vlcplaylistitems )
    {
        _p_vlcplaylistitems->AddRef();
        return NOERROR;
    }
    return E_OUTOFMEMORY;
};

/****************************************************************************/

STDMETHODIMP VLCSubtitle::get_track(long* spu)
{
    if( NULL == spu )
        return E_POINTER;

    libvlc_media_player_t* p_md;

    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        // get the current internal subtitles track ID
        int i_actualTrack = libvlc_video_get_spu(p_md);

        //get the number of available track
        int i_spuTrackCount = libvlc_video_get_spu_count(p_md);
        if( i_spuTrackCount < 0 )
        {
            *spu = i_actualTrack;
            return S_OK;
        }

        // get tracks description
        libvlc_track_description_t *p_currentTrack = libvlc_video_get_spu_description(p_md);
        if( !p_currentTrack )
            return E_FAIL;

        int i_fakeTrackIndex = 0;
        while( p_currentTrack )
        {
            if (i_actualTrack == p_currentTrack->i_id)
                break;

            p_currentTrack = p_currentTrack->p_next;
            i_fakeTrackIndex++;
        }
        libvlc_track_description_list_release(p_currentTrack);

        *spu = i_fakeTrackIndex;
    }
    return hr;
};

STDMETHODIMP VLCSubtitle::put_track(long spu)
{
    libvlc_media_player_t* p_md;

    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        // bounds checking
        int i_count = libvlc_video_get_spu_count(p_md);
        if (spu >= i_count || i_count == 0)
            return E_INVALIDARG;

        libvlc_track_description_t *p_currentTrack = libvlc_video_get_spu_description(p_md);

        for ( int x = 0; x < spu+1; x++ )
        {
            if (x == spu)
                break;

            if (p_currentTrack->p_next)
                p_currentTrack = p_currentTrack->p_next;
            else
            {
                libvlc_track_description_list_release(p_currentTrack);
                return E_INVALIDARG;
            }
        }
        int i_actualTrack = p_currentTrack->i_id;
        libvlc_track_description_list_release(p_currentTrack);

        libvlc_video_set_spu(p_md, i_actualTrack);
    }
    return hr;
};

STDMETHODIMP VLCSubtitle::get_count(long* spuNumber)
{
    if( NULL == spuNumber )
        return E_POINTER;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        // get the number of video subtitle available and return it
        *spuNumber = libvlc_video_get_spu_count(p_md);
    }
    return hr;
};


STDMETHODIMP VLCSubtitle::description(long nameID, BSTR* name)
{
    if( NULL == name )
       return E_POINTER;

    libvlc_media_player_t* p_md;

    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        int i, i_limit;
        const char *psz_name;
        libvlc_track_description_t *p_spuDesc;

        // get subtitles description
        p_spuDesc = libvlc_video_get_spu_description(p_md);
        if( !p_spuDesc )
        {
            libvlc_track_description_list_release(p_spuDesc);
            return E_FAIL;
        }

        // get the number of available subtitle
        i_limit = libvlc_video_get_spu_count(p_md);
        if( i_limit < 0 )
        {
            libvlc_track_description_list_release(p_spuDesc);
            return E_FAIL;
        }

        // check if the number given is a good one
        if ( ( nameID > ( i_limit -1 ) ) || ( nameID < 0 ) )
        {
            libvlc_track_description_list_release(p_spuDesc);
            return E_INVALIDARG;
        }

        // get the good spuDesc
        for( i = 0 ; i < nameID ; i++ )
        {
            p_spuDesc = p_spuDesc->p_next;
        }
        // get the subtitle name
        psz_name = p_spuDesc->psz_name;
        libvlc_track_description_list_release(p_spuDesc);

        // return it
        if( psz_name != NULL )
        {
            *name = BSTRFromCStr(CP_UTF8, psz_name);
            return (NULL == *name) ? E_OUTOFMEMORY : NOERROR;
        }
        *name = NULL;
        return E_FAIL;
    }
    return hr;
};

/****************************************************************************/

STDMETHODIMP VLCVideo::get_fullscreen(VARIANT_BOOL* fullscreen)
{
    if( NULL == fullscreen )
        return E_POINTER;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        *fullscreen = varbool( libvlc_get_fullscreen(p_md) );
    }
    return hr;
};

STDMETHODIMP VLCVideo::put_fullscreen(VARIANT_BOOL fullscreen)
{
    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        libvlc_set_fullscreen(p_md, VARIANT_FALSE != fullscreen);
    }
    return hr;
};

STDMETHODIMP VLCVideo::get_width(long* width)
{
    if( NULL == width )
        return E_POINTER;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        *width = libvlc_video_get_width(p_md);
    }
    return hr;
};

STDMETHODIMP VLCVideo::get_height(long* height)
{
    if( NULL == height )
        return E_POINTER;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        *height = libvlc_video_get_height(p_md);
    }
    return hr;
};

STDMETHODIMP VLCVideo::get_aspectRatio(BSTR* aspect)
{
    if( NULL == aspect )
        return E_POINTER;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        char *psz_aspect = libvlc_video_get_aspect_ratio(p_md);

        if( !psz_aspect )
        {
            *aspect = BSTRFromCStr(CP_UTF8, psz_aspect);
            if( NULL == *aspect )
                hr = E_OUTOFMEMORY;
        } else if( NULL == psz_aspect)
                hr = E_OUTOFMEMORY;
        free( psz_aspect );
    }
    return hr;
};

STDMETHODIMP VLCVideo::put_aspectRatio(BSTR aspect)
{
    if( NULL == aspect )
        return E_POINTER;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        char *psz_aspect = CStrFromBSTR(CP_UTF8, aspect);
        if( !psz_aspect )
        {
            return E_OUTOFMEMORY;
        }

        libvlc_video_set_aspect_ratio(p_md, psz_aspect);

        CoTaskMemFree(psz_aspect);
    }
    return hr;
};

STDMETHODIMP VLCVideo::get_subtitle(long* spu)
{
    if( NULL == spu )
        return E_POINTER;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        *spu = libvlc_video_get_spu(p_md);
    }
    return hr;
};

STDMETHODIMP VLCVideo::put_subtitle(long spu)
{
    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        libvlc_video_set_spu(p_md, spu);
    }
    return hr;
};

STDMETHODIMP VLCVideo::get_crop(BSTR* geometry)
{
    if( NULL == geometry )
        return E_POINTER;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        char *psz_geometry = libvlc_video_get_crop_geometry(p_md);
        if( !psz_geometry )
        {
            *geometry = BSTRFromCStr(CP_UTF8, psz_geometry);
            if( !geometry )
                hr = E_OUTOFMEMORY;
        } else if( !psz_geometry )
                hr = E_OUTOFMEMORY;
        free( psz_geometry );
    }
    return hr;
};

STDMETHODIMP VLCVideo::put_crop(BSTR geometry)
{
    if( NULL == geometry )
        return E_POINTER;

    if( 0 == SysStringLen(geometry) )
        return E_INVALIDARG;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        char *psz_geometry = CStrFromBSTR(CP_UTF8, geometry);
        if( !psz_geometry )
        {
            return E_OUTOFMEMORY;
        }

        libvlc_video_set_crop_geometry(p_md, psz_geometry);

        CoTaskMemFree(psz_geometry);
    }
    return hr;
};

STDMETHODIMP VLCVideo::get_teletext(long* page)
{
    if( NULL == page )
        return E_POINTER;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        *page = libvlc_video_get_teletext(p_md);
    }

    return hr;
};

STDMETHODIMP VLCVideo::put_teletext(long page)
{
    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);

    if( SUCCEEDED(hr) )
    {
        libvlc_video_set_teletext(p_md, page);
    }
    return hr;
};

STDMETHODIMP VLCVideo::takeSnapshot(LPPICTUREDISP* picture)
{
    if( NULL == picture )
        return E_POINTER;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        static int uniqueId = 0;
        TCHAR path[MAX_PATH+1];

        int pathlen = GetTempPath(MAX_PATH-24, path);
        if( (0 == pathlen) || (pathlen > (MAX_PATH-24)) )
            return E_FAIL;

        /* check temp directory path by openning it */
        {
            HANDLE dirHandle = CreateFile(path, GENERIC_READ,
                       FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
                       NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
            if( INVALID_HANDLE_VALUE == dirHandle )
            {
                Instance()->setErrorInfo(IID_IVLCVideo,
                        "Invalid temporary directory for snapshot images, check values of TMP, TEMP envars.");
                return E_FAIL;
            }
            else
            {
                BY_HANDLE_FILE_INFORMATION bhfi;
                BOOL res = GetFileInformationByHandle(dirHandle, &bhfi);
                CloseHandle(dirHandle);
                if( !res || !(bhfi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
                {
                    Instance()->setErrorInfo(IID_IVLCVideo,
                            "Invalid temporary directory for snapshot images, check values of TMP, TEMP envars.");
                    return E_FAIL;
                }
            }
        }

        TCHAR filepath[MAX_PATH+1];

        _stprintf(filepath, TEXT("%sAXVLC%lXS%lX.bmp"),
                 path, GetCurrentProcessId(), ++uniqueId);

#ifdef _UNICODE
        /* reuse path storage for UTF8 string */
        char *psz_filepath = (char *)path;
        WCHAR* wpath = filepath;
#else
        char *psz_filepath = path;
        /* first convert to unicode using current code page */
        WCHAR wpath[MAX_PATH+1];
        if( 0 == MultiByteToWideChar(CP_ACP, 0, filepath, -1,
                                     wpath, sizeof(wpath)/sizeof(WCHAR)) )
            return E_FAIL;
#endif
        /* convert to UTF8 */
        pathlen = WideCharToMultiByte(CP_UTF8, 0, wpath, -1,
                                      psz_filepath, sizeof(path), NULL, NULL);
        // fail if path is 0 or too short (i.e pathlen is the same as
        // storage size)

        if( (0 == pathlen) || (sizeof(path) == pathlen) )
            return E_FAIL;

        /* take snapshot into file */
        if( libvlc_video_take_snapshot(p_md, 0, psz_filepath, 0, 0) == 0 )
        {
            /* open snapshot file */
            HANDLE snapPic = LoadImage(NULL, filepath, IMAGE_BITMAP, 0, 0,
                                       LR_CREATEDIBSECTION|LR_LOADFROMFILE);
            if( snapPic )
            {
                PICTDESC snapDesc;

                snapDesc.cbSizeofstruct = sizeof(PICTDESC);
                snapDesc.picType        = PICTYPE_BITMAP;
                snapDesc.bmp.hbitmap    = (HBITMAP)snapPic;
                snapDesc.bmp.hpal       = NULL;

                hr = OleCreatePictureIndirect(&snapDesc, IID_IPictureDisp,
                                              TRUE, (LPVOID*)picture);
                if( FAILED(hr) )
                {
                    *picture = NULL;
                    DeleteObject(snapPic);
                }
            }
            DeleteFile(filepath);
        }
    }
    return hr;
};

STDMETHODIMP VLCVideo::toggleFullscreen()
{
    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        Instance()->toggleFullscreen();
    }
    return hr;
};

STDMETHODIMP VLCVideo::toggleTeletext()
{
    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        libvlc_toggle_teletext(p_md);
    }
    return hr;
};

STDMETHODIMP VLCVideo::get_marquee(IVLCMarquee** obj)
{
    return object_get(obj,_p_vlcmarquee);
}

STDMETHODIMP VLCVideo::get_logo(IVLCLogo** obj)
{
    return object_get(obj,_p_vlclogo);
}

STDMETHODIMP VLCVideo::get_deinterlace(IVLCDeinterlace** obj)
{
    return object_get(obj,_p_vlcdeint);
}

/****************************************************************************/

HRESULT VLCLogo::do_put_int(unsigned idx, LONG val)
{
    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        libvlc_video_set_logo_int(p_md, idx, val);
    }
    return hr;
}

HRESULT VLCLogo::do_get_int(unsigned idx, LONG *val)
{
    if( NULL == val )
        return E_POINTER;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        *val = libvlc_video_get_logo_int(p_md, idx);
    }
    return hr;
}

STDMETHODIMP VLCLogo::file(BSTR fname)
{
    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);

    char *n = CStrFromBSTR(CP_UTF8, fname);
    if( !n ) hr = E_OUTOFMEMORY;

    if( SUCCEEDED(hr) )
    {
        libvlc_video_set_logo_string(p_md, libvlc_logo_file, n);
    }

    CoTaskMemFree(n);
    return hr;
}

STDMETHODIMP VLCLogo::get_position(BSTR* val)
{
    if( NULL == val )
        return E_POINTER;

    LONG i;
    HRESULT hr = do_get_int(libvlc_logo_position, &i);

    if(SUCCEEDED(hr))
        *val = BSTRFromCStr(CP_UTF8, position_bynumber(i));

    return hr;
}

STDMETHODIMP VLCLogo::put_position(BSTR val)
{
    char *n = CStrFromBSTR(CP_UTF8, val);
    if( !n ) return E_OUTOFMEMORY;

    size_t i;
    HRESULT hr;
    if( position_byname( n, i ) )
        hr = do_put_int(libvlc_logo_position,i);
    else
        hr = E_INVALIDARG;

    CoTaskMemFree(n);
    return hr;
}
/****************************************************************************/

STDMETHODIMP VLCMediaDescription::get_meta(BSTR* val, libvlc_meta_t e_meta)
{
    if( NULL == val )
        return E_POINTER;

    *val = 0;

    libvlc_media_player_t *p_md;
    HRESULT hr = getMD(&p_md);
    if( SUCCEEDED(hr) )
    {
        libvlc_media_t * p_media = libvlc_media_player_get_media(p_md);
        const char* info = p_media ? libvlc_media_get_meta(p_media, e_meta) : 0;
        *val = info ? BSTRFromCStr(CP_UTF8, info) : 0;
        hr = *val ? S_OK : E_FAIL;
    }
    return hr;
}

STDMETHODIMP VLCMediaDescription::get_title(BSTR *val)
{
    return get_meta(val, libvlc_meta_Title);
}

STDMETHODIMP VLCMediaDescription::get_artist(BSTR *val)
{
    return get_meta(val, libvlc_meta_Artist);
}
STDMETHODIMP VLCMediaDescription::get_genre(BSTR *val)
{
    return get_meta(val, libvlc_meta_Genre);
}
STDMETHODIMP VLCMediaDescription::get_copyright(BSTR *val)
{
    return get_meta(val, libvlc_meta_Copyright);
}
STDMETHODIMP VLCMediaDescription::get_album(BSTR *val)
{
    return get_meta(val, libvlc_meta_Album);
}
STDMETHODIMP VLCMediaDescription::get_trackNumber(BSTR *val)
{
    return get_meta(val, libvlc_meta_TrackNumber);
}
STDMETHODIMP VLCMediaDescription::get_description(BSTR *val)
{
    return get_meta(val, libvlc_meta_Description);
}
STDMETHODIMP VLCMediaDescription::get_rating(BSTR *val)
{
    return get_meta(val, libvlc_meta_Rating);
}
STDMETHODIMP VLCMediaDescription::get_date(BSTR *val)
{
    return get_meta(val, libvlc_meta_Date);
}
STDMETHODIMP VLCMediaDescription::get_setting(BSTR *val)
{
    return get_meta(val, libvlc_meta_Setting);
}
STDMETHODIMP VLCMediaDescription::get_URL(BSTR *val)
{
    return get_meta(val, libvlc_meta_URL);
}
STDMETHODIMP VLCMediaDescription::get_language(BSTR *val)
{
    return get_meta(val, libvlc_meta_Language);
}
STDMETHODIMP VLCMediaDescription::get_nowPlaying(BSTR *val)
{
    return get_meta(val, libvlc_meta_NowPlaying);
}
STDMETHODIMP VLCMediaDescription::get_publisher(BSTR *val)
{
    return get_meta(val, libvlc_meta_Publisher);
}
STDMETHODIMP VLCMediaDescription::get_encodedBy(BSTR *val)
{
    return get_meta(val, libvlc_meta_EncodedBy);
}
STDMETHODIMP VLCMediaDescription::get_artworkURL(BSTR *val)
{
    return get_meta(val, libvlc_meta_ArtworkURL);
}
STDMETHODIMP VLCMediaDescription::get_trackID(BSTR *val)
{
    return get_meta(val, libvlc_meta_TrackID);
}
/****************************************************************************/

VLCControl2::VLCControl2(VLCPlugin *p_instance) :
    _p_instance(p_instance),
    _p_typeinfo(NULL),
    _p_vlcaudio(new VLCAudio(p_instance)),
    _p_vlcinput(new VLCInput(p_instance)),
    _p_vlcplaylist(new VLCPlaylist(p_instance)),
    _p_vlcsubtitle(new VLCSubtitle(p_instance)),
    _p_vlcvideo(new VLCVideo(p_instance)),
    _p_vlcmedia_desc(p_instance)
{
}

VLCControl2::~VLCControl2()
{
    delete static_cast<VLCVideo*>(_p_vlcvideo);
    delete static_cast<VLCSubtitle*>(_p_vlcsubtitle);
    delete static_cast<VLCPlaylist*>(_p_vlcplaylist);
    delete static_cast<VLCInput*>(_p_vlcinput);
    delete static_cast<VLCAudio*>(_p_vlcaudio);
    if( _p_typeinfo )
        _p_typeinfo->Release();
};

HRESULT VLCControl2::loadTypeInfo(void)
{
    HRESULT hr = NOERROR;
    if( NULL == _p_typeinfo )
    {
        ITypeLib *p_typelib;

        hr = _p_instance->getTypeLib(LOCALE_USER_DEFAULT, &p_typelib);
        if( SUCCEEDED(hr) )
        {
            hr = p_typelib->GetTypeInfoOfGuid(IID_IVLCControl2, &_p_typeinfo);
            if( FAILED(hr) )
            {
                _p_typeinfo = NULL;
            }
            p_typelib->Release();
        }
    }
    return hr;
};

STDMETHODIMP VLCControl2::GetTypeInfoCount(UINT* pctInfo)
{
    if( NULL == pctInfo )
        return E_INVALIDARG;

    if( SUCCEEDED(loadTypeInfo()) )
        *pctInfo = 1;
    else
        *pctInfo = 0;

    return NOERROR;
};

STDMETHODIMP VLCControl2::GetTypeInfo(UINT, LCID, LPTYPEINFO* ppTInfo)
{
    if( NULL == ppTInfo )
        return E_INVALIDARG;

    if( SUCCEEDED(loadTypeInfo()) )
    {
        _p_typeinfo->AddRef();
        *ppTInfo = _p_typeinfo;
        return NOERROR;
    }
    *ppTInfo = NULL;
    return E_NOTIMPL;
};

STDMETHODIMP VLCControl2::GetIDsOfNames(REFIID, LPOLESTR* rgszNames,
        UINT cNames, LCID, DISPID* rgDispID)
{
    if( SUCCEEDED(loadTypeInfo()) )
    {
        return DispGetIDsOfNames(_p_typeinfo, rgszNames, cNames, rgDispID);
    }
    return E_NOTIMPL;
};

STDMETHODIMP VLCControl2::Invoke(DISPID dispIdMember, REFIID,
        LCID, WORD wFlags, DISPPARAMS* pDispParams,
        VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    if( SUCCEEDED(loadTypeInfo()) )
    {
        return DispInvoke(this, _p_typeinfo, dispIdMember, wFlags, pDispParams,
                pVarResult, pExcepInfo, puArgErr);
    }
    return E_NOTIMPL;
};

STDMETHODIMP VLCControl2::get_AutoLoop(VARIANT_BOOL *autoloop)
{
    if( NULL == autoloop )
        return E_POINTER;

    *autoloop = varbool( _p_instance->getAutoLoop() );
    return S_OK;
};

STDMETHODIMP VLCControl2::put_AutoLoop(VARIANT_BOOL autoloop)
{
    _p_instance->setAutoLoop((VARIANT_FALSE != autoloop) ? TRUE: FALSE);
    return S_OK;
};

STDMETHODIMP VLCControl2::get_AutoPlay(VARIANT_BOOL *autoplay)
{
    if( NULL == autoplay )
        return E_POINTER;

    *autoplay = varbool( _p_instance->getAutoPlay() );
    return S_OK;
};

STDMETHODIMP VLCControl2::put_AutoPlay(VARIANT_BOOL autoplay)
{
    _p_instance->setAutoPlay((VARIANT_FALSE != autoplay) ? TRUE: FALSE);
    return S_OK;
};

STDMETHODIMP VLCControl2::get_BaseURL(BSTR *url)
{
    if( NULL == url )
        return E_POINTER;

    *url = SysAllocStringLen(_p_instance->getBaseURL(),
                SysStringLen(_p_instance->getBaseURL()));
    return NOERROR;
};

STDMETHODIMP VLCControl2::put_BaseURL(BSTR mrl)
{
    _p_instance->setBaseURL(mrl);

    return S_OK;
};

STDMETHODIMP VLCControl2::get_MRL(BSTR *mrl)
{
    if( NULL == mrl )
        return E_POINTER;

    *mrl = SysAllocStringLen(_p_instance->getMRL(),
                SysStringLen(_p_instance->getMRL()));
    return NOERROR;
};

STDMETHODIMP VLCControl2::put_MRL(BSTR mrl)
{
    _p_instance->setMRL(mrl);

    return S_OK;
};


STDMETHODIMP VLCControl2::get_Toolbar(VARIANT_BOOL *visible)
{
    if( NULL == visible )
        return E_POINTER;

    *visible = varbool( _p_instance->getShowToolbar() != FALSE );

    return S_OK;
};

STDMETHODIMP VLCControl2::put_Toolbar(VARIANT_BOOL visible)
{
    _p_instance->setShowToolbar((VARIANT_FALSE != visible) ? TRUE: FALSE);
    return S_OK;
};

STDMETHODIMP VLCControl2::get_FullscreenEnabled(VARIANT_BOOL* enabled)
{
    if( NULL == enabled )
        return E_POINTER;

    *enabled = varbool( _p_instance->get_options().get_enable_fs() );

    return S_OK;
}

STDMETHODIMP VLCControl2::put_FullscreenEnabled(VARIANT_BOOL enabled)
{
    _p_instance->get_options().set_enable_fs( VARIANT_FALSE != enabled );
    return S_OK;
}

STDMETHODIMP VLCControl2::get_StartTime(long *seconds)
{
    if( NULL == seconds )
        return E_POINTER;

    *seconds = _p_instance->getStartTime();
    return S_OK;
};

STDMETHODIMP VLCControl2::put_StartTime(long seconds)
{
    _p_instance->setStartTime(seconds);
    return S_OK;
};

STDMETHODIMP VLCControl2::get_VersionInfo(BSTR *version)
{
    if( NULL == version )
        return E_POINTER;

    const char *versionStr = libvlc_get_version();
    if( NULL != versionStr )
    {
        *version = BSTRFromCStr(CP_UTF8, versionStr);

        return (NULL == *version) ? E_OUTOFMEMORY : NOERROR;
    }
    *version = NULL;
    return E_FAIL;
};

STDMETHODIMP VLCControl2::versionInfo(BSTR *version)
{
    if( NULL == version )
        return E_POINTER;

    const char *versionStr = libvlc_get_version();
    if( NULL != versionStr )
    {
        *version = BSTRFromCStr(CP_UTF8, versionStr);

        return (NULL == *version) ? E_OUTOFMEMORY : NOERROR;
    }
    *version = NULL;
    return E_FAIL;
};

STDMETHODIMP VLCControl2::get_Visible(VARIANT_BOOL *isVisible)
{
    if( NULL == isVisible )
        return E_POINTER;

    *isVisible = varbool( _p_instance->getVisible() );

    return S_OK;
};

STDMETHODIMP VLCControl2::put_Visible(VARIANT_BOOL isVisible)
{
    _p_instance->setVisible(isVisible != VARIANT_FALSE);
    return S_OK;
};

STDMETHODIMP VLCControl2::get_Volume(long *volume)
{
    if( NULL == volume )
        return E_POINTER;

    *volume  = _p_instance->getVolume();
    return S_OK;
};

STDMETHODIMP VLCControl2::put_Volume(long volume)
{
    _p_instance->setVolume(volume);
    return S_OK;
};

STDMETHODIMP VLCControl2::get_BackColor(OLE_COLOR *backcolor)
{
    if( NULL == backcolor )
        return E_POINTER;

    *backcolor  = _p_instance->getBackColor();
    return S_OK;
};

STDMETHODIMP VLCControl2::put_BackColor(OLE_COLOR backcolor)
{
    _p_instance->setBackColor(backcolor);
    return S_OK;
};

STDMETHODIMP VLCControl2::get_audio(IVLCAudio** obj)
{
    return object_get(obj,_p_vlcaudio);
}

STDMETHODIMP VLCControl2::get_input(IVLCInput** obj)
{
    return object_get(obj,_p_vlcinput);
}

STDMETHODIMP VLCControl2::get_playlist(IVLCPlaylist** obj)
{
    return object_get(obj,_p_vlcplaylist);
}

STDMETHODIMP VLCControl2::get_subtitle(IVLCSubtitle** obj)
{
    return object_get(obj,_p_vlcsubtitle);
}

STDMETHODIMP VLCControl2::get_video(IVLCVideo** obj)
{
    return object_get(obj,_p_vlcvideo);
}

STDMETHODIMP VLCControl2::get_mediaDescription(IVLCMediaDescription** obj)
{
    return object_get<IVLCMediaDescription>(obj, &_p_vlcmedia_desc);
}
