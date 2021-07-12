/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_FRAMEGRABBERCONTROL2_H
#define YARP_DEV_FRAMEGRABBERCONTROL2_H

#include <yarp/conf/system.h>
#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_FRAMEGRABBERCONTROL2_H_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/dev/FrameGrabberControl2.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0

#define YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_FRAMEGRABBERINTERFACES_H_ON_PURPOSE
#include <yarp/dev/FrameGrabberInterfaces.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_FRAMEGRABBERINTERFACES_H_ON_PURPOSE

/*! \file FrameGrabberControl2.h define common interfaces to discover
 * remote camera capabilities */


namespace yarp {
namespace dev {
/**
 * This interface exports a list of general methods to discover the remote camera
 * capabilities. It also has the get/set feature methods so can be used in conjuntion
 * with former IFrameGrabberControls2 or as standalone.
 * @deprecated Since YARP 3.0.0
 */
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IFrameGrabberControl instead") IFrameGrabberControls IFrameGrabberControls2;

} // namespace dev
} // namespace yarp

#endif // YARP_NO_DEPRECATED

#endif  // YARP_DEV_FRAMEGRABBERCONTROL2_H
