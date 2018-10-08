/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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
    virtual bool open(yarp::os::Searchable& config) override;
    virtual bool close() override;

    // IRGBDSensor
    virtual int    getRgbHeight() override;
    virtual int    getRgbWidth() override;
    virtual bool   getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations) override;
    virtual bool   getRgbResolution(int &width, int &height) override;
    virtual bool   setRgbResolution(int width, int height) override;
    virtual bool   getRgbFOV(double& horizontalFov, double& verticalFov) override;
    virtual bool   setRgbFOV(double horizontalFov, double verticalFov) override;
    virtual bool   getRgbMirroring(bool& mirror) override;
    virtual bool   setRgbMirroring(bool mirror) override;

    virtual bool   getRgbIntrinsicParam(Property& intrinsic) override;
    virtual int    getDepthHeight() override;
    virtual int    getDepthWidth() override;
    virtual bool   setDepthResolution(int width, int height) override;
    virtual bool   getDepthFOV(double& horizontalFov, double& verticalFov) override;
    virtual bool   setDepthFOV(double horizontalFov, double verticalFov) override;
    virtual bool   getDepthIntrinsicParam(Property& intrinsic) override;
    virtual double getDepthAccuracy() override;
    virtual bool   setDepthAccuracy(double accuracy) override;
    virtual bool   getDepthClipPlanes(double& nearPlane, double& farPlane) override;
    virtual bool   setDepthClipPlanes(double nearPlane, double farPlane) override;
    virtual bool   getDepthMirroring(bool& mirror) override;
    virtual bool   setDepthMirroring(bool mirror) override;


    virtual bool   getExtrinsicParam(sig::Matrix &extrinsic) override;
    virtual bool   getRgbImage(FlexImage& rgbImage, Stamp* timeStamp = NULL) override;
    virtual bool   getDepthImage(depthImage& depthImage, Stamp* timeStamp = NULL) override;
    virtual bool   getImages(FlexImage& colorFrame, depthImage& depthFrame, Stamp* colorStamp=NULL, Stamp* depthStamp=NULL) override;

    virtual RGBDSensor_status     getSensorStatus() override;
    virtual std::string getLastErrorMsg(Stamp* timeStamp = NULL) override;

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
