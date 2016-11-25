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

#ifndef DEG2RAD
#define DEG2RAD (3.14159265359/180.0)
#endif

namespace yarp
{
    namespace dev
    {
        class depthCameraDriver;
    }
}

struct plum_bob
{
    double k1;
    double k2;
    double t1;
    double t2;
    double k3;
};

struct intrinsicParams
{
    yarp::sig::Matrix retificationMatrix;
    double            principalPointX;
    double            principalPointY;
    double            focalLenghtX;
    double            focalLenghtY;
    plum_bob          distortionModel;
};

struct RGBDParam
{
    RGBDParam() : name("unknown"), isSetting(false), isDescription(false), size(1)
    {
        val.resize(size);
    }

    std::string  name;
    bool         isSetting;
    bool         isDescription;
    int          size;

    std::vector<yarp::os::Value> val;
};

struct CameraParameters
{
    RGBDParam               clipPlanes;
    RGBDParam               accuracy;
    RGBDParam               depthRes;
    RGBDParam               depth_Fov;
    intrinsicParams         depthIntrinsic;

    RGBDParam               rgbRes;
    RGBDParam               rgb_Fov;
    RGBDParam               mirroring;
    intrinsicParams         rgbIntrinsic;
    yarp::sig::Matrix       transformationMatrix;
};

class streamFrameListener : public openni::VideoStream::NewFrameListener
{
public:

    //Properties
    yarp::os::Mutex         mutex;
    yarp::os::Stamp         stamp;
    yarp::sig::FlexImage    image;
    openni::PixelFormat     pixF;
    int                     w, h;
    size_t                  dataSize;
    bool                    isReady;

    //Method
    streamFrameListener();
    bool isValid(){return frameRef.isValid() & isReady;}
    void destroy(){frameRef.release();}

private:
    virtual void onNewFrame(openni::VideoStream& stream);
    openni::VideoFrameRef   frameRef;
};

class yarp::dev::depthCameraDriver : public yarp::dev::DeviceDriver,
                                     public yarp::dev::IRGBDSensor,
                                     public yarp::dev::IFrameGrabberControls2
{
    typedef yarp::sig::ImageOf<yarp::sig::PixelFloat> depthImage;
    typedef yarp::os::Stamp                           Stamp;
    typedef yarp::os::Property                        Property;
    typedef yarp::sig::FlexImage                      FlexImage;

public:
    depthCameraDriver();
    ~depthCameraDriver();
    static int pixFormatToCode(openni::PixelFormat p);

    // DeviceDriver
    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    // IRGBDSensor
    virtual int    getRgbHeight();
    virtual int    getRgbWidth();
    virtual bool   setRgbResolution(int width, int height);
    virtual bool   getRgbFOV(double& horizontalFov, double& verticalFov);
    virtual bool   setRgbFOV(double horizontalFov, double verticalFov);
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
    virtual bool   getExtrinsicParam(sig::Matrix &extrinsic);
    virtual bool   getRgbImage(FlexImage& rgbImage, Stamp* timeStamp = NULL);
    virtual bool   getDepthImage(depthImage& depthImage, Stamp* timeStamp = NULL);
    virtual bool   getImages(FlexImage& colorFrame, depthImage& depthFrame, Stamp* colorStamp=NULL, Stamp* depthStamp=NULL);

    virtual RGBDSensor_status     getSensorStatus();
    virtual yarp::os::ConstString getLastErrorMsg(Stamp* timeStamp = NULL);

    //IFrameGrabberControls2
    virtual bool   getCameraDescription(CameraDescriptor *camera);
    virtual bool   hasFeature(int feature, bool*   hasFeature);
    virtual bool   setFeature(int feature, double  value);
    virtual bool   getFeature(int feature, double* value);
    virtual bool   setFeature(int feature, double  value1,  double  value2);
    virtual bool   getFeature(int feature, double* value1,  double* value2);
    virtual bool   hasOnOff(  int feature, bool*   HasOnOff);
    virtual bool   setActive( int feature, bool    onoff);
    virtual bool   getActive( int feature, bool*   isActive);
    virtual bool   hasAuto(   int feature, bool*   hasAuto);
    virtual bool   hasManual( int feature, bool*   hasManual);
    virtual bool   hasOnePush(int feature, bool*   hasOnePush);
    virtual bool   setMode(   int feature, FeatureMode mode);
    virtual bool   getMode(   int feature, FeatureMode *mode);
    virtual bool   setOnePush(int feature);



private:
    //method
    inline bool initializeOpeNIDevice();
    inline bool setParams(const os::Bottle& settings, const os::Bottle& description);
    inline bool parseIntrinsic(const yarp::os::Searchable& config, const std::string& groupName, intrinsicParams &params);
    bool        getImage(FlexImage& Frame, Stamp* Stamp, streamFrameListener& sourceFrame);
    bool        getImage(depthImage& Frame, Stamp* Stamp, streamFrameListener& sourceFrame);
    bool        setResolution(int w, int h, openni::VideoStream &stream);
    bool        setFOV(double horizontalFov, double verticalFov, openni::VideoStream &stream);
    bool        setIntrinsic(yarp::os::Property& intrinsic, const intrinsicParams& values);
    bool        checkParam(const os::Bottle& settings, const os::Bottle& description, RGBDParam& param);
    bool        checkParam(const yarp::os::Bottle& input, RGBDParam &param, bool& found);
    void        settingErrorMsg(const std::string& error, bool& ret);

    //properties
    openni::Device            m_device;
    openni::VideoStream       m_depthStream;
    openni::VideoStream       m_imageStream;
    streamFrameListener       m_depthFrame;
    streamFrameListener       m_imageFrame;
    yarp::os::ConstString     m_lastError;
    CameraParameters          m_cameraDescription;

    std::vector<cameraFeature_id_t> m_supportedFeatures;
};
#endif
