// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium, European Commission FP6 Project IST-004370
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 * Author: Paul Fitzpatrick, Alessandro Scalzo
*/


#include <yarp/dev/TestFrameGrabber.h>
#include <yarp/dev/ServerFrameGrabber.h>
#include <yarp/dev/RemoteFrameGrabber.h>
#include <yarp/dev/PolyDriver.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

// should move more of implementation into this file

// just to keep linkers from complaining about empty archive
bool dummyRemoteFrameGrabberMethod() {
    return false;
}
