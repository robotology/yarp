// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 the Wine project
 * CopyPolicy: Released under the terms of the GNU LGPL v2.1. or later
 *
 */

/*
 * Copyright (C) the Wine project
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef _vfwWine
#define _vfwWine

// pasted from WINE's vfw.h;

/** 
	@todo later, if mingw's vfw.h becomes complete - delete this file
 */

#ifdef WINE_NO_UNICODE_MACROS
# define WINELIB_NAME_AW(func) \
    func##_must_be_suffixed_with_W_or_A_in_this_context \
    func##_must_be_suffixed_with_W_or_A_in_this_context
#else  /* WINE_NO_UNICODE_MACROS */
# ifdef UNICODE
#  define WINELIB_NAME_AW(func) func##W
# else
#  define WINELIB_NAME_AW(func) func##A
# endif
#endif  /* WINE_NO_UNICODE_MACROS */
#define WM_CAP_START                    WM_USER

#define WM_CAP_UNICODE_START            WM_USER+100

#define WM_CAP_GET_CAPSTREAMPTR         (WM_CAP_START + 1)

#define WM_CAP_SET_CALLBACK_ERRORW      (WM_CAP_UNICODE_START + 2)
#define WM_CAP_SET_CALLBACK_STATUSW     (WM_CAP_UNICODE_START + 3)
#define WM_CAP_SET_CALLBACK_ERRORA      (WM_CAP_START + 2)
#define WM_CAP_SET_CALLBACK_STATUSA     (WM_CAP_START+ 3)

#define WM_CAP_SET_CALLBACK_ERROR       WINELIB_NAME_AW(WM_CAP_SET_CALLBACK_ERROR)
#define WM_CAP_SET_CALLBACK_STATUS      WINELIB_NAME_AW(WM_CAP_SET_CALLBACK_STATUS)

#define WM_CAP_SET_CALLBACK_YIELD       (WM_CAP_START +  4)
#define WM_CAP_SET_CALLBACK_FRAME       (WM_CAP_START +  5)
#define WM_CAP_SET_CALLBACK_VIDEOSTREAM (WM_CAP_START +  6)
#define WM_CAP_SET_CALLBACK_WAVESTREAM  (WM_CAP_START +  7)
#define WM_CAP_GET_USER_DATA            (WM_CAP_START +  8)
#define WM_CAP_SET_USER_DATA            (WM_CAP_START +  9)

#define WM_CAP_DRIVER_CONNECT           (WM_CAP_START +  10)
#define WM_CAP_DRIVER_DISCONNECT        (WM_CAP_START +  11)

#define WM_CAP_DRIVER_GET_NAMEA         (WM_CAP_START +  12)
#define WM_CAP_DRIVER_GET_VERSIONA      (WM_CAP_START +  13)
#define WM_CAP_DRIVER_GET_NAMEW         (WM_CAP_UNICODE_START +  12)
#define WM_CAP_DRIVER_GET_VERSIONW      (WM_CAP_UNICODE_START +  13)

#define WM_CAP_DRIVER_GET_NAME          WINELIB_NAME_AW(WM_CAP_DRIVER_GET_NAME)
#define WM_CAP_DRIVER_GET_VERSION       WINELIB_NAME_AW(WM_CAP_DRIVER_GET_VERSION)

#define WM_CAP_DRIVER_GET_CAPS          (WM_CAP_START +  14)

#define WM_CAP_FILE_SET_CAPTURE_FILEA   (WM_CAP_START +  20)
#define WM_CAP_FILE_GET_CAPTURE_FILEA   (WM_CAP_START +  21)
#define WM_CAP_FILE_ALLOCATE            (WM_CAP_START +  22)
#define WM_CAP_FILE_SAVEASA             (WM_CAP_START +  23)
#define WM_CAP_FILE_SET_INFOCHUNK       (WM_CAP_START +  24)
#define WM_CAP_FILE_SAVEDIBA            (WM_CAP_START +  25)
#define WM_CAP_FILE_SET_CAPTURE_FILEW   (WM_CAP_UNICODE_START +  20)
#define WM_CAP_FILE_GET_CAPTURE_FILEW   (WM_CAP_UNICODE_START +  21)
#define WM_CAP_FILE_SAVEASW             (WM_CAP_UNICODE_START +  23)
#define WM_CAP_FILE_SAVEDIBW            (WM_CAP_UNICODE_START +  25)

#define WM_CAP_FILE_SET_CAPTURE_FILE    WINELIB_NAME_AW(WM_CAP_FILE_SET_CAPTURE_FILE)
#define WM_CAP_FILE_GET_CAPTURE_FILE    WINELIB_NAME_AW(WM_CAP_FILE_GET_CAPTURE_FILE)
#define WM_CAP_FILE_SAVEAS              WINELIB_NAME_AW(WM_CAP_FILE_SAVEAS)
#define WM_CAP_FILE_SAVEDIB             WINELIB_NAME_AW(WM_CAP_FILE_SAVEDIB)

#define WM_CAP_EDIT_COPY                (WM_CAP_START +  30)

#define WM_CAP_SET_AUDIOFORMAT          (WM_CAP_START +  35)
#define WM_CAP_GET_AUDIOFORMAT          (WM_CAP_START +  36)

#define WM_CAP_DLG_VIDEOFORMAT          (WM_CAP_START +  41)
#define WM_CAP_DLG_VIDEOSOURCE          (WM_CAP_START +  42)
#define WM_CAP_DLG_VIDEODISPLAY         (WM_CAP_START +  43)
#define WM_CAP_GET_VIDEOFORMAT          (WM_CAP_START +  44)
#define WM_CAP_SET_VIDEOFORMAT          (WM_CAP_START +  45)
#define WM_CAP_DLG_VIDEOCOMPRESSION     (WM_CAP_START +  46)

#define WM_CAP_SET_PREVIEW              (WM_CAP_START +  50)
#define WM_CAP_SET_OVERLAY              (WM_CAP_START +  51)
#define WM_CAP_SET_PREVIEWRATE          (WM_CAP_START +  52)
#define WM_CAP_SET_SCALE                (WM_CAP_START +  53)
#define WM_CAP_GET_STATUS               (WM_CAP_START +  54)
#define WM_CAP_SET_SCROLL               (WM_CAP_START +  55)

#define WM_CAP_GRAB_FRAME               (WM_CAP_START +  60)
#define WM_CAP_GRAB_FRAME_NOSTOP        (WM_CAP_START +  61)

#define WM_CAP_SEQUENCE                 (WM_CAP_START +  62)
#define WM_CAP_SEQUENCE_NOFILE          (WM_CAP_START +  63)
#define WM_CAP_SET_SEQUENCE_SETUP       (WM_CAP_START +  64)
#define WM_CAP_GET_SEQUENCE_SETUP       (WM_CAP_START +  65)

#define WM_CAP_SET_MCI_DEVICEA          (WM_CAP_START +  66)
#define WM_CAP_GET_MCI_DEVICEA          (WM_CAP_START +  67)
#define WM_CAP_SET_MCI_DEVICEW          (WM_CAP_UNICODE_START +  66)
#define WM_CAP_GET_MCI_DEVICEW          (WM_CAP_UNICODE_START +  67)

#define WM_CAP_SET_MCI_DEVICE           WINELIB_NAME_AW(WM_CAP_SET_MCI_DEVICE)
#define WM_CAP_GET_MCI_DEVICE           WINELIB_NAME_AW(WM_CAP_GET_MCI_DEVICE)

#define WM_CAP_STOP                     (WM_CAP_START +  68)
#define WM_CAP_ABORT                    (WM_CAP_START +  69)

#define WM_CAP_SINGLE_FRAME_OPEN        (WM_CAP_START +  70)
#define WM_CAP_SINGLE_FRAME_CLOSE       (WM_CAP_START +  71)
#define WM_CAP_SINGLE_FRAME             (WM_CAP_START +  72)

#define WM_CAP_PAL_OPENA                (WM_CAP_START +  80)
#define WM_CAP_PAL_SAVEA                (WM_CAP_START +  81)
#define WM_CAP_PAL_OPENW                (WM_CAP_UNICODE_START +  80)
#define WM_CAP_PAL_SAVEW                (WM_CAP_UNICODE_START +  81)

#define WM_CAP_PAL_OPEN                 WINELIB_NAME_AW(WM_CAP_PAL_OPEN)
#define WM_CAP_PAL_SAVE                 WINELIB_NAME_AW(WM_CAP_PAL_SAVE)

#define WM_CAP_PAL_PASTE                (WM_CAP_START +  82)
#define WM_CAP_PAL_AUTOCREATE           (WM_CAP_START +  83)
#define WM_CAP_PAL_MANUALCREATE         (WM_CAP_START +  84)

#define WM_CAP_SET_CALLBACK_CAPCONTROL  (WM_CAP_START +  85)

#define WM_CAP_UNICODE_END              WM_CAP_PAL_SAVEW
#define WM_CAP_END                      WM_CAP_UNICODE_END

typedef struct tagCapDriverCaps {
    UINT        wDeviceIndex;
    BOOL        fHasOverlay;
    BOOL        fHasDlgVideoSource;
    BOOL        fHasDlgVideoFormat;
    BOOL        fHasDlgVideoDisplay;
    BOOL        fCaptureInitialized;
    BOOL        fDriverSuppliesPalettes;
    HANDLE      hVideoIn;
    HANDLE      hVideoOut;
    HANDLE      hVideoExtIn;
    HANDLE      hVideoExtOut;
} CAPDRIVERCAPS, *PCAPDRIVERCAPS, *LPCAPDRIVERCAPS;

typedef struct tagCapStatus {
    UINT        uiImageWidth;
    UINT        uiImageHeight;
    BOOL        fLiveWindow;
    BOOL        fOverlayWindow;
    BOOL        fScale;
    POINT       ptScroll;
    BOOL        fUsingDefaultPalette;
    BOOL        fAudioHardware;
    BOOL        fCapFileExists;
    DWORD       dwCurrentVideoFrame;
    DWORD       dwCurrentVideoFramesDropped;
    DWORD       dwCurrentWaveSamples;
    DWORD       dwCurrentTimeElapsedMS;
    HPALETTE    hPalCurrent;
    BOOL        fCapturingNow;
    DWORD       dwReturn;
    UINT        wNumVideoAllocated;
    UINT        wNumAudioAllocated;
} CAPSTATUS, *PCAPSTATUS, *LPCAPSTATUS;

typedef struct tagCaptureParms {
    DWORD       dwRequestMicroSecPerFrame;
    BOOL        fMakeUserHitOKToCapture;
    UINT        wPercentDropForError;
    BOOL        fYield;
    DWORD       dwIndexSize;
    UINT        wChunkGranularity;
    BOOL        fUsingDOSMemory;
    UINT        wNumVideoRequested;
    BOOL        fCaptureAudio;
    UINT        wNumAudioRequested;
    UINT        vKeyAbort;
    BOOL        fAbortLeftMouse;
    BOOL        fAbortRightMouse;
    BOOL        fLimitEnabled;
    UINT        wTimeLimit;
    BOOL        fMCIControl;
    BOOL        fStepMCIDevice;
    DWORD       dwMCIStartTime;
    DWORD       dwMCIStopTime;
    BOOL        fStepCaptureAt2x;
    UINT        wStepCaptureAverageFrames;
    DWORD       dwAudioBufferSize;
    BOOL        fDisableWriteCache;
    UINT        AVStreamMaster;
} CAPTUREPARMS, *PCAPTUREPARMS, *LPCAPTUREPARMS;

typedef struct videohdr_tag {
    LPBYTE      lpData;
    DWORD       dwBufferLength;
    DWORD       dwBytesUsed;
    DWORD       dwTimeCaptured;
    DWORD       dwUser;
    DWORD       dwFlags;
    DWORD       dwReserved[4];
} VIDEOHDR, *PVIDEOHDR, *LPVIDEOHDR;

#define VHDR_DONE       0x00000001
#define VHDR_PREPARED   0x00000002
#define VHDR_INQUEUE    0x00000004
#define VHDR_KEYFRAME   0x00000008

#endif

