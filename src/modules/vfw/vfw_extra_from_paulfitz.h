/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef VFW_EXTRA_PAULFITZ
#define VFW_EXTRA_PAULFITZ

#ifndef capSetCallbackOnFrame
#define capSetCallbackOnFrame(handle, callback) (safeSendMessage (handle, WM_CAP_SET_CALLBACK_FRAME, 0, callback))
#endif

#ifndef capPreviewScale
#define capPreviewScale(handle, flag) (safeSendMessage (handle, WM_CAP_SET_SCALE, flag, 0))
#endif

#endif


