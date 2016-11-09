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
#include <yarp/os/all.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/IRGBDSensor.h>
#include <OpenNI.h>

#ifndef RAD2DEG
#define RAD2DEG (180/3.14159265359)
#endif

namespace yarp
{
    namespace dev
    {
        class depthCameraDriver;
    }
}

class streamFrameListener : public openni::VideoStream::NewFrameListener
{
public:

    yarp::os::Mutex         mutex;
    yarp::os::Stamp         stamp;
    yarp::sig::FlexImage    image;
    openni::VideoFrameRef   frameRef;
    openni::PixelFormat     pixF;
    int                     w, h;

private:
    virtual void onNewFrame(openni::VideoStream& stream);
};

class yarp::dev::depthCameraDriver : public yarp::dev::DeviceDriver,
                                     public yarp::dev::IRGBDSensor,
                                     public yarp::dev::IFrameGrabberControls2
{
public:
    depthCameraDriver();
    ~depthCameraDriver();

    // DeviceDriver
    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    // IRGBDSensor
    virtual int    getRgbHeight();
    virtual int    getRgbWidth();
    virtual bool   setRgbResolution(int width, int height);
    virtual bool   getRgbFOV(double& horizontalFov, double& verticalFov);
    virtual bool   setRgbFOV(double horizontalFov, double verticalFov);
    virtual bool   getRgbIntrinsicParam(yarp::os::Property& intrinsic);
    virtual bool   getRgbSensorInfo(yarp::os::Property& info);
    virtual int    getDepthHeight();
    virtual int    getDepthWidth();
    virtual bool   setDepthResolution(int width, int height);
    virtual bool   getDepthFOV(double& horizontalFov, double& verticalFov);
    virtual bool   setDepthFOV(double horizontalFov, double verticalFov);
    virtual bool   getDepthIntrinsicParam(yarp::os::Property& intrinsic);
    virtual bool   getDepthSensorInfo(yarp::os::Property info);
    virtual double getDepthAccuracy();
    virtual bool   setDepthAccuracy(double accuracy);
    virtual bool   getDepthClipPlanes(double& near, double& far);
    virtual bool   setDepthClipPlanes(double near, double far);
    virtual bool   getExtrinsicParam(yarp::os::Property& extrinsic);
    virtual bool   getRgbImage(yarp::sig::FlexImage& rgbImage, yarp::os::Stamp* timeStamp = NULL);
    virtual bool   getDepthImage(yarp::sig::ImageOf<sig::PixelFloat> &depthImage, yarp::os::Stamp* timeStamp = NULL);
    virtual bool   getImages(yarp::sig::FlexImage& colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthFrame, yarp::os::Stamp* colorStamp=NULL, yarp::os::Stamp* depthStamp=NULL);
    virtual bool   getCameraDescription(CameraDescriptor *camera);
    virtual bool   hasFeature(int feature, bool *hasFeature);
    virtual bool   setFeature(int feature, double value);
    virtual bool   getFeature(int feature, double *value);
    virtual bool   setFeature(int feature, double value1, double value2);
    virtual bool   getFeature(int feature, double *value1, double *value2);
    virtual bool   hasOnOff(  int feature, bool *HasOnOff);
    virtual bool   setActive( int feature, bool onoff);
    virtual bool   getActive( int feature, bool *isActive);
    virtual bool   hasAuto(   int feature, bool *hasAuto);
    virtual bool   hasManual( int feature, bool *hasManual);
    virtual bool   hasOnePush(int feature, bool *hasOnePush);
    virtual bool   setMode(   int feature, FeatureMode mode);
    virtual bool   getMode(   int feature, FeatureMode *mode);
    virtual bool   setOnePush(int feature);


    virtual RGBDSensor_status     getSensorStatus();
    virtual yarp::os::ConstString getLastErrorMsg(yarp::os::Stamp* timeStamp = NULL);
    static int                    pixFormatToCode(openni::PixelFormat p);

private:
    //method
    inline bool initializeOpeNIDevice();

    bool        getImage(yarp::sig::FlexImage& Frame, yarp::os::Stamp* Stamp, streamFrameListener& sourceFrame);
    bool        getImage(yarp::sig::ImageOf<yarp::sig::PixelFloat>& Frame, yarp::os::Stamp* Stamp, streamFrameListener& sourceFrame);

    //properties
    openni::Device            m_device;
    openni::VideoStream       m_depthStream;
    openni::VideoStream       m_imageStream;
    streamFrameListener       m_depthFrame;
    streamFrameListener       m_imageFrame;
};
#endif
