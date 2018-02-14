/*
 * Copyright (C) 2010 RobotCub Consortium
 * Author: Alessandro Scalzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/dev/ServerFrameGrabber.h>
#include <yarp/dev/RemoteFrameGrabberDC1394.h>
#include <yarp/dev/PolyDriver.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

// should move more of implementation into this file

// just to keep linkers from complaining about empty archive
bool dummyRemoteFrameGrabberDC1394Method() {
    return false;
}
