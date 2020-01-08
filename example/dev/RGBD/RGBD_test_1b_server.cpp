/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <yarp/os/Time.h>
#include <yarp/dev/IWrapper.h>
#include <yarp/dev/Drivers.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IRGBDSensor.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main(int argc, char *argv[])
{
    Network yarp;
    PolyDriver wrapperDriver, rgbdDriver;
    Property   wrapperConfig, rgbdConfig;

    // Keyword 'device' is used by YARP framework

    // Wrapper config
    wrapperConfig.put("device", "RGBDSensorWrapper");       // Network server broadcasting data through YARP network
    wrapperConfig.put("name",   "/server");                 // Prefix for port names opened by the YARP server

    // Device driver config
    rgbdConfig.put("device", "fakeDepthCamera");            // Device producing (fake) data
    rgbdConfig.put("mode",   "ball");                       // (fake) data type to be produced

    yInfo() << "Opening server";
    // Instantiating RGBD server
    wrapperDriver.open(wrapperConfig);
    if (!wrapperDriver.isValid())
    {
        yError("Failed to create and configure the RGBD server\n");
        return 1;
    }

    yInfo() << "Opening fake device";
    // Instantiating RGBD device
    rgbdDriver.open(rgbdConfig);
    if (!rgbdDriver.isValid())
    {
        yError("Failed to create and configure the fake device\n");
        return 1;
    }

    yInfo() << "Attaching server to device";
    yarp::dev::IWrapper *wrapperInterface;
    wrapperDriver.view(wrapperInterface);
    wrapperInterface->attach(&rgbdDriver);                // Bind the wrapper with the actual device

    // Stay up some time waiting for the client to do its work
    yarp::os::Time::delay(10);
    wrapperDriver.close();
    rgbdDriver.close();

    return 0;
}
