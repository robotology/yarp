/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/RemoteFrameGrabberDC1394.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

// should move more of implementation into this file

// just to keep linkers from complaining about empty archive
bool dummyRemoteFrameGrabberDC1394Method() {
    return false;
}
