/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_FRAMEGRABBERINTERFACES_H
#define YARP_FRAMEGRABBERINTERFACES_H

#include <yarp/conf/system.h>
#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/dev/IFrameGrabberInterfaces.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5.0

#include <yarp/dev/IFrameGrabberRgb.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IFrameGrabberImageRaw.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IFrameGrabberControlsDC1394.h>
#include <yarp/dev/IFrameWriterImage.h>

#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#include <yarp/dev/FrameGrabberControl2.h>
#include <yarp/dev/IFrameGrabber.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE

#endif // YARP_NO_DEPRECATED

#endif // YARP_FRAMEGRABBERINTERFACES_H
