/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

int main()
{
    //YARP network initialization
    yarp::os::Network yarp;
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yError() << "Cannot connect to yarp network";
    }

    yInfo("Hello...");
    yarp::os::Time::delay(1);
    yInfo("...world");

    return 0;
}
