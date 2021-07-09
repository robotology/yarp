/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FRAMEGRABBERINTERFACES_H
#define YARP_FRAMEGRABBERINTERFACES_H

#include <yarp/conf/system.h>
#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_FRAMEGRABBERINTERFACES_H_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/dev/FrameGrabberInterfaces.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5.0

#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IFrameGrabberControlsDC1394.h>
#include <yarp/dev/IFrameWriterImage.h>

#define YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_FRAMEGRABBERCONTROL2_H_ON_PURPOSE
#include <yarp/dev/FrameGrabberControl2.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_FRAMEGRABBERCONTROL2_H_ON_PURPOSE

#define YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_IFRAMEGRABBER_H_ON_PURPOSE
#include <yarp/dev/IFrameGrabber.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_IFRAMEGRABBER_H_ON_PURPOSE

#define YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_IFRAMEGRABBERRGB_H_ON_PURPOSE
#include <yarp/dev/IFrameGrabberRgb.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_IFRAMEGRABBERRGB_H_ON_PURPOSE

#endif // YARP_NO_DEPRECATED

#endif // YARP_FRAMEGRABBERINTERFACES_H
