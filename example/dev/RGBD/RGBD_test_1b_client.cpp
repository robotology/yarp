/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/Time.h>

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

    Network yarp;
    PolyDriver clientDriver;
    Property clientConfig;

    // Keyword 'device' is used by YARP framework

    // client config
    clientConfig.put("device", "RGBDSensorClient");              // Network client receiving data from YARP network
    clientConfig.put("localImagePort", "/client/rgbInput:i");    // Name of the rgb input port opened by the client
    clientConfig.put("localDepthPort", "/client/depthInput:i");  // Name of the depth input port opened by the client
    clientConfig.put("remoteImagePort", "/server/rgbImage:o");   // Name of the server port broadcasting rgb image
    clientConfig.put("remoteDepthPort", "/server/depthImage:o"); // Name of the server port broadcasting depth image
    clientConfig.put("localRpcPort", "/client/rpc");             // Name of the client RPC port
    clientConfig.put("remoteRpcPort", "/server/rpc:i");          // Name of the server RPC port

    yInfo("Opening client");
    // Instantiating RGBD client
    clientDriver.open(clientConfig);
    if (!clientDriver.isValid()) {
        yError("Failed to create and configure the client device\n");
        return 1;
    }

    yarp::dev::IRGBDSensor* RGBDInterface; // interface we want to use
    if (!clientDriver.view(RGBDInterface)) //  wanted device interface
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

    yarp::os::Time::delay(0.1); // wait for first data to arrive
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
    clientDriver.close();

    return 0;
}
