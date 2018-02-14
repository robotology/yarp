/*
 * Copyright (C) 2006 RobotCub Consortium
 * Author: Paul Fitzpatrick, Alessandro Scalzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/


#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/RemoteFrameGrabber.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

// should move more of implementation into this file

// just to keep linkers from complaining about empty archive
bool dummyRemoteFrameGrabberMethod() {
    return false;
}
