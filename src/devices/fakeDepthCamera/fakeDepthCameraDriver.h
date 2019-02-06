/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef DEPTHCAMERA_DRIVER_H
#define DEPTHCAMERA_DRIVER_H

#include <iostream>
#include <string>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/RateThread.h>
#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>

namespace yarp
{
    namespace dev
    {
        class fakeDepthCameraDriver;
    }
}

class yarp::dev::fakeDepthCameraDriver : public yarp::dev::DeviceDriver,
                                         public yarp::dev::IRGBDSensor
{
private:
    typedef yarp::sig::ImageOf<yarp::sig::PixelFloat> depthImage;
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    typedef yarp::os::Stamp                           Stamp;
    typedef yarp::os::Property                        Property;
    typedef yarp::sig::FlexImage                      FlexImage;

#endif
public:
    fakeDepthCameraDriver();
    ~fakeDepthCameraDriver();

    // DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // IRGBDSensor
    int    getRgbHeight() override;
    int    getRgbWidth() override;
    bool   getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations) override;
    bool   getRgbResolution(int &width, int &height) override;
    bool   setRgbResolution(int width, int height) override;
    bool   getRgbFOV(double& horizontalFov, double& verticalFov) override;
    bool   setRgbFOV(double horizontalFov, double verticalFov) override;
    bool   getRgbMirroring(bool& mirror) override;
    bool   setRgbMirroring(bool mirror) override;

    bool   getRgbIntrinsicParam(Property& intrinsic) override;
    int    getDepthHeight() override;
    int    getDepthWidth() override;
    bool   setDepthResolution(int width, int height) override;
    bool   getDepthFOV(double& horizontalFov, double& verticalFov) override;
    bool   setDepthFOV(double horizontalFov, double verticalFov) override;
    bool   getDepthIntrinsicParam(Property& intrinsic) override;
    double getDepthAccuracy() override;
    bool   setDepthAccuracy(double accuracy) override;
    bool   getDepthClipPlanes(double& nearPlane, double& farPlane) override;
    bool   setDepthClipPlanes(double nearPlane, double farPlane) override;
    bool   getDepthMirroring(bool& mirror) override;
    bool   setDepthMirroring(bool mirror) override;


    bool   getExtrinsicParam(sig::Matrix &extrinsic) override;
    bool   getRgbImage(FlexImage& rgbImage, Stamp* timeStamp = NULL) override;
    bool   getDepthImage(depthImage& depthImage, Stamp* timeStamp = NULL) override;
    bool   getImages(FlexImage& colorFrame, depthImage& depthFrame, Stamp* colorStamp=NULL, Stamp* depthStamp=NULL) override;

    RGBDSensor_status     getSensorStatus() override;
    std::string getLastErrorMsg(Stamp* timeStamp = NULL) override;

private:
    double rgb_h{480};
    double rgb_w{640};
    double dep_h{480};
    double dep_w{640};
    double accuracy{0.001};
    double rgb_Vfov{36};
    double rgb_Hfov{50};
    double dep_Vfov{36};
    double dep_Hfov{50};
    double dep_near{0.4};
    double dep_far{6};

    yarp::sig::ImageOf<yarp::sig::PixelRgb> imageof;
    yarp::dev::PolyDriver                   testgrabber;
    IFrameGrabberImage*                     image;


};
#endif
