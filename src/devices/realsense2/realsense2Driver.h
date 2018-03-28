/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#ifndef REALSENSE2_DRIVER_H
#define REALSENSE2_DRIVER_H

#ifdef WIN32
#define _USE_MATH_DEFINES
#endif

#include <iostream>
#include <cstring>
#include <map>
#include <mutex>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/FrameGrabberControl2.h>
#include <yarp/os/RateThread.h>
#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>
#include <yarp/os/all.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/RGBDSensorParamParser.h>
#include <librealsense2/rs.hpp>

constexpr double RAD2DEG (180.0 / M_PI);

constexpr double DEG2RAD (M_PI / 180.0);

namespace yarp
{
    namespace dev
    {
        class realsense2Driver;
    }
}


class yarp::dev::realsense2Driver :  public yarp::dev::DeviceDriver,
                                     public yarp::dev::IRGBDSensor,
                                     public yarp::dev::IFrameGrabberControls2
{
private:
    typedef yarp::sig::ImageOf<yarp::sig::PixelFloat> depthImage;
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    typedef yarp::os::Stamp                           Stamp;
    typedef yarp::os::Property                        Property;
    typedef yarp::sig::FlexImage                      FlexImage;

#endif
public:
    realsense2Driver();
    ~realsense2Driver();

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
    virtual bool   getRgbImage(FlexImage& rgbImage, Stamp* timeStamp = nullptr) override;
    virtual bool   getDepthImage(depthImage& depthImage, Stamp* timeStamp = nullptr) override;
    virtual bool   getImages(FlexImage& colorFrame, depthImage& depthFrame, Stamp* colorStamp=NULL, Stamp* depthStamp=NULL) override;

    virtual RGBDSensor_status     getSensorStatus() override;
    virtual yarp::os::ConstString getLastErrorMsg(Stamp* timeStamp = NULL) override;

    //IFrameGrabberControls2
    virtual bool   getCameraDescription(CameraDescriptor *camera) override;
    virtual bool   hasFeature(int feature, bool*   hasFeature) override;
    virtual bool   setFeature(int feature, double  value) override;
    virtual bool   getFeature(int feature, double* value) override;
    virtual bool   setFeature(int feature, double  value1,  double  value2) override;
    virtual bool   getFeature(int feature, double* value1,  double* value2) override;
    virtual bool   hasOnOff(  int feature, bool*   HasOnOff) override;
    virtual bool   setActive( int feature, bool    onoff) override;
    virtual bool   getActive( int feature, bool*   isActive) override;
    virtual bool   hasAuto(   int feature, bool*   hasAuto) override;
    virtual bool   hasManual( int feature, bool*   hasManual) override;
    virtual bool   hasOnePush(int feature, bool*   hasOnePush) override;
    virtual bool   setMode(   int feature, FeatureMode mode) override;
    virtual bool   getMode(   int feature, FeatureMode *mode) override;
    virtual bool   setOnePush(int feature) override;

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    //method
    inline bool initializeRealsenseDevice();
    inline bool setParams();

    bool        getImage(FlexImage& Frame, Stamp* timeStamp, rs2::frameset& sourceFrame);
    bool        getImage(depthImage& Frame, Stamp* timeStamp, const rs2::frameset& sourceFrame);
    bool        setIntrinsic(yarp::os::Property& intrinsic, const rs2_intrinsics& values);
    bool        setExtrinsicParam(yarp::sig::Matrix& extrinsic, const rs2_extrinsics& values);
    void        settingErrorMsg(const std::string& error, bool& ret);
    void        updateTransformations();
    bool        pipelineStartup();
    bool        pipelineShutdown();


    // realsense classes
    std::mutex   m_mutex;
    rs2::context m_ctx;
    rs2::config m_cfg;
    rs2::pipeline m_pipeline;
    rs2::device  m_device;
    std::vector<rs2::sensor> m_sensors;
    rs2::sensor* m_depth_sensor;
    rs2::sensor* m_color_sensor;
    rs2_intrinsics m_depth_intrin, m_color_intrin;
    rs2_extrinsics m_depth_to_color, m_color_to_depth;


    yarp::os::Stamp m_rgb_stamp;
    yarp::os::Stamp m_depth_stamp;
    yarp::os::ConstString m_lastError;
    yarp::dev::RGBDSensorParamParser* m_paramParser;
    bool m_depthRegistration;
    bool m_verbose;
    bool m_initialized;
    int m_period;
    std::vector<cameraFeature_id_t> m_supportedFeatures;
    std::map<std::string, RGBDSensorParamParser::RGBDParam> m_params_map;
#endif
};
#endif
