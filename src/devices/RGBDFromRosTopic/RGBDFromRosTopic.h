/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef RGBD_FROM_ROS_TOPIC_H
#define RGBD_FROM_ROS_TOPIC_H

#include <iostream>
#include <cstring>
#include <map>
#include <mutex>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/FrameGrabberControl2.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Property.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/RGBDSensorParamParser.h>

#include <yarp/os/Node.h>
#include <yarp/os/Subscriber.h>
#include <yarp/rosmsg/sensor_msgs/CameraInfo.h>
#include <yarp/rosmsg/sensor_msgs/Image.h>

#include <yarp/rosmsg/impl/yarpRosHelper.h>

/**
 *  @ingroup dev_impl_media
 *
 * @brief `RGBDFromRosTopic` : driver for realsense2 compatible devices.
 *
 * This device driver exposes the IRGBDSensor and IFrameGrabberControls
 * interfaces to read the images and operate on the available settings.
 * See the documentation for more details about each interface.
 *
 * This device is paired with its server called RGBDSensorWrapper to stream the images and perform remote operations.
 *
 * The configuration file is subdivided into 2 major sections called "SETTINGS" and "HW_DESCRIPTION".
 *
 * The "SETTINGS" section is meant for read/write parameters, meaning parameters which can be get and set by the device.
 * A common case of setting is the image resolution in pixel. This setting will be read by the device and it'll be applied
 * in the startup phase. If the setting fails, the device will terminate the execution with a error message.
 *
 * The "HW_DESCRIPTION" section is meant for read only parameters which describe the hardware property of the device and
 * cannot be provided by the device through software API.
 * A common case is the 'Field Of View' property, which may or may not be supported by the physical device.
 * When a property is present in the HW_DESCRIPTION group, the YARP RGBDSensorClient will report this value when asked for
 * and setting will be disabled.
 * This group can also be used to by-pass realsense2 API in case some functionality is not correctly working with the current
 * device. For example the 'clipPlanes' property may return incorrect values or values using non-standard measurement unit.
 * In this case using the HW_DESCRIPTION, a user can override the value got from OpenNI2 API with a custom value.
 *
 * \note Parameters inside the HW_DESCRIPTION are read only, so the relative set functionality will be disabled.
 * \note For parameters which are neither in SETTINGS nor in HW_DESCRIPTION groups, read / write functionality is assumed
 *  but not initial setting will be performed. Device will start with manufacturer default values.
 * \warning A single parameter cannot be present into both SETTINGS and HW_DESCRIPTION groups.
 * \warning whenever more then one value is required by the setting, the values must be in parentheses!
 *
 * | YARP device name |
 * |:-----------------:|
 * | `RGBDFromRosTopic` |
 *
 *   Parameters used by this device are:
 * | Parameter name               | SubParameter        | Type                |  Read / write   | Units          | Default Value | Required                         | Description                                                                            | Notes                                                                 |
 * |:----------------------------:|:-------------------:|:-------------------:|:---------------:|:--------------:|:-------------:|:--------------------------------:|:--------------------------------------------------------------------------------------:|:---------------------------------------------------------------------:|
 * |  verbose                     |      -              | bool                |  Read / write   |                |   false       |  No                              | Flag for enabling debug prints                                                         |                                                                       |
 * |  SETTINGS                    |      -              | group               |  Read / write   | -              |   -           |  Yes                             | Initial setting of the device.                                                         |  Properties must be read/writable in order for setting to work        |
 * |                              |  rgbResolution      | int, int            |  Read / write   | pixels         |   -           |  Yes                             | Size of rgb image in pixels                                                            |  2 values expected as height, width                                   |
 * |                              |  depthResolution    | int, int            |  Read / write   | pixels         |   -           |  Yes                             | Size of depth image in pixels                                                          |  Values are height, width                                             |
 * |                              |  accuracy           | double              |  Read / write   | meters         |   -           |  No                              | Accuracy of the device, as the depth measurement error at 1 meter distance             |  Note that only few realsense devices allows to set it                |
 * |                              |  framerate          | int                 |  Read / Write   | fps            |   30          |  No                              | Framerate of the sensor                                                                |                                                                       |
 * |                              |  alignmentFrame     | string              |  Read / Write   | -              |   RGB         |  No                              | This parameter specifies the frame to which the frames RGB and Depth will be aligned.  |  The accepted values are RGB, Depth, None. This operation could be heavy, set it to None to increase the fps.|
 *
 * Configuration file using .ini format, for using as RGBD device:
 *
 * \code{.unparsed}

device       RGBDSensorWrapper
subdevice    realsense2
name         /depthCamera

[SETTINGS]
depthResolution (640 480)    #Note the parentheses
rgbResolution   (640 480)
framerate       30
enableEmitter   true
alignmentFrame  RGB

 *
 * \endcode
 *
 * Configuration file using .ini format, for using as stereo camera:
 * \code{.unparsed}
device       grabberDual
subdevice    realsense2
name         /stereoCamera
capabilities RAW
stereoMode   true

[SETTINGS]
rgbResolution   (640 480)
depthResolution   (640 480)
framerate       30
enableEmitter   false

 * \endcode
 */

class colorImageInputProcessor :
    public yarp::os::Subscriber<yarp::rosmsg::sensor_msgs::Image>
{
    std::mutex             m_port_mutex;
    yarp::sig::FlexImage   m_lastRGBImage;
    yarp::os::Stamp        m_lastStamp;
    bool                   m_contains_data;
    yarp::os::Subscriber <yarp::rosmsg::sensor_msgs::CameraInfo> m_subscriber_camera_info;
    yarp::rosmsg::sensor_msgs::CameraInfo m_lastCameraInfo;

public:
//    colorImageInputProcessor(const colorImageInputProcessor& alt) { m_lastRGBImage = alt.m_lastRGBImage; m_lastStamp = alt.m_lastStamp; m_contains_data = alt.m_contains_data; }
    colorImageInputProcessor();
    using yarp::os::Subscriber<yarp::rosmsg::sensor_msgs::Image>::onRead;
    virtual void onRead(yarp::rosmsg::sensor_msgs::Image& v) override;
    void getLastData(yarp::sig::FlexImage& data, yarp::os::Stamp& stmp);
    size_t getWidth() const;
    size_t getHeight() const;
    bool getFOV(double& horizontalFov, double& verticalFov) const;
    bool getIntrinsicParam(yarp::os::Property& intrinsic) const;
};

typedef yarp::sig::ImageOf<yarp::sig::PixelFloat> depthImage;

class depthImageInputProcessor : public yarp::os::Subscriber<yarp::rosmsg::sensor_msgs::Image>
{
    std::mutex             m_port_mutex;
    yarp::sig::ImageOf<yarp::sig::PixelFloat>   m_lastDepthImage;
    yarp::os::Stamp        m_lastStamp;
    bool                   m_contains_data;
    yarp::os::Subscriber <yarp::rosmsg::sensor_msgs::CameraInfo> m_subscriber_camera_info;
    yarp::rosmsg::sensor_msgs::CameraInfo m_lastCameraInfo;

public:
//    depthImageInputProcessor(const depthImageInputProcessor& alt) { m_lastDepthImage = alt.m_lastDepthImage; m_lastStamp = alt.m_lastStamp; m_contains_data = alt.m_contains_data; }
    depthImageInputProcessor();
    using yarp::os::Subscriber<yarp::rosmsg::sensor_msgs::Image>::onRead;
    virtual void onRead(yarp::rosmsg::sensor_msgs::Image& v) override;
    void getLastData(depthImage& data, yarp::os::Stamp& stmp);
    size_t getWidth() const;
    size_t getHeight() const;
    bool getFOV(double& horizontalFov, double& verticalFov) const;
    bool getIntrinsicParam(yarp::os::Property& intrinsic) const;
};

class RGBDFromRosTopic :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IRGBDSensor
{
private:
    typedef yarp::os::Stamp                           Stamp;
    typedef yarp::os::Property                        Property;
    typedef yarp::sig::FlexImage                      FlexImage;

public:
    RGBDFromRosTopic();
    ~RGBDFromRosTopic() override = default;

    // DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // IRGBDSensor
    int    getRgbHeight() override;
    int    getRgbWidth() override;
    bool   getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig> &configurations) override;
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


    bool   getExtrinsicParam(yarp::sig::Matrix &extrinsic) override;
    bool   getRgbImage(FlexImage& rgbImage, Stamp* timeStamp = nullptr) override;
    bool   getDepthImage(depthImage& depthImage, Stamp* timeStamp = nullptr) override;
    bool   getImages(FlexImage& colorFrame, depthImage& depthFrame, Stamp* colorStamp=NULL, Stamp* depthStamp=NULL) override;

    RGBDSensor_status     getSensorStatus() override;
    std::string getLastErrorMsg(Stamp* timeStamp = NULL) override;

    //IFrameGrabberControls
    /*
    bool   getCameraDescription(CameraDescriptor *camera) override;
    bool   hasFeature(int feature, bool*   hasFeature) override;
    bool   setFeature(int feature, double  value) override;
    bool   getFeature(int feature, double* value) override;
    bool   setFeature(int feature, double  value1,  double  value2) override;
    bool   getFeature(int feature, double* value1,  double* value2) override;
    bool   hasOnOff(  int feature, bool*   HasOnOff) override;
    bool   setActive( int feature, bool    onoff) override;
    bool   getActive( int feature, bool*   isActive) override;
    bool   hasAuto(   int feature, bool*   hasAuto) override;
    bool   hasManual( int feature, bool*   hasManual) override;
    bool   hasOnePush(int feature, bool*   hasOnePush) override;
    bool   setMode(   int feature, FeatureMode mode) override;
    bool   getMode(   int feature, FeatureMode *mode) override;
    bool   setOnePush(int feature) override;
    */

    // ros-topic related
    mutable std::mutex m_mutex;
    yarp::os::Node* m_ros_node = nullptr;
    colorImageInputProcessor    m_rgb_input_processor;
    depthImageInputProcessor    m_depth_input_processor;
    std::string m_rgb_topic_name;
    std::string m_depth_topic_name;

    yarp::os::Stamp m_rgb_stamp;
    yarp::os::Stamp m_depth_stamp;
    std::string m_lastError;
    yarp::dev::RGBDSensorParamParser m_paramParser;
    bool m_verbose;
    bool m_initialized;
    int  m_fps;
};
#endif
