/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>

#include <yarp/sig/Image.h>

#include <yarp/dev/Drivers.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/PolyDriver.h>

using yarp::dev::IRGBDSensor;
using yarp::dev::PolyDriver;
using yarp::os::Network;
using yarp::os::Property;
using yarp::sig::FlexImage;
using yarp::sig::ImageOf;
using yarp::sig::PixelFloat;

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Network yarp; // Initialize yarp framework

    // use YARP to create and configure an instance of fakeDepthCamera
    Property config;
    config.put("device", "fakeDepthCamera"); // device producing (fake) data
    config.put("mode", "ball");              // fake data type to be produced

    PolyDriver dd;
    dd.open(config);
    if (!dd.isValid()) {
        yError("Failed to create and configure a the fake device\n");
        return 1;
    }

    yarp::dev::IRGBDSensor* RGBDInterface; // interface we want to use
    if (!dd.view(RGBDInterface))           // grab wanted device interface
    {
        yError("Failed to get RGBDInterface device interface\n");
        return 1;
    }

    // Let's use the interface to get info from device
    int rgbImageHeight = RGBDInterface->getRgbHeight();
    int rgbImageWidth = RGBDInterface->getRgbWidth();
    int depthImageHeight = RGBDInterface->getDepthHeight();
    int depthImageWidth = RGBDInterface->getDepthWidth();

    YARP_UNUSED(rgbImageHeight);
    YARP_UNUSED(rgbImageWidth);
    YARP_UNUSED(depthImageHeight);
    YARP_UNUSED(depthImageWidth);

    FlexImage rgbImage;
    ImageOf<PixelFloat> depthImage;
    bool gotImage = RGBDInterface->getImages(rgbImage, depthImage);

    if (gotImage) {
        yInfo("Succesfully retieved an image");
    } else {
        yError("Failed retieving images");
    }

    yarp::os::Property intrinsic;
    RGBDInterface->getRgbIntrinsicParam(intrinsic);
    yInfo("RGB intrinsic parameters: \n%s", intrinsic.toString().c_str());
    dd.close();

    return 0;
}
