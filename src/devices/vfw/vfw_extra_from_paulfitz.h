/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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


