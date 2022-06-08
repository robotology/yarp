/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
