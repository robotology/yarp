/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "RemoteFrameGrabber.h"

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

YARP_LOG_COMPONENT(REMOTEFRAMEGRABBER, "yarp.devices.RemoteFrameGrabber")

RemoteFrameGrabber::RemoteFrameGrabber() :
        FrameGrabberControls_Forwarder(port),
        FrameGrabberControlsDC1394_Forwarder(port),
        RgbVisualParams_Forwarder(port)
{
}
