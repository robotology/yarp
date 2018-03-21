/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "RemoteFrameGrabber.h"

#include <yarp/dev/Drivers.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

// should move more of implementation into this file

// needed for the driver factory.
yarp::dev::DriverCreator *createRemoteFrameGrabber() {
    return new yarp::dev::DriverCreatorOf<yarp::dev::RemoteFrameGrabber>("remote_grabber",
                                                                         "grabber",
                                                                         "yarp::dev::RemoteFrameGrabber");
}
