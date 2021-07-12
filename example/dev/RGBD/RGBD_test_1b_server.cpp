/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/Time.h>

#include <yarp/dev/IWrapper.h>
#include <yarp/dev/PolyDriver.h>

using yarp::dev::IWrapper;
using yarp::dev::PolyDriver;
using yarp::os::Network;
using yarp::os::Property;

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Network yarp;
    PolyDriver wrapperDriver;
    PolyDriver rgbdDriver;
    Property wrapperConfig;
    Property rgbdConfig;

    // Keyword 'device' is used by YARP framework

    // Wrapper config
    wrapperConfig.put("device", "RGBDSensorWrapper"); // Network server broadcasting data through YARP network
    wrapperConfig.put("name", "/server");             // Prefix for port names opened by the YARP server

    // Device driver config
    rgbdConfig.put("device", "fakeDepthCamera"); // Device producing (fake) data
    rgbdConfig.put("mode", "ball");              // (fake) data type to be produced

    yInfo("Opening server");
    // Instantiating RGBD server
    wrapperDriver.open(wrapperConfig);
    if (!wrapperDriver.isValid()) {
        yError("Failed to create and configure the RGBD server\n");
        return 1;
    }

    yInfo("Opening fake device");
    // Instantiating RGBD device
    rgbdDriver.open(rgbdConfig);
    if (!rgbdDriver.isValid()) {
        yError("Failed to create and configure the fake device\n");
        return 1;
    }

    yInfo("Attaching server to device");
    IWrapper* wrapperInterface;
    wrapperDriver.view(wrapperInterface);
    wrapperInterface->attach(&rgbdDriver); // Bind the wrapper with the actual device

    // Stay up some time waiting for the client to do its work
    yarp::os::Time::delay(10);
    wrapperDriver.close();
    rgbdDriver.close();

    return 0;
}
