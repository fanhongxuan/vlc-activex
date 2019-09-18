/*****************************************************************************
 * vlc_win32_fullscreen.h: a VLC plugin for Mozilla
 *****************************************************************************
 * Copyright 2002-2011 VideoLAN and VLC authors
 * $Id$
 *
 * Authors: Sergey Radionov <rsatom@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <windows.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <windowsx.h>
#include "win32_fullscreen.h"
#include <time.h> // for time
#include "msg.h"
#include "metadata.h"
////////////////////////////////////////////////////////////////////////////////
//VLCControlsWnd members
////////////////////////////////////////////////////////////////////////////////
VLCControlsWnd*
VLCControlsWnd::CreateControlsWindow(HINSTANCE hInstance,
                                     VLCWindowsManager* wm, HWND hWndParent)
{
    VLCControlsWnd* wnd = new VLCControlsWnd(hInstance, wm);
    if( wnd && wnd->Create(hWndParent) ) {
        return wnd;
    }
    delete wnd;
    return 0;
}

VLCControlsWnd::VLCControlsWnd(HINSTANCE hInstance, VLCWindowsManager* wm)
    :VLCWnd(hInstance), _wm(wm),
     hToolTipWnd(0), hFSButton(0), hPlayPauseButton(0),
     hVideoPosScroll(0), hMuteButton(0), hVolumeSlider(0)
{
}

VLCControlsWnd::~VLCControlsWnd()
{
    if(hToolTipWnd){
        ::DestroyWindow(hToolTipWnd);
        hToolTipWnd = 0;
    }
}

bool VLCControlsWnd::Create(HWND hWndParent)
{
    return VLCWnd::CreateEx(WS_EX_TOPMOST, TEXT("VLC Controls Window"),
                            WS_CHILD|WS_CLIPSIBLINGS,
                            0, 0, 0, 0, hWndParent, 0);
}

void VLCControlsWnd::PreRegisterWindowClass(WNDCLASS* wc)
{
    wc->lpszClassName = TEXT("VLC Controls Class");
    wc->hbrBackground = (HBRUSH)(COLOR_3DFACE+1);
};

void VLCControlsWnd::CreateToolTip()
{
    hToolTipWnd = CreateWindowEx(WS_EX_TOPMOST,
            TOOLTIPS_CLASS,
            NULL,
            WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            hWnd(),
            NULL,
            hInstance(),
            NULL);

    SetWindowPos(hToolTipWnd,
            HWND_TOPMOST,
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);


    TOOLINFO ti;
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS|TTF_IDISHWND;
    ti.hwnd   = hWnd();
    ti.hinst  = hInstance();

    TCHAR HintText[100];
    RECT ActivateTTRect;

    //end fullscreen button tooltip
    GetWindowRect(hFSButton, &ActivateTTRect);
    GetWindowText(hFSButton, HintText, sizeof(HintText));
    ti.uId = (UINT_PTR)hFSButton;
    ti.rect = ActivateTTRect;
    ti.lpszText = HintText;
    SendMessage(hToolTipWnd, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

    //play/pause button tooltip
    GetWindowRect(hPlayPauseButton, &ActivateTTRect);
    GetWindowText(hPlayPauseButton, HintText, sizeof(HintText));
    ti.uId = (UINT_PTR)hPlayPauseButton;
    ti.rect = ActivateTTRect;
    ti.lpszText = HintText;
    SendMessage(hToolTipWnd, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

    //mute button tooltip
    GetWindowRect(hMuteButton, &ActivateTTRect);
    GetWindowText(hMuteButton, HintText, sizeof(HintText));
    ti.uId = (UINT_PTR)hMuteButton;
    ti.rect = ActivateTTRect;
    ti.lpszText = HintText;
    SendMessage(hToolTipWnd, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
}

LRESULT VLCControlsWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg){
        case WM_CREATE:{
            const int ControlsHeight = 21+3;
            const int ButtonsWidth = ControlsHeight;

            int HorizontalOffset = xControlsSpace;
            int ControlWidth = ButtonsWidth;
            hPlayPauseButton =
                CreateWindow(TEXT("BUTTON"), TEXT("Play/Pause"),
                             WS_CHILD|WS_VISIBLE|BS_BITMAP|BS_FLAT,
                             HorizontalOffset, xControlsSpace,
                             ControlWidth, ControlsHeight, hWnd(),
                             (HMENU)ID_FS_PLAY_PAUSE, 0, 0);
            SendMessage(hPlayPauseButton, BM_SETIMAGE,
                        (WPARAM)IMAGE_BITMAP, (LPARAM)RC().hPauseBitmap);
            HorizontalOffset+=ControlWidth+xControlsSpace;

            ControlWidth = 200;
            int VideoPosControlHeight = 10;
            hVideoPosScroll =
                CreateWindow(PROGRESS_CLASS, TEXT("Video Position"),
                             WS_CHILD|WS_DISABLED|WS_VISIBLE|SBS_HORZ|SBS_TOPALIGN|PBS_SMOOTH,
                             HorizontalOffset, xControlsSpace+(ControlsHeight-VideoPosControlHeight)/2,
                             ControlWidth, VideoPosControlHeight, hWnd(),
                             (HMENU)ID_FS_VIDEO_POS_SCROLL, 0, 0);
            HMODULE hThModule = LoadLibrary(TEXT("UxTheme.dll"));
            if(hThModule){
                FARPROC proc = GetProcAddress(hThModule, "SetWindowTheme");
                typedef HRESULT (WINAPI* SetWindowThemeProc)(HWND, LPCWSTR, LPCWSTR);
                if(proc){
                    ((SetWindowThemeProc)proc)(hVideoPosScroll, L"", L"");
                }
                FreeLibrary(hThModule);
            }
            HorizontalOffset+=ControlWidth+xControlsSpace;

            ControlWidth = ButtonsWidth;
            hMuteButton =
                CreateWindow(TEXT("BUTTON"), TEXT("Mute"),
                             WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|BS_PUSHLIKE|BS_BITMAP, //BS_FLAT
                             HorizontalOffset, xControlsSpace,
                             ControlWidth, ControlsHeight,
                             hWnd(), (HMENU)ID_FS_MUTE, 0, 0);
            SendMessage(hMuteButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP,
                        (LPARAM)RC().hVolumeBitmap);
            HorizontalOffset+=ControlWidth+xControlsSpace;

            ControlWidth = 100;
            hVolumeSlider =
                CreateWindow(TRACKBAR_CLASS, TEXT("Volume"),
                             WS_CHILD|WS_VISIBLE|TBS_HORZ|TBS_BOTTOM|TBS_AUTOTICKS,
                             HorizontalOffset, xControlsSpace,
                             ControlWidth, ControlsHeight - 4, hWnd(),
                             (HMENU)ID_FS_VOLUME, 0, 0);
            HorizontalOffset+=ControlWidth+xControlsSpace;
            SendMessage(hVolumeSlider, TBM_SETRANGE, FALSE, (LPARAM) MAKELONG (0, 100));
            SendMessage(hVolumeSlider, TBM_SETTICFREQ, (WPARAM) 10, 0);

            ControlWidth = ButtonsWidth;
            DWORD dwFSBtnStyle = WS_CHILD|BS_BITMAP|BS_FLAT;
            if( !PO() || PO()->get_enable_fs() ){
                dwFSBtnStyle |= WS_VISIBLE;
            }
            hFSButton =
                CreateWindow(TEXT("BUTTON"), TEXT("Toggle fullscreen"),
                             dwFSBtnStyle,
                             HorizontalOffset, xControlsSpace,
                             ControlWidth, ControlsHeight, hWnd(),
                             (HMENU)ID_FS_SWITCH_FS, 0, 0);
            SendMessage(hFSButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP,
                        (LPARAM)RC().hDeFullscreenBitmap);
            HorizontalOffset+=ControlWidth+xControlsSpace;

            RECT rect;
            GetClientRect(GetParent(hWnd()), &rect);

            int ControlWndWidth = HorizontalOffset;
            int ControlWndHeight = xControlsSpace+ControlsHeight+xControlsSpace;
            SetWindowPos(hWnd(), 0,
                         0, (rect.bottom - rect.top) - ControlWndWidth,
                         rect.right-rect.left, ControlWndHeight,
                         SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE);

            //new message blinking timer
            SetTimer(hWnd(), 2, 500, NULL);

            CreateToolTip();

            break;
        }
        case WM_SHOWWINDOW:{
            if(FALSE!=wParam){ //showing
                UpdateButtons();
            }
            break;
        }
        case WM_LBUTTONUP:{
            POINT BtnUpPoint = {LOWORD(lParam), HIWORD(lParam)};
            RECT VideoPosRect;
            GetWindowRect(hVideoPosScroll, &VideoPosRect);
            ClientToScreen(hWnd(), &BtnUpPoint);
            if(PtInRect(&VideoPosRect, BtnUpPoint)){
                SetVideoPos(float(BtnUpPoint.x-VideoPosRect.left)/(VideoPosRect.right-VideoPosRect.left));
            }
            break;
        }
        case WM_TIMER:{
            switch(wParam){
                case 1:{
                    POINT MousePoint;
                    GetCursorPos(&MousePoint);
                    RECT ControlWndRect;
                    GetWindowRect(hWnd(), &ControlWndRect);
                    if(PtInRect(&ControlWndRect, MousePoint)||GetCapture()==hVolumeSlider){
                        //do not allow control window to close while mouse is within
                        NeedShowControls();
                    }
                    else{
                        NeedHideControls();
                    }
                    break;
                }
                case 2:{
                    UpdateButtons();
                    break;
                }
            }
            break;
        }
        case WM_SETCURSOR:{
            RECT VideoPosRect;
            GetWindowRect(hVideoPosScroll, &VideoPosRect);
            DWORD dwMsgPos = GetMessagePos();
            POINT MsgPosPoint = {LOWORD(dwMsgPos), HIWORD(dwMsgPos)};
            if(PtInRect(&VideoPosRect, MsgPosPoint)){
                SetCursor(LoadCursor(NULL, IDC_HAND));
                return TRUE;
            }
            else{
                return VLCWnd::WindowProc(uMsg, wParam, lParam);
            }
            break;
        }
        case WM_NCDESTROY:
            break;
        case WM_COMMAND:{
            WORD NCode = HIWORD(wParam);
            WORD Control = LOWORD(wParam);
            switch(NCode){
                case BN_CLICKED:{
                    switch(Control){
                        case ID_FS_SWITCH_FS:
                            WM().ToggleFullScreen();
                            break;
                        case ID_FS_PLAY_PAUSE:{
                            if( VP() ){
                                if( IsPlaying() )
                                    VP()->pause();
                                else
                                    VP()->play();
                            }
                            break;
                        }
                        case ID_FS_MUTE:{
                            if( VP() ){
                                VP()->set_mute( IsDlgButtonChecked(hWnd(), ID_FS_MUTE) != FALSE );
                                SyncVolumeSliderWithVLCVolume();
                            }
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }
        case WM_SIZE:{
            if( GetWindowLong(hWnd(), GWL_STYLE) & WS_VISIBLE &&
                !WM().IsFullScreen() &&
                ( !PO() || !PO()->get_show_toolbar() ) )
            {
                //hide controls when they are not allowed
                NeedHideControls();
            }

            const int new_client_width = LOWORD(lParam);

            bool isFSBtnVisible =
                (GetWindowLong(hFSButton, GWL_STYLE) & WS_VISIBLE) != 0;

            HDWP hDwp = BeginDeferWindowPos(4);

            int VideoScrollWidth = new_client_width;

            POINT pt = {0, 0};
            RECT rect;
            GetWindowRect(hPlayPauseButton, &rect);
            pt.x = rect.right;
            ScreenToClient(hWnd(), &pt);
            VideoScrollWidth -= pt.x;
            VideoScrollWidth -= xControlsSpace;

            RECT VideoSrcollRect;
            GetWindowRect(hVideoPosScroll, &VideoSrcollRect);

            RECT MuteRect;
            GetWindowRect(hMuteButton, &MuteRect);
            VideoScrollWidth -= xControlsSpace;
            VideoScrollWidth -= (MuteRect.right - MuteRect.left);

            RECT VolumeRect;
            GetWindowRect(hVolumeSlider, &VolumeRect);
            VideoScrollWidth -= xControlsSpace;
            VideoScrollWidth -= (VolumeRect.right - VolumeRect.left);

            RECT FSRect = {0, 0, 0, 0};
            if( isFSBtnVisible ) {
                GetWindowRect(hFSButton, &FSRect);
                VideoScrollWidth -= xControlsSpace;
                VideoScrollWidth -= (FSRect.right - FSRect.left);
                VideoScrollWidth -= xControlsSpace;
            }

            pt.x = VideoSrcollRect.left;
            pt.y = VideoSrcollRect.top;
            ScreenToClient(hWnd(), &pt);
            hDwp = DeferWindowPos(hDwp, hVideoPosScroll, 0, pt.x, pt.y,
                                  VideoScrollWidth,
                                  VideoSrcollRect.bottom - VideoSrcollRect.top,
                                  SWP_NOACTIVATE|SWP_NOOWNERZORDER);

            int HorizontalOffset =
                pt.x + VideoScrollWidth + xControlsSpace;
            pt.x = 0;
            pt.y = MuteRect.top;
            ScreenToClient(hWnd(), &pt);
            hDwp = DeferWindowPos(hDwp, hMuteButton, 0,
                                  HorizontalOffset, pt.y, 0, 0,
                                  SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
            HorizontalOffset +=
                MuteRect.right - MuteRect.left + xControlsSpace;

            pt.x = 0;
            pt.y = VolumeRect.top;
            ScreenToClient(hWnd(), &pt);
            hDwp = DeferWindowPos(hDwp, hVolumeSlider, 0,
                                  HorizontalOffset, pt.y, 0, 0,
                                  SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
            HorizontalOffset +=
                VolumeRect.right - VolumeRect.left + xControlsSpace;

            if( isFSBtnVisible ) {
                pt.x = 0;
                pt.y = FSRect.top;
                ScreenToClient(hWnd(), &pt);
                hDwp = DeferWindowPos(hDwp, hFSButton, 0,
                                      HorizontalOffset, pt.y, 0, 0,
                                      SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
            }

            EndDeferWindowPos(hDwp);
            break;
        }
        case WM_HSCROLL:
        case WM_VSCROLL: {
            if( VP() ){
                if(hVolumeSlider==(HWND)lParam){
                    LRESULT SliderPos = SendMessage(hVolumeSlider, (UINT) TBM_GETPOS, 0, 0);
                    SetVLCVolumeBySliderPos(SliderPos);
                }
            }
            break;
        }
        default:
            return VLCWnd::WindowProc(uMsg, wParam, lParam);
    }
    return 0L;
}

void VLCControlsWnd::UpdateButtons()
{
    if(hVideoPosScroll){
        SetVideoPosScrollRangeByVideoLen();
        SyncVideoPosScrollPosWithVideoPos();
    }

    if(hVolumeSlider){
        SyncVolumeSliderWithVLCVolume();
    }

    if(hFSButton) {
        HANDLE hFSBitmap =
            WM().IsFullScreen() ? RC().hDeFullscreenBitmap :
                                  RC().hFullscreenBitmap;

        HANDLE hBitmap =
            (HANDLE)SendMessage(hFSButton, BM_GETIMAGE, (WPARAM)IMAGE_BITMAP, 0);

        if( WM().getNewMessageFlag() &&
           (hBitmap == RC().hDeFullscreenBitmap ||
            hBitmap == RC().hFullscreenBitmap) )
        {
            SendMessage(hFSButton, BM_SETIMAGE,
                        (WPARAM)IMAGE_BITMAP,
                        (LPARAM)RC().hNewMessageBitmap);
            //do not allow control window to close while there are new messages
            NeedShowControls();
        }
        else{
            if(hBitmap != hFSBitmap)
                SendMessage(hFSButton, BM_SETIMAGE,
                            (WPARAM)IMAGE_BITMAP,
                            (LPARAM)hFSBitmap);
        }
    }

    if(hPlayPauseButton){
        HANDLE hBmp = IsPlaying() ? RC().hPauseBitmap : RC().hPlayBitmap;
        SendMessage(hPlayPauseButton, BM_SETIMAGE,
                    (WPARAM)IMAGE_BITMAP, (LPARAM)hBmp);
    }

}

void VLCControlsWnd::NeedShowControls()
{
    if( !(GetWindowLong(hWnd(), GWL_STYLE) & WS_VISIBLE) ) {
        if(WM().IsFullScreen() || (PO() && PO()->get_show_toolbar() ) )
            ShowWindow( hWnd(), SW_SHOW );
    }
    //hide controls after 2 seconds
    SetTimer(hWnd(), 1, 2*1000, NULL);
}

void VLCControlsWnd::NeedHideControls()
{
    KillTimer( hWnd(), 1 );
    ShowWindow( hWnd(), SW_HIDE );
}

void VLCControlsWnd::SyncVideoPosScrollPosWithVideoPos()
{
    if( VP() ){
        libvlc_time_t pos = VP()->get_time();
        SetVideoPosScrollPosByVideoPos(pos);
    }
}

void VLCControlsWnd::SetVideoPosScrollRangeByVideoLen()
{
    if( VP() ){
        libvlc_time_t MaxLen = VP()->get_length();
        VideoPosShiftBits = 0;
        while(MaxLen>0xffff){
            MaxLen >>= 1;
            ++VideoPosShiftBits;
        };
        SendMessage(hVideoPosScroll, (UINT)PBM_SETRANGE, 0, MAKELPARAM(0, MaxLen));
    }
}

void VLCControlsWnd::SetVideoPosScrollPosByVideoPos(libvlc_time_t CurScrollPos)
{
    PostMessage(hVideoPosScroll, (UINT)PBM_SETPOS, (WPARAM) (CurScrollPos >> VideoPosShiftBits), 0);
}

void VLCControlsWnd::SetVideoPos(float Pos) //0-start, 1-end
{
    if( VP() ){
        vlc_player& vp = *VP();
        vp.set_time( static_cast<libvlc_time_t>( vp.get_length()*Pos ) );
        SyncVideoPosScrollPosWithVideoPos();
    }
}

void VLCControlsWnd::SyncVolumeSliderWithVLCVolume()
{
    if( VP() ){
        vlc_player& vp = *VP();
        unsigned int vol = vp.get_volume();
        const LRESULT SliderPos = SendMessage(hVolumeSlider, (UINT) TBM_GETPOS, 0, 0);
        if((UINT)SliderPos!=vol)
            SendMessage(hVolumeSlider, (UINT) TBM_SETPOS, (WPARAM) TRUE, (LPARAM) vol);

        bool muted = vp.is_muted();
        int MuteButtonState = SendMessage(hMuteButton, (UINT) BM_GETCHECK, 0, 0);
        if((muted&&(BST_UNCHECKED==MuteButtonState))||(!muted&&(BST_CHECKED==MuteButtonState))){
            SendMessage(hMuteButton, BM_SETCHECK, (WPARAM)(muted?BST_CHECKED:BST_UNCHECKED), 0);
        }
        LRESULT lResult = SendMessage(hMuteButton, BM_GETIMAGE, (WPARAM)IMAGE_BITMAP, 0);
        if( (muted && ((HANDLE)lResult == RC().hVolumeBitmap)) ||
            (!muted&&((HANDLE)lResult == RC().hVolumeMutedBitmap)) )
        {
            HANDLE hBmp = muted ? RC().hVolumeMutedBitmap : RC().hVolumeBitmap ;
            SendMessage(hMuteButton, BM_SETIMAGE,
                        (WPARAM)IMAGE_BITMAP, (LPARAM)hBmp);
        }
    }
}

void VLCControlsWnd::SetVLCVolumeBySliderPos(int CurPos)
{
    if( VP() ){
        vlc_player& vp = *VP();
        vp.set_volume(CurPos);
        if(0==CurPos){
            vp.set_mute( IsDlgButtonChecked( hWnd(), ID_FS_MUTE) != FALSE );
        }
        SyncVolumeSliderWithVLCVolume();
    }
}


/////////////////////////////////////
//VLCControlsWnd event handlers

void VLCControlsWnd::handle_position_changed_event(const libvlc_event_t* )
{
    SyncVideoPosScrollPosWithVideoPos();
}

void VLCControlsWnd::handle_input_state_event(const libvlc_event_t* event)
{
    switch( event->type )
    {
        case libvlc_MediaPlayerPlaying:
            PostMessage(hPlayPauseButton, BM_SETIMAGE,
                        (WPARAM)IMAGE_BITMAP, (LPARAM)RC().hPauseBitmap);
            break;
        case libvlc_MediaPlayerPaused:
            PostMessage(hPlayPauseButton, BM_SETIMAGE,
                        (WPARAM)IMAGE_BITMAP, (LPARAM)RC().hPlayBitmap);
            break;
        case libvlc_MediaPlayerStopped:
            PostMessage(hPlayPauseButton, BM_SETIMAGE,
                        (WPARAM)IMAGE_BITMAP, (LPARAM)RC().hPlayBitmap);
            break;
    }
}

//libvlc events arrives from separate thread
void VLCControlsWnd::OnLibVlcEvent(const libvlc_event_t* event)
{
    switch(event->type){
        case libvlc_MediaPlayerPlaying:
        case libvlc_MediaPlayerPaused:
        case libvlc_MediaPlayerStopped:
            handle_input_state_event(event);
            break;
        case libvlc_MediaPlayerPositionChanged:
            handle_position_changed_event(event);
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//VLCHolderWnd members
////////////////////////////////////////////////////////////////////////////////
enum{
    WM_TRY_SET_MOUSE_HOOK = WM_USER+1,
    WM_MOUSE_EVENT_NOTIFY = WM_APP+1,
    WM_MOUSE_EVENT_NOTIFY_SUCCESS = 0xFF
};

VLCHolderWnd*
VLCHolderWnd::CreateHolderWindow(HINSTANCE hInstance,
                                 HWND hParentWnd, VLCWindowsManager* WM)
{
    VLCHolderWnd* wnd = new VLCHolderWnd(hInstance, WM);
    if( wnd && wnd->Create(hParentWnd) ) {
        return wnd;
    }
    delete wnd;
    return 0;
}

VLCHolderWnd::~VLCHolderWnd()
{
    if(_hBgBrush) {
        DeleteObject(_hBgBrush);
        _hBgBrush = 0;
    }
	StopMsgServer();
}

bool VLCHolderWnd::Create(HWND hWndParent)
{
    return VLCWnd::Create(TEXT("Holder Window"),
                            WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VISIBLE,
                            0, 0, 0, 0, hWndParent, 0);
}

void VLCHolderWnd::PreRegisterWindowClass(WNDCLASS* wc)
{
    if( !_hBgBrush){
        unsigned r = 0, g = 0, b = 0;
        HTMLColor2RGB(PO()->get_bg_color().c_str(), &r, &g, &b);
        _hBgBrush = CreateSolidBrush(RGB(r, g, b));
    }

    wc->hbrBackground = _hBgBrush;
    wc->lpszClassName = TEXT("Web Plugin VLC Window Holder Class");
}

vaRect::vaRect()
{
	mTTL = -1;
	mTimeStamp = 0;
	mbRect = false;
	mbFinish = false;
	memset(&mCaptionRect, 0, sizeof(mCaptionRect));
	mHwnd = NULL;
}

vaRect::~vaRect()
{
	Reset();
}

vaRect::vaRect(const vaRect &other)
{
	int i = 0;
	for (i = 0; i < other.mPointList.size(); i++){
		AddPoint(*other.mPointList[i]);
	}
	mCaptionRect = other.mCaptionRect;
	mbRect = other.mbRect;
	mbFinish = other.mbFinish;
	mHwnd = other.mHwnd;
	mTTL = other.mTTL;
	mTimeStamp = other.mTimeStamp;
}

bool vaRect::Reset()
{
	int i = 0;
	for (i = 0; i < mPointList.size(); i++){
		delete mPointList[i];
	}
	mPointList.clear();
	memset(&mCaptionRect, 0, sizeof(mCaptionRect));
	mbRect = false;
	mbFinish = false;
	mTimeStamp = 0;
	mTTL = -1;
	mHwnd = NULL;
	return true;
}

bool vaRect::IsOnCaption(int x, int y)
{
	if (!IsFinish()){
		return false;
	}
	if (mPointList.size() < 3){
		return false;
	}
	if (x >= mCaptionRect.left && x <= mCaptionRect.right &&
		y >= mCaptionRect.top && y <= mCaptionRect.bottom){
			return true;
	}
	return false;
}

bool vaRect::IsClosed()
{
	if (mPointList.size() < 3){
		return false;
	}
	
	vaPoint *pStart = mPointList[0];
	vaPoint *pStop = mPointList[mPointList.size() - 1];
	if (NULL != pStart && NULL != pStop){
		if (pStart->IsSameAs(*pStop)){
			// Update the mCaptionRect here
			return true;
		}
	}
	return false;
}

#define CAPTION_WIDTH 10
#define CAPTION_HEIGHT 10
void vaRect::AddPoint(const vaPoint &point)
{
	vaPoint *pPoint = new vaPoint(point);
	mPointList.push_back(pPoint);
}

void vaRect::UpdateCaptionRect()
{
	RECT clientRect;
	memset(&clientRect, 0, sizeof(clientRect));
	GetClientRect(mHwnd, &clientRect);
	mCaptionRect.left = mPointList[0]->x;
	mCaptionRect.right =  mPointList[0]->x + CAPTION_WIDTH;
	mCaptionRect.bottom = mPointList[0]->y;
	mCaptionRect.top = mPointList[0]->y - CAPTION_HEIGHT;
	if (mCaptionRect.top < clientRect.top){
		mCaptionRect.top = clientRect.top;
		mCaptionRect.bottom = clientRect.top + CAPTION_HEIGHT;
	}
	if (mCaptionRect.right > clientRect.right){
		mCaptionRect.right = clientRect.right;
		mCaptionRect.left = clientRect.right - CAPTION_WIDTH;
	}
}


void vaRect::Draw(HDC dc)
{
	int j = 0;
	if (mPointList.empty()){
		return;
	}
	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255,0, 0));
	HGDIOBJ  hOldPen = SelectObject(dc, hPen);
	for (j = 0; j < mPointList.size(); j++){
		vaPoint *pBegin  = mPointList[j];
		vaPoint *pEnd    = NULL;
		if ((j+1) < mPointList.size()){
			pEnd = mPointList[j+1];
		}
		else if (mPointList.size() == 1){
			pEnd = mPointList[0];
		}
		if (NULL != pBegin && NULL != pEnd){
			MoveToEx(dc, pBegin->x, pBegin->y, NULL);
			LineTo(dc, pEnd->x, pEnd->y);
		}
	}
	HPEN hStartPointPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
	SelectObject(dc, hStartPointPen);
	MoveToEx(dc, mPointList[0]->x, mPointList[0]->y, NULL);
	LineTo(dc, mPointList[0]->x, mPointList[0]->y);

	if (IsFinish()){
		// draw the close catpion
		UpdateCaptionRect();

		HPEN hCaption = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
		SelectObject(dc, hCaption);
		MoveToEx(dc, mCaptionRect.left, mCaptionRect.top, NULL);
		LineTo(dc, mCaptionRect.right, mCaptionRect.bottom);
		MoveToEx(dc, mCaptionRect.right, mCaptionRect.top, NULL);
		LineTo(dc, mCaptionRect.left, mCaptionRect.bottom);
		SelectObject(dc, hPen);
		DeleteObject(hCaption);
	}

	SelectObject(dc, hOldPen);

	DeleteObject(hStartPointPen);
	DeleteObject(hPen);
}

void vaRect::DelLastPoint()
{
	if (IsFinish()){
		return;
	}

	if (mPointList.empty()){
		return;
	}
	if (mPointList.size() == 1){
		Reset();
	}
	else{
		delete mPointList[mPointList.size() - 1];
		mPointList.resize(mPointList.size() - 1);
	}
}

bool vaRect::SetRectRightBottom(int x, int y)
{
	if (mPointList.empty()){
		return false;
	}
	if (mPointList.size() == 1 && mbRect == false){
		// make sure the distance is enough
		vaPoint cur = vaPoint();
		cur.x = x; cur.y = y;
		if (mPointList[0]->IsSameAs(cur)){
			return false;
		}
		mbRect = true;
		vaPoint  *pt= new vaPoint;
		pt->x = mPointList[0]->x;
		pt->y = y;
		mPointList.push_back(pt); // left bottom
		pt = new vaPoint;
		pt->x = x;
		pt->y = y;  
		mPointList.push_back(pt); // right bottom
		pt = new vaPoint;
		pt->x = x;
		pt->y = mPointList[0]->y;
		mPointList.push_back(pt); // right top
		pt = new vaPoint;
		pt->x = mPointList[0]->x;
		pt->y = mPointList[0]->y;
		mPointList.push_back(pt);
		return true;
	}
	else if (mPointList.size() == 5 && mbRect == true){
		mPointList[1]->y = y; // left bottom
		mPointList[2]->x = x;
		mPointList[2]->y = y;
		mPointList[3]->x = x;
		return true;
	}
	return false;
}

static VLCHolderWnd *theHolderWnd = NULL;
static u32 msg_handleMetadataReq(void *pBuffer)
{
	if (NULL != theHolderWnd){
		return theHolderWnd->HandleMetadataReq(pBuffer);
	}
	return 0;
}

unsigned int VLCHolderWnd::HandleMetadataReq(void *pBuffer){
	ctrl_metadata_req_t *pReq = (ctrl_metadata_req_t *)pBuffer;
	if (pReq->Type == ctrl_metadata_add_obj){
		if (pReq->Data.AddObj.Obj.Type == ctrl_metadata_object_type_text){
		}
		else if (pReq->Data.AddObj.Obj.Type == ctrl_metadata_object_type_rect){
			vaRect *pRect = new vaRect();
			vaPoint start;
			start.x = pReq->Data.AddObj.Obj.Data.Rect.TopLeft.X;
			start.y = pReq->Data.AddObj.Obj.Data.Rect.TopLeft.Y;
			pRect->AddPoint(start);
			pRect->SetRectRightBottom(pReq->Data.AddObj.Obj.Data.Rect.BottomRight.X, pReq->Data.AddObj.Obj.Data.Rect.BottomRight.Y);
			pRect->SetFinish(true);
			pRect->SetHwnd(hWnd());
			pRect->SetTTL(pReq->Data.AddObj.Obj.TTL);
			if (pReq->Data.AddObj.Obj.Timestamp != 0){
				pRect->SetTimeStamp(pReq->Data.AddObj.Obj.Timestamp);
			}
			else{
				pRect->SetTimeStamp((int)time(NULL));
			}
			mRects.push_back(pRect);
		}
		else if (pReq->Data.AddObj.Obj.Type == ctrl_metadata_object_type_comp){
		}
		else if (pReq->Data.AddObj.Obj.Type == ctrl_metadata_object_type_cycle){
		}
		else{
			// unknown type
		}
	}
	else if (pReq->Type == ctrl_metadata_del_obj){

	}
	InvalidateRect(hWnd(), NULL, false);
	return 0;
}

void VLCHolderWnd::StartMsgServer()
{
	if (mbMsgServerStarted){
		return;
	}
    if (VP() == NULL){
        return;
    }
    const char *pServerIp = VP()->get_msgq_addr();
    const char *pServerPort = VP()->get_msgq_port();
    if (NULL == pServerIp || NULL == pServerPort){
        return;
    }
    
    msg_setcfg(MSG_SERVERIP, pServerIp);
	msg_setcfg(MSG_PORT, pServerPort);
	msg_register_serial(ctrl_metadata_req, metadata_h2n, metadata_n2h);
	if (msg_register(ctrl_metadata_req, msg_handleMetadataReq) == 0){
		mbMsgServerStarted = true;
		if (theHolderWnd != this){
			theHolderWnd = this;
		}
	}
}

void VLCHolderWnd::StopMsgServer()
{
	if (theHolderWnd == this){
		theHolderWnd = NULL;
	}
	msg_unregister(ctrl_metadata_req);
}

void VLCHolderWnd::onPaint(HDC hDC)
{
    HDC hMemory = CreateCompatibleDC(hDC);
    if (NULL == hMemory){
        return;
    }
    StartMsgServer();
    BITMAPINFO info;
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = mWidth;
    info.bmiHeader.biHeight = - mHeight;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    info.bmiHeader.biSizeImage = 0;
    info.bmiHeader.biXPelsPerMeter = 3000;
    info.bmiHeader.biYPelsPerMeter = 3000;
    info.bmiHeader.biClrUsed = 0;
    info.bmiHeader.biClrImportant = 0;
    HBITMAP hBitmap = CreateCompatibleBitmap(hDC, mWidth, mHeight);
    SelectObject(hMemory, hBitmap);
    SetDIBits(hMemory, hBitmap, 0, mHeight, mpBuffer, &info, DIB_RGB_COLORS);

	std::list<vaRect*>::iterator it = mRects.begin();
	while(it != mRects.end()){
		vaRect *pRect = *it;
		if (NULL != pRect){
			// check the timestamp and ttl
			if (pRect->GetTimeStamp() != 0 && pRect->GetTTL() != 0){
				int now = (int)time(NULL);
				int timeout = pRect->GetTimeStamp() + pRect->GetTTL();
				if (now >= timeout){
					std::list<vaRect*>::iterator del = it;
					it++;
					mRects.erase(del);
					continue;
				}
			}
			pRect->Draw(hMemory);
		}
		it++;
	}
	mCurRect.Draw(hMemory);
#if 0
    HPEN hPen = CreatePen(PS_SOLID, 5, RGB(255,0,0));
    SelectObject(hMemory, hPen);
    for (int i = 0; i < mLines.size(); i++){
        vaLine *pLine = mLines[i];
        if (NULL != pLine){
            MoveToEx(hMemory, pLine->startX, pLine->startY, NULL);
            LineTo(hMemory, pLine->endX, pLine->endY);
        }
    }
#endif
    BitBlt(hDC, 0, 0, mWidth, mHeight, hMemory, 0, 0, SRCCOPY);

    DeleteDC(hMemory);
    
}

void VLCHolderWnd::ClearAllLines()
{
    for (int i = 0; i < mLines.size(); i++){
        vaLine *pLine = mLines[i];
        delete pLine;
        mLines[i] = NULL;
    }
    mLines.clear();
    InvalidateRect(hWnd(), NULL, TRUE);
}

LRESULT VLCHolderWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch( uMsg )
    {
        case WM_CREATE:{
            CREATESTRUCT* CreateStruct = (CREATESTRUCT*)(lParam);

            RECT ParentClientRect;
            GetClientRect(CreateStruct->hwndParent, &ParentClientRect);
            MoveWindow(hWnd(), 0, 0,
                       (ParentClientRect.right-ParentClientRect.left),
                       (ParentClientRect.bottom-ParentClientRect.top), FALSE);
            _CtrlsWnd =
                VLCControlsWnd::CreateControlsWindow(hInstance(), _wm,
                                                     hWnd());
            break;
        }
        case WM_PAINT:{
            PAINTSTRUCT PaintStruct;
            HDC hDC = BeginPaint(hWnd(), &PaintStruct);
            onPaint(hDC);
#if 0
            RECT rect;
            GetClientRect(hWnd(), &rect);
            int IconX = ((rect.right - rect.left) - GetSystemMetrics(SM_CXICON))/2;
            int IconY = ((rect.bottom - rect.top) - GetSystemMetrics(SM_CYICON))/2;
            DrawIcon(hDC, IconX, IconY, RC().hBackgroundIcon);
#endif
            EndPaint(hWnd(), &PaintStruct);
            break;
        }
        case WM_SHOWWINDOW:{
            if(FALSE!=wParam){ //showing
                NeedShowControls();
            }
            break;
        }
        case WM_TRY_SET_MOUSE_HOOK:{
            MouseHook(true);
            break;
        }
        case WM_SIZE:
            if(_CtrlsWnd){
                int new_client_width = LOWORD(lParam);
                int new_client_height = HIWORD(lParam);

                RECT rect;
                GetWindowRect(_CtrlsWnd->hWnd(), &rect);

                MoveWindow(_CtrlsWnd->hWnd(),
                           0, new_client_height - (rect.bottom - rect.top),
                           new_client_width, (rect.bottom-rect.top), TRUE);
            }
            break;
        case WM_LBUTTONDOWN:
        {
			// note:fanhongxuan@gmail.com
			// left down, add a new point
			bool isDelete = false;
			libvlc_state_t state = libvlc_NothingSpecial;
			if (NULL != VP()){state = VP()->get_state();}
			if (state != libvlc_Playing && state != libvlc_Paused){
				break;
			}
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
			vaPoint pt;
			pt.x = x;pt.y = y;
			std::list<vaRect *>::iterator it = mRects.begin();
			while(it != mRects.end()){
				vaRect *pRect = *it;
				if (NULL != pRect && pRect->IsOnCaption(x, y)){
					// delete this
					delete pRect;
					mRects.erase(it);
					isDelete = true;
					break;
				}
				it++;
			}
			if (!isDelete){
				mCurRect.AddPoint(pt);
			}
			InvalidateRect(hWnd(), NULL, FALSE);
#if 0			
            if (!mCurLine.IsStart()){
                mCurLine.Start(x,y);
            }
            else{
                // ignore this line
                mCurLine.Reset();
            }
#endif
            break;
        }
        case WM_LBUTTONUP:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
			if (mCurRect.SetRectRightBottom(x, y)){
				libvlc_state_t state = libvlc_NothingSpecial;
			    if (NULL != VP()){state = VP()->get_state();}
				if (state != libvlc_Playing && state != libvlc_Paused){
					break;
				}
				mCurRect.SetFinish(true);
				mCurRect.SetHwnd(hWnd());
				mRects.push_back(new vaRect(mCurRect));
				mCurRect.Reset();
                InvalidateRect(hWnd(), NULL, FALSE);
			}
#if 0
            if (mCurLine.IsStart()){
                mCurLine.Stop(x,y);
                mLines.push_back(new vaLine(mCurLine));
            }
            mCurLine.Reset();
            InvalidateRect(hWnd(), NULL, FALSE);
#endif
			
            break;
        }
        case WM_RBUTTONDOWN:
        {
#if 0
            ClearAllLines();
#endif
			libvlc_state_t state = libvlc_NothingSpecial;
			if (NULL != VP()){state = VP()->get_state();}
			if (state != libvlc_Playing && state != libvlc_Paused){
				break;
			}
			mCurRect.DelLastPoint();
			InvalidateRect(hWnd(), NULL, TRUE);
            break;
        }
        case WM_LBUTTONDBLCLK:
			{
				// note:fanhongxuan@gmail.com
				// when left doble click, add the mCurRect to mRects.
				libvlc_state_t state = libvlc_NothingSpecial;
			    if (NULL != VP()){state = VP()->get_state();}
				if (state != libvlc_Playing && state != libvlc_Paused){
					break;
				}
				if (mCurRect.IsClosed()){
					mCurRect.SetFinish(true);
					mCurRect.SetHwnd(hWnd());
					mRects.push_back(new vaRect(mCurRect));
					mCurRect.Reset();
				}
				InvalidateRect(hWnd(), NULL, FALSE);
				// note:fanhongxuan@gmail.com
				// 
			}
			break;
        case WM_MOUSEMOVE:
			{
				if (wParam & MK_LBUTTON){
					int x = GET_X_LPARAM(lParam);
					int y = GET_Y_LPARAM(lParam);
					libvlc_state_t state = libvlc_NothingSpecial;
			        if (NULL != VP()){state = VP()->get_state();}
					if (state != libvlc_Playing && state != libvlc_Paused){
						break;
					}
					if (mCurRect.SetRectRightBottom(x, y)){
						InvalidateRect(hWnd(), NULL, FALSE);
					}
				}
			}
            // WM().OnMouseEvent(uMsg);
            break;
        case WM_MOUSE_EVENT_NOTIFY:{
            WM().OnMouseEvent(wParam);
            return WM_MOUSE_EVENT_NOTIFY_SUCCESS;
        }
        default:
            return VLCWnd::WindowProc(uMsg, wParam, lParam);
    }
    return 0;
}

void VLCHolderWnd::DestroyWindow()
{
    LibVlcDetach();

    if(_CtrlsWnd){
        delete _CtrlsWnd;
        _CtrlsWnd = 0;
    }

    if(hWnd())
        ::DestroyWindow(hWnd());
};

LRESULT CALLBACK VLCHolderWnd::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    bool AllowReceiveMessage = true;
    if(nCode >= 0){
        switch(wParam){
            case WM_MOUSEMOVE:
            case WM_LBUTTONDBLCLK:{
                MOUSEHOOKSTRUCT* mhs = reinterpret_cast<MOUSEHOOKSTRUCT*>(lParam);

                //try to find HolderWnd and notify it
                HWND hNotifyWnd = mhs->hwnd;
                LRESULT SMRes = ::SendMessage(hNotifyWnd, WM_MOUSE_EVENT_NOTIFY, wParam, 0);
                while( hNotifyWnd && WM_MOUSE_EVENT_NOTIFY_SUCCESS != SMRes){
                    hNotifyWnd = GetParent(hNotifyWnd);
                    SMRes = ::SendMessage(hNotifyWnd, WM_MOUSE_EVENT_NOTIFY, wParam, 0);
                }

                AllowReceiveMessage = WM_MOUSEMOVE==wParam || (WM_MOUSE_EVENT_NOTIFY_SUCCESS != SMRes);
                break;
            }
        }
    }

    LRESULT NHRes = CallNextHookEx(NULL, nCode, wParam, lParam);
    if(AllowReceiveMessage)
        return NHRes;
    else
        return 1;
}

void VLCHolderWnd::MouseHook(bool SetHook)
{
    if(SetHook){
        HWND hMPWnd = FindMP_hWnd();
        const DWORD WndThreadID = (hMPWnd) ? GetWindowThreadProcessId(hMPWnd, NULL) : 0;
        if( _hMouseHook &&( !hMPWnd || WndThreadID != _MouseHookThreadId) ){
            //unhook if something changed
            MouseHook(false);
        }

        if(!_hMouseHook && hMPWnd && WndThreadID){
            _MouseHookThreadId = WndThreadID;
            _hMouseHook =
                SetWindowsHookEx(WH_MOUSE, VLCHolderWnd::MouseHookProc,
                                 NULL, WndThreadID);
        }
    }
    else{
        if(_hMouseHook){
            UnhookWindowsHookEx(_hMouseHook);
            _MouseHookThreadId=0;
            _hMouseHook = 0;
        }
    }
}

HWND VLCHolderWnd::FindMP_hWnd()
{
    if(_CtrlsWnd)
        return GetWindow(_CtrlsWnd->hWnd(), GW_HWNDNEXT);
    else
        return GetWindow(hWnd(), GW_CHILD);
}

//libvlc events arrives from separate thread
void VLCHolderWnd::OnLibVlcEvent(const libvlc_event_t* event)
{
    //We need set hook to catch doubleclicking (to switch to fullscreen and vice versa).
    //But libvlc media window may not exist yet,
    //and we don't know when it will be created, nor ThreadId of it.
    //So we try catch events,
    //(suppose wnd will be ever created),
    //and then try set mouse hook.
    HWND hMPWnd = FindMP_hWnd();
    const DWORD WndThreadID = (hMPWnd) ? GetWindowThreadProcessId(hMPWnd, NULL) : 0;
    //if no hook, or window thread has changed
    if(!_hMouseHook || (hMPWnd && WndThreadID != _MouseHookThreadId)){
        DWORD s = GetWindowLong(hMPWnd, GWL_STYLE);
        s |= WS_CLIPSIBLINGS;
        SetWindowLong(hMPWnd, GWL_STYLE, s);

        //libvlc events arrives from separate thread,
        //so we need post message to main thread, to notify it.
        PostMessage(hWnd(), WM_TRY_SET_MOUSE_HOOK, 0, 0);
    }

    if( _CtrlsWnd )
        _CtrlsWnd->OnLibVlcEvent(event);
}

/**
 * add by fanhongxuan@gmail.com begin
 *
 */

static void *vlc_video_lock_callback(void *pData, void **p_pixels)
{
	VLCHolderWnd *pWnd = reinterpret_cast<VLCHolderWnd*>(pData);
	if (NULL == pWnd){
		*p_pixels = NULL;
		return 0;
	}
	if (NULL == pWnd->mpBuffer){
		pWnd->mpBuffer = new unsigned char[pWnd->mWidth * pWnd->mHeight * 4];
	}
	*p_pixels = pWnd->mpBuffer;
	return 0;
}

static void vlc_video_unlock_callback(void *pData, void *picture, void *const *planes)
{
    // when a frame is decoded ok, will call this function
	VLCHolderWnd *pWnd = reinterpret_cast<VLCHolderWnd *>(pData);
	if (NULL != pWnd){
		pWnd->mFrameCount++;
        InvalidateRect(pWnd->hWnd(),NULL, FALSE);
	}
}

static void vlc_video_display_callback(void *opaque, void *picture)
{
    // when a display is needed, will call this
}

/**
 * add by fanhongxuan@gmail.com end
 */
void VLCHolderWnd::LibVlcAttach()
{
    // modified by fanhongxuan@gmail.com
    #if 0
    if( VP() )
        libvlc_media_player_set_hwnd( VP()->get_mp(), hWnd() );
    #endif
	// todo:fanhongxuan@gmail.com
	// read the mWidth and height from the current setting of the window.
	RECT rect;
	::GetWindowRect(hWnd(), &rect);
	mWidth = rect.right - rect.left;
	mHeight = rect.bottom - rect.top;
    int pitch = mWidth * 4;
	libvlc_media_player_t *pVp = VP()->get_mp();
	if (NULL == pVp){
		return;
	}
    libvlc_video_set_format(VP()->get_mp(), "RGBA", mWidth, mHeight, pitch);
    libvlc_video_set_callbacks(VP()->get_mp(), 
        vlc_video_lock_callback, 
        vlc_video_unlock_callback, 
        vlc_video_display_callback, this);
}

void VLCHolderWnd::LibVlcDetach()
{
    if (NULL != VP() && NULL != VP()->get_mp()){
        libvlc_video_set_callbacks(VP()->get_mp(),NULL,NULL,NULL, NULL);
    }
    #if 0
    if( VP() )
        libvlc_media_player_set_hwnd( VP()->get_mp(), 0);
    #endif
    MouseHook(false);
}

////////////////////////////////////////////////////////////////////////////////
//VLCFullScreenWnd members
////////////////////////////////////////////////////////////////////////////////
HINSTANCE VLCFullScreenWnd::_hinstance = 0;
ATOM VLCFullScreenWnd::_fullscreen_wndclass_atom = 0;

void VLCFullScreenWnd::RegisterWndClassName(HINSTANCE hInstance)
{
    //save hInstance for future use
    _hinstance = hInstance;

    WNDCLASS wClass;

    memset(&wClass, 0 , sizeof(wClass));
    if( ! GetClassInfo(_hinstance,  getClassName(), &wClass) )
    {
        wClass.style          = CS_NOCLOSE|CS_DBLCLKS;
        wClass.lpfnWndProc    = FSWndWindowProc;
        wClass.cbClsExtra     = 0;
        wClass.cbWndExtra     = 0;
        wClass.hInstance      = _hinstance;
        wClass.hIcon          = NULL;
        wClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wClass.hbrBackground  = (HBRUSH)(COLOR_3DFACE+1);
        wClass.lpszMenuName   = NULL;
        wClass.lpszClassName  = getClassName();

        _fullscreen_wndclass_atom = RegisterClass(&wClass);
    }
    else
    {
        _fullscreen_wndclass_atom = 0;
    }

}
void VLCFullScreenWnd::UnRegisterWndClassName()
{
    if(0 != _fullscreen_wndclass_atom){
        UnregisterClass(MAKEINTATOM(_fullscreen_wndclass_atom), _hinstance);
        _fullscreen_wndclass_atom = 0;
    }
}

LRESULT CALLBACK VLCFullScreenWnd::FSWndWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    VLCFullScreenWnd* fs_data = reinterpret_cast<VLCFullScreenWnd *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch( uMsg )
    {
        case WM_CREATE:{
            CREATESTRUCT* CreateStruct = (CREATESTRUCT*)(lParam);
            VLCWindowsManager* WM = (VLCWindowsManager*)CreateStruct->lpCreateParams;

            fs_data = new VLCFullScreenWnd(hWnd, WM);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(fs_data));

            break;
        }
        case WM_NCDESTROY:
            delete fs_data;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
            break;
        case WM_SHOWWINDOW:{
            if(FALSE==wParam){ //hiding
                break;
            }

            //simulate lParam for WM_SIZE
            RECT ClientRect;
            GetClientRect(hWnd, &ClientRect);
            lParam = MAKELPARAM(ClientRect.right, ClientRect.bottom);
        }
        case WM_SIZE:{
            if(fs_data->_WindowsManager->IsFullScreen()){
                int new_client_width = LOWORD(lParam);
                int new_client_height = HIWORD(lParam);
                VLCHolderWnd* HolderWnd =  fs_data->_WindowsManager->getHolderWnd();
                SetWindowPos(HolderWnd->hWnd(), HWND_BOTTOM, 0, 0,
                             new_client_width, new_client_height,
                             SWP_NOACTIVATE|SWP_NOOWNERZORDER);
            }
            break;
        }
        case WM_KEYDOWN: {
            if (fs_data)
                fs_data->_WindowsManager->OnKeyDownEvent(wParam);
            break;
        }
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0L;
};

VLCFullScreenWnd* VLCFullScreenWnd::CreateFSWindow(VLCWindowsManager* WM)
{
    HWND hWnd = CreateWindow(getClassName(),
                TEXT("VLC ActiveX Full Screen Window"),
                WS_POPUP|WS_CLIPCHILDREN,
                0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
                0,
                0,
                VLCFullScreenWnd::_hinstance,
                (LPVOID)WM
                );
    if(hWnd)
        return reinterpret_cast<VLCFullScreenWnd*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    return 0;
}

///////////////////////
//VLCWindowsManager
///////////////////////
VLCWindowsManager::VLCWindowsManager(HMODULE hModule, const VLCViewResources& rc,
                                     const vlc_player_options* po)
    :_rc(rc), _hModule(hModule), _po(po), _hWindowedParentWnd(0), _vp(0),
    _HolderWnd(0), _FSWnd(0), _b_new_messages_flag(false), Last_WM_MOUSEMOVE_Pos(0)
{
    VLCFullScreenWnd::RegisterWndClassName(hModule);
}

VLCWindowsManager::~VLCWindowsManager()
{
    VLCFullScreenWnd::UnRegisterWndClassName();
}

void VLCWindowsManager::CreateWindows(HWND hWindowedParentWnd)
{
    _hWindowedParentWnd = hWindowedParentWnd;

    if(!_HolderWnd){
        _HolderWnd =
            VLCHolderWnd::CreateHolderWindow(getHModule(),
                                             hWindowedParentWnd, this);
    }
}

void VLCWindowsManager::DestroyWindows()
{
    if(_HolderWnd){
        _HolderWnd->DestroyWindow();
        delete _HolderWnd;
        _HolderWnd = 0;
    }

    if(_FSWnd){
        _FSWnd->DestroyWindow();
    }
    _FSWnd = 0;
}

void VLCWindowsManager::LibVlcAttach(vlc_player* vp)
{
    if(!_HolderWnd)
        return;//VLCWindowsManager::CreateWindows was not called

    if( vp && _vp != vp ){
        LibVlcDetach();
    }

    if( !_vp ){
        _vp = vp;
        VlcEvents(true);
    }

    _HolderWnd->LibVlcAttach();
}

void VLCWindowsManager::LibVlcDetach()
{
    if(_HolderWnd)
        _HolderWnd->LibVlcDetach();

    if(_vp){
        VlcEvents(false);
        _vp = 0;
    }
}

void VLCWindowsManager::StartFullScreen()
{
    if( !_HolderWnd || ( PO() && !PO()->get_enable_fs() ) )
        return;//VLCWindowsManager::CreateWindows was not called

    if( VP() && !IsFullScreen() ){
        if( !_FSWnd ){
            _FSWnd= VLCFullScreenWnd::CreateFSWindow(this);
        }

        RECT FSRect = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };

        HMONITOR hMonitor = MonitorFromWindow(_hWindowedParentWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO MonInfo;
        memset(&MonInfo, 0, sizeof(MonInfo));
        MonInfo.cbSize = sizeof(MonInfo);
        if( GetMonitorInfo(hMonitor, &MonInfo) ) {
            FSRect = MonInfo.rcMonitor;
        }

#ifdef _DEBUG
        //to simplify debugging in fullscreen mode
        UINT FSFlags = SWP_NOZORDER;
#else
        UINT FSFlags = 0;
#endif

        SetParent(_HolderWnd->hWnd(), _FSWnd->getHWND());
        SetWindowPos(_FSWnd->getHWND(), HWND_TOPMOST,
                     FSRect.left, FSRect.top,
                     FSRect.right - FSRect.left, FSRect.bottom - FSRect.top,
                     FSFlags);

        ShowWindow(_FSWnd->getHWND(), SW_SHOW);
    }
}

void VLCWindowsManager::EndFullScreen()
{
    if(!_HolderWnd)
        return;//VLCWindowsManager::CreateWindows was not called

    if(IsFullScreen()){
        SetParent(_HolderWnd->hWnd(), _hWindowedParentWnd);

        RECT WindowedParentRect;
        GetClientRect(_hWindowedParentWnd, &WindowedParentRect);
        MoveWindow(_HolderWnd->hWnd(), 0, 0,
                   WindowedParentRect.right, WindowedParentRect.bottom, FALSE);

        ShowWindow(_FSWnd->getHWND(), SW_HIDE);

        if(_FSWnd){
            _FSWnd->DestroyWindow();
        }
        _FSWnd = 0;
   }
}

void VLCWindowsManager::ToggleFullScreen()
{
    if(IsFullScreen()){
        EndFullScreen();
    }
    else{
        StartFullScreen();
    }
}

bool VLCWindowsManager::IsFullScreen()
{
    return 0!=_FSWnd && 0!=_HolderWnd && GetParent(_HolderWnd->hWnd())==_FSWnd->getHWND();
}

void VLCWindowsManager::OnKeyDownEvent(UINT uKeyMsg)
{
    switch(uKeyMsg){
        case VK_ESCAPE:
        case 'F':
            EndFullScreen();
            break;
    }
}

void VLCWindowsManager::OnMouseEvent(UINT uMouseMsg)
{
    switch(uMouseMsg){
        case WM_LBUTTONDBLCLK:
            ToggleFullScreen();
            break;
        case WM_MOUSEMOVE:{
            DWORD MsgPos = GetMessagePos();
            if(Last_WM_MOUSEMOVE_Pos != MsgPos){
                Last_WM_MOUSEMOVE_Pos = MsgPos;
                _HolderWnd->NeedShowControls();
            }
            break;
        }
    }
}

//libvlc events arrives from separate thread
void VLCWindowsManager::OnLibVlcEvent_proxy(const libvlc_event_t* event, void *param)
{
    VLCWindowsManager* WM = static_cast<VLCWindowsManager*>(param);
    WM->OnLibVlcEvent(event);
}

void VLCWindowsManager::VlcEvents(bool Attach)
{
    if( !VP() )
        return;

    vlc_player& vp = *VP();

    libvlc_event_manager_t* em =
        libvlc_media_player_event_manager( vp.get_mp() );
    if(!em)
        return;

    for(int e=libvlc_MediaPlayerMediaChanged; e<=libvlc_MediaPlayerVout; ++e){
        switch(e){
        //case libvlc_MediaPlayerMediaChanged:
        //case libvlc_MediaPlayerNothingSpecial:
        //case libvlc_MediaPlayerOpening:
        //case libvlc_MediaPlayerBuffering:
        case libvlc_MediaPlayerPlaying:
        case libvlc_MediaPlayerPaused:
        case libvlc_MediaPlayerStopped:
        //case libvlc_MediaPlayerForward:
        //case libvlc_MediaPlayerBackward:
        //case libvlc_MediaPlayerEndReached:
        //case libvlc_MediaPlayerEncounteredError:
        //case libvlc_MediaPlayerTimeChanged:
        case libvlc_MediaPlayerPositionChanged:
        //case libvlc_MediaPlayerSeekableChanged:
        //case libvlc_MediaPlayerPausableChanged:
        //case libvlc_MediaPlayerTitleChanged:
        //case libvlc_MediaPlayerSnapshotTaken:
        //case libvlc_MediaPlayerLengthChanged:
        //case libvlc_MediaPlayerVout:
            if(Attach)
                libvlc_event_attach(em, e, OnLibVlcEvent_proxy, this);
            else
                libvlc_event_detach(em, e, OnLibVlcEvent_proxy, this);
            break;
        }
    }
}

void VLCWindowsManager::OnLibVlcEvent(const libvlc_event_t* event)
{
    if(_HolderWnd) _HolderWnd->OnLibVlcEvent(event);
    if(_FSWnd) _FSWnd->OnLibVlcEvent(event);
}
