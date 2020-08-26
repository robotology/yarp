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
#include <RGBDRosConversionUtils.h>

#include <yarp/os/Node.h>
#include <yarp/os/Subscriber.h>
#include <yarp/rosmsg/sensor_msgs/CameraInfo.h>
#include <yarp/rosmsg/sensor_msgs/Image.h>

#include <yarp/rosmsg/impl/yarpRosHelper.h>

/**
 *  @ingroup dev_impl_media
 *
 * @brief `RGBDFromRosTopic` is a driver for a virtual RGBD device: the data is not originated from a physical sensor but from a ROS topic.
 *
 * This device driver exposes the IRGBDSensor interface to read the images published by a ROS node.
 * See the documentation for more details about each interface.
 *
 * This device is paired with its server called RGBDSensorWrapper to stream the images and perform remote operations.
 *
 * | YARP device name   |
 * |:------------------:|
 * | `RGBDFromRosTopic` |
 *
 *   Parameters used by this device are:
 * | Parameter name               | SubParameter        | Type                | Units          | Default Value                | Required                         | Description                                                                            | Notes                                                                 |
 * |:----------------------------:|:-------------------:|:-------------------:|:--------------:|:----------------------------:|:--------------------------------:|:--------------------------------------------------------------------------------------:|:---------------------------------------------------------------------:|
 * |  verbose                     |      -              | bool                | -              |   false                      |  No                              | Flag for enabling debug prints                                                         |         |
 * |  rgb_data_topic              |      -              | string              | -              | /camera/color/image_raw      |  No                              | The device connects to this ROS topic to get RGB data                                  |         |
 * |  depth_data_topic            |      -              | string              | -              | /camera/depth/image_rect_raw |  No                              | The device connects to this ROS topic to get Depth dat                                 |         |
 * |  rgb_info_topic              |      -              | string              | -              | /camera/color/camera_info    |  No                              | The device connects to this ROS topic to get RGB info/parameters                       |         |
 * |  depth_info_topic            |      -              | string              | -              | /camera/depth/camera_info    |  No                              | The device connects to this ROS topic to get Depth info/parameters                     |         |
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device       RGBDSensorWrapper
 * subdevice    RGBDFromRosTopic
 * period 30
 * name /ROSsensor
 * \endcode
 */

typedef yarp::sig::ImageOf<yarp::sig::PixelFloat> depthImage;

//---------------------------------------------------------------------------------------
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

    /*
    //IFrameGrabberControls
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
    commonImageProcessor*   m_rgb_input_processor = nullptr;
    commonImageProcessor*   m_depth_input_processor = nullptr;
    std::string m_rgb_data_topic_name;
    std::string m_depth_data_topic_name;
    std::string m_rgb_info_topic_name;
    std::string m_depth_info_topic_name;

    yarp::os::Stamp m_rgb_stamp;
    yarp::os::Stamp m_depth_stamp;
    std::string m_lastError;
    bool m_verbose;
    bool m_initialized;
};
#endif
