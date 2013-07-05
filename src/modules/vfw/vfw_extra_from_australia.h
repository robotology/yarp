/*
 * Copyright (C) 2006 The University of Queensland, Australia <www.uq.edu.au>
 * CopyPolicy: GPL2 or later, or 4-clause BSD at your choice
 */

/**
	@author Written by Leon Zadorin at the Vislab at the University Of Queensland.
 */
/*
 * Either one of the following two licences may be applied to the contents
 * written by Leon Zadorin. 
 */
/*
 * Copyright (c) 2006 The University of Queensland, Australia <www.uq.edu.au>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *       This product includes software developed by 
 *       Vislab at The University of Queensland and its contributors.
 * 4. Neither the name of The University of Queensland nor the names of Vislab
 *    and contributors may be used to endorse or promote products derived from 
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY VISLAB AT THE UNIVERSITY OF QUEENSLAND AND
 * CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL VISLAB AT THE
 * UNIVERSITY OF QUEENSLAND OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Copyright (C) 2006 The University of Queensland, Australia <www.uq.edu.au>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _vfwExtra
#define _vfwExtra


/** 
	@todo later, if mingw's vfw.h becomes complete - delete this file
 */

// this is a quick and dirty hack, if anyone actually cares about the win port - they can write a better version and if anyone actually cares about running good software - they should use *bsd/linux in the first place and stop making the life for others difficult.

#ifdef __cplusplus
#define safeSendMessage(handle, message, param1, param2) (::IsWindow (handle) == True ? ::SendMessage (handle, message, (WPARAM)param1, (LPARAM)param2) : 0)
#else
#define safeSendMessage(handle, message, param1, param2) (IsWindow (handle) == True ? SendMessage (handle, message, (WPARAM)param1, (LPARAM)param2) : 0)
#endif

#define capSetCallbackOnError(handle, callback) (safeSendMessage (handle, WM_CAP_SET_CALLBACK_ERROR, 0, callback))
#define capSetCallbackOnVideoStream(handle, callback) (safeSendMessage (handle, WM_CAP_SET_CALLBACK_VIDEOSTREAM, 0, callback))
#define capSetUserData(handle, userData) (safeSendMessage (handle, WM_CAP_SET_USER_DATA, 0, userData))
#define capGetUserData(handle) (safeSendMessage (handle, WM_CAP_GET_USER_DATA, 0, 0))
#define capDriverConnect(handle, device) (safeSendMessage (handle, WM_CAP_DRIVER_CONNECT, device, 0))
#define capSetVideoFormat(handle, memory, size) (safeSendMessage (handle, WM_CAP_SET_VIDEOFORMAT, size, memory))
#define capDriverDisconnect(handle) (safeSendMessage (handle, WM_CAP_DRIVER_DISCONNECT, 0, 0))
#define capDriverGetCaps(handle, memory, size)           (safeSendMessage (handle, WM_CAP_DRIVER_GET_CAPS, size, memory))
#define capDlgVideoFormat(handle) (safeSendMessage (handle, WM_CAP_DLG_VIDEOFORMAT, 0, 0))
#define capDlgVideoSource(handle) (safeSendMessage (handle, WM_CAP_DLG_VIDEOSOURCE, 0, 0))
#define capDlgVideoDisplay(handle) (safeSendMessage (handle, WM_CAP_DLG_VIDEODISPLAY, 0, 0))
#define capGetVideoFormat(handle, memory, size) (safeSendMessage (handle, WM_CAP_GET_VIDEOFORMAT, size, memory))
#define capGetVideoFormatSize(handle) (safeSendMessage (handle, WM_CAP_GET_VIDEOFORMAT, 0, 0))
#define capSetVideoFormat(handle, memory, size) (safeSendMessage (handle, WM_CAP_SET_VIDEOFORMAT, size, memory))
#define capPreview(handle, doPreview) (safeSendMessage (handle, WM_CAP_SET_PREVIEW, doPreview, 0))
#define capPreviewRate(handle, rate) (safeSendMessage (handle, WM_CAP_SET_PREVIEWRATE, rate, 0))
#define capOverlay(handle, doOverlay) (safeSendMessage (handle, WM_CAP_SET_OVERLAY, doOverlay, 0))
#define capCaptureSequence(handle) (safeSendMessage (handle, WM_CAP_SEQUENCE, 0, 0))
#define capCaptureSequenceNoFile(handle) (safeSendMessage (handle, WM_CAP_SEQUENCE_NOFILE, 0, 0))
#define capCaptureStop(handle) (safeSendMessage (handle, WM_CAP_STOP, 0, 0))
#define capCaptureAbort(handle) (safeSendMessage (handle, WM_CAP_ABORT, 0, 0))
#define capCaptureSingleFrameOpen(handle) (safeSendMessage (handle, WM_CAP_SINGLE_FRAME_OPEN, 0, 0))
#define capCaptureGetSetup(handle, memory, size) (safeSendMessage (handle, WM_CAP_GET_SEQUENCE_SETUP, size, memory))
#define capCaptureSetSetup(handle, memory, size) (safeSendMessage (handle, WM_CAP_SET_SEQUENCE_SETUP, size, memory))

#endif
