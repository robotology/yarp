/*
* Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
* Author: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
* CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
*/

#ifndef DEPTHCAMERA_DRIVER_H
#define DEPTHCAMERA_DRIVER_H

#include <iostream>
#include <string>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/FrameGrabberControl2.h>
#include <yarp/os/RateThread.h>
#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>
#include <yarp/os/all.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>


#ifndef RAD2DEG
#define RAD2DEG (180/3.14159265359)
#endif

#ifndef DEG2RAD
#define DEG2RAD (3.14159265359/180.0)
#endif

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
    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    // IRGBDSensor
    virtual int    getRgbHeight();
    virtual int    getRgbWidth();
    virtual bool   getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations);
    virtual bool   getRgbResolution(int &width, int &height);
    virtual bool   setRgbResolution(int width, int height);
    virtual bool   getRgbFOV(double& horizontalFov, double& verticalFov);
    virtual bool   setRgbFOV(double horizontalFov, double verticalFov);
    virtual bool   getRgbMirroring(bool& mirror);
    virtual bool   setRgbMirroring(bool mirror);

    virtual bool   getRgbIntrinsicParam(Property& intrinsic);
    virtual int    getDepthHeight();
    virtual int    getDepthWidth();
    virtual bool   setDepthResolution(int width, int height);
    virtual bool   getDepthFOV(double& horizontalFov, double& verticalFov);
    virtual bool   setDepthFOV(double horizontalFov, double verticalFov);
    virtual bool   getDepthIntrinsicParam(Property& intrinsic);
    virtual double getDepthAccuracy();
    virtual bool   setDepthAccuracy(double accuracy);
    virtual bool   getDepthClipPlanes(double& nearPlane, double& farPlane);
    virtual bool   setDepthClipPlanes(double nearPlane, double farPlane);
    virtual bool   getDepthMirroring(bool& mirror);
    virtual bool   setDepthMirroring(bool mirror);


    virtual bool   getExtrinsicParam(sig::Matrix &extrinsic);
    virtual bool   getRgbImage(FlexImage& rgbImage, Stamp* timeStamp = NULL);
    virtual bool   getDepthImage(depthImage& depthImage, Stamp* timeStamp = NULL);
    virtual bool   getImages(FlexImage& colorFrame, depthImage& depthFrame, Stamp* colorStamp=NULL, Stamp* depthStamp=NULL);

    virtual RGBDSensor_status     getSensorStatus();
    virtual yarp::os::ConstString getLastErrorMsg(Stamp* timeStamp = NULL);

private:
    double rgb_h;
    double rgb_w;
    double dep_h;
    double dep_w;
    double accuracy;
    double rgb_Vfov;
    double rgb_Hfov;
    double dep_Vfov;
    double dep_Hfov;
    double dep_near;
    double dep_far;

    yarp::dev::PolyDriver testgrabber;
    IFrameGrabberImage*   image;


};
#endif
