/*
 * Copyright (C) 2006 RobotCub Consortium
 * Author: Paul Fitzpatrick, Alessandro Scalzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
