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
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IRGBDSensor.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main(int argc, char *argv[])
{
    Network yarp;       // Initialize yarp framework

    // use YARP to create and configure an instance of fakeDepthCamera
    Property config;
    config.put("device", "fakeDepthCamera");         // device producing (fake) data
    config.put("mode",   "ball");                    // fake data type to be produced

    PolyDriver dd;
    dd.open(config);
    if (!dd.isValid())
    {
        yError("Failed to create and configure a the fake device\n");
        return 1;
    }

    yarp::dev::IRGBDSensor *RGBDInterface;              // interface we want to use
    if (!dd.view(RGBDInterface))                        // grab wanted device interface
    {
        yError("Failed to get RGBDInterface device interface\n");
        return 1;
    }

    // Let's use the interface to get info from device
    int rgbImageHeight   = RGBDInterface->getRgbHeight();
    int rgbImageWidth    = RGBDInterface->getRgbWidth();
    int depthImageHeight = RGBDInterface->getDepthHeight();
    int depthImageWidth  = RGBDInterface->getDepthWidth();

    FlexImage rgbImage;
    ImageOf<PixelFloat> depthImage;
    bool gotImage = RGBDInterface->getImages(rgbImage, depthImage);

    if(gotImage)
        yInfo("Succesfully retieved an image");
    else
        yError("Failed retieving images");

    yarp::os::Property intrinsic;
    RGBDInterface->getRgbIntrinsicParam(intrinsic);
    yInfo("RGB intrinsic parameters: \n%s", intrinsic.toString().c_str());
    dd.close();

    return 0;
}
