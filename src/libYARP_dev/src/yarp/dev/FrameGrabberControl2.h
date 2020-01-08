/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_FRAMEGRABBERCONTROL2_H
#define YARP_DEV_FRAMEGRABBERCONTROL2_H

#include <yarp/dev/FrameGrabberInterfaces.h>        // to include VOCAB definitions

/*! \file FrameGrabberControl2.h define common interfaces to discover
 * remote camera capabilities */

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0

namespace yarp{
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
#endif

#endif  // YARP_DEV_FRAMEGRABBERCONTROL2_H
