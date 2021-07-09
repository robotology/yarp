/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RGBD_FROM_ROS_TOPIC_H
#define RGBD_FROM_ROS_TOPIC_H

#include <iostream>
#include <cstring>
#include <map>
#include <mutex>

#include <yarp/dev/DeviceDriver.h>
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

typedef yarp::sig::ImageOf<yarp::sig::PixelFloat> depthImage;

/**
 *  @ingroup dev_impl_media
 *
 * @brief `RGBDSensorFromRosTopic` is a driver for a virtual RGBD device: the data is not originated from a physical sensor but from a ROS topic.
 *
 * This device driver exposes the IRGBDSensor interface to read the images published by a ROS node.
 * See the documentation for more details about each interface.
 *
 * This device can be used in two different ways:
 * - as a client, directly opened by your application
 * - as a device, wrapped by a RGBDSensorWrapper
 *
 * When used as a client, the RGBDSensorFromRosTopic device directly connects to the ROS publisher via `tcp_ros` carrier.
 * This mode does not use a RGBDSensorWrapper and thus minimize latency. It is thus recommended when the application consists of
 * a single client. However, if multiple clients are involved, as shown in the diagram below, this architecture might be inefficient,
 * due to the multiple tcp_ros connections.
 *
 * \dot
 * digraph G {
 *
 *   subgraph cluster_4 {
 *     node [style=filled,color=black,fillcolor=white];
 *     label = "ROS node";
 *     subgraph cluster_5 {
 *        node [style=filled,color=black,fillcolor=white];
 *        label = "";
 *        SENSOR [label = "physical sensor"]
 *        bgcolor=palegreen3
 *        }
 *     ROS [label=<<B>ROS</B> <BR/> /camera/color/image_raw <BR/> /camera/color/camera_info <BR/> /camera/depth/image_rect_raw <BR/> /camera/depth/camera_info>];
 *     bgcolor=palegreen1
 *   }
 *
 *   subgraph cluster_0 {
 *     node [style=filled,color=black,fillcolor=white];
 *     node [style=filled,color=black,fillcolor=white];
 *     c1 [label = <<B>RGBDSensorFromRosTopic</B> <BR/> /camera/color/image_raw <BR/> /camera/color/camera_info <BR/> /camera/depth/image_rect_raw <BR/> /camera/depth/camera_info> ];
 *     label = "process1";
 *     bgcolor=paleturquoise1
 *   }
 *
 *   subgraph cluster_1 {
 *     node [style=filled,color=black,fillcolor=white];
 *     c2 [label = <<B>RGBDSensorFromRosTopic</B> <BR/> /camera/color/image_raw <BR/> /camera/color/camera_info <BR/> /camera/depth/image_rect_raw <BR/> /camera/depth/camera_info> ];
 *     label = "process2";
 *     bgcolor=paleturquoise1
 *   }
 *
 *     subgraph cluster_2 {
 *     node [style=filled,color=black,fillcolor=white];
 *     c3 [label = <<B>RGBDSensorFromRosTopic</B> <BR/> /camera/color/image_raw <BR/> /camera/color/camera_info <BR/> /camera/depth/image_rect_raw <BR/> /camera/depth/camera_info> ];
 *     label = "process3";
 *    bgcolor=paleturquoise1
 *   }
 *
 *   SENSOR -> ROS [ style="dashed" dir="none"]
 *   ROS -> c1 [ label=<<B>tcp_ros</B>> fontcolor="red"  ];
 *   ROS ->  c2 [ label=<<B>tcp_ros</B>> fontcolor="red"  ];
 *   ROS ->  c3 [ label=<<B>tcp_ros</B>> fontcolor="red"  ];
 * }
 *  \enddot
 *
 *
 * The second mode, instead, is  useful if the application consists of several modules, and each of them
 * employ a client connected to the same server (RGBDSensorWrapper). In this case, if all the modules run
 * on the same machine, the `unix_stream` can be employed to share data between them, minimizing data transfer
 * operations and greatly boosting the performance. See the digram below.
 *
 * \dot
 * digraph G {
 *
 *  subgraph cluster_4 {
 *    node [style=filled,color=black,fillcolor=white];
 *    label = "ROS node";
 *    subgraph cluster_5 {
 *       node [style=filled,color=black,fillcolor=white];
 *       label = "";
 *       SENSOR [label = "physical sensor"]
 *       bgcolor=palegreen3
 *       }
 *    ROS [label=<<B>ROS</B> <BR/> /camera/color/image_raw <BR/> /camera/color/camera_info <BR/> /camera/depth/image_rect_raw <BR/> /camera/depth/camera_info>];
 *    bgcolor=palegreen1
 *  }
 *
 *  subgraph cluster_0 {
 *    node [style=filled,color=black,fillcolor=white];
 *    node [style=filled,color=black,fillcolor=white];
 *    c1 [label=<<B>RGBDSensorClient</B> <BR/> /client1/rgbImage:i <BR/> /client1/depthImage:i>];
 *    label = "process1";
 *    bgcolor=paleturquoise1
 *  }
 *
 *  subgraph cluster_1 {
 *    node [style=filled,color=black,fillcolor=white];
 *    c2 [label=<<B>RGBDSensorClient</B> <BR/> /client2/rgbImage:i <BR/> /client2/depthImage:i>];
 *    label = "process2";
 *    bgcolor=paleturquoise1
 *  }
 *
 *    subgraph cluster_2 {
 *    node [style=filled,color=black,fillcolor=white];
 *    c3 [label=<<B>RGBDSensorClient</B>  <BR/> /client3/rgbImage:i <BR/> /client3/depthImage:i>];
 *    label = "process3";
 *   bgcolor=paleturquoise1
 *  }
 *
 *  subgraph cluster_3 {
 *    node [style=filled,fillcolor=blue, color=black];
 *    label = "yarpdev process";
 *    bgcolor=paleturquoise1
 *    subgraph cluster_3 {
 *       node [style=filled, fillcolor=white, color=black];
 *       label = "RGBDSensorWrapper device";
 *       bgcolor=paleturquoise3
 *       subgraph cluster_3 {
 *           node [style=filled, fillcolor=white, color=black];
 *           label = "RGBDSensorFromRosTopic device";
 *           bgcolor=paleturquoise2
 *           rgbtopic [label = <<B>RGBDSensorFromRosTopic</B> <BR/> /camera/color/image_raw <BR/> /camera/color/camera_info <BR/> /camera/depth/image_rect_raw <BR/> /camera/depth/camera_info> ]}
 *           w_out [label=<<B>Wrapper</B> <BR/> /wrapper/rgbImage:o <BR/> /wrapper/depthImage:o>];
 *      }
 *  }
 *
 *  SENSOR -> ROS [ style="dashed" dir="none"]
 *  rgbtopic -> w_out [ style="dashed" dir="none"]
 *  ROS-> rgbtopic  [ label=<<B>tcp_ros</B>> fontcolor="red"  ];
 *  w_out -> c1 [ label=<<B>unix_stream</B>> fontcolor="blue"];
 *  w_out ->  c2 [ label=<<B>unix_stream</B>> fontcolor="blue" ];
 *  w_out ->  c3 [ label=<<B>unix_stream</B>> fontcolor="blue" ];
 *  }
 *  \enddot
 *
 * | YARP device name   |
 * |:------------------:|
 * | `RGBDSensorFromRosTopic` |
 *
 *   Parameters used by this device are:
 * | Parameter name               | SubParameter        | Type                | Units          | Default Value | Required  | Description                                                                                          | Notes   |
 * |:----------------------------:|:-------------------:|:-------------------:|:--------------:|:-------------:|:---------:|:----------------------------------------------------------------------------------------------------:|:-------:|
 * |  color_topic_name       |      -              | string              | -              | -             |  Yes       | The device connects to this ROS topic to get RGB data (there must be also camera_info with the last subtopic)|         |
 * |  depth_topic_name       |      -              | string              | -              | -             |  Yes       | The device connects to this ROS topic to get Depth data (there must be also camera_info with the last subtopic)    |         | * |  node_name                   |      -              | string              | -              | -             |  Yes       | the name of the ros node                                                                             |         |
 * |  node_name              |      -              | string              | -              | -             |  Yes       | the name of the ros node    |         | * |  node_name                   |      -              | string              | -              | -             |  Yes       | the name of the ros node                                                                             |         |
 *
 * Example of configuration file (using .ini format) when the device is wrapped by RGBDSensorWrapper.
 *
 * \code{.unparsed}
 * device       RGBDSensorWrapper
 * subdevice    RGBDSensorFromRosTopic
 * color_topic_name    /camera/color/image_raw
 * depth_topic_name    /camera/depth/image_raw
 * node_name    RGBDSensorFromRosTopic
 * \endcode
 */

class RGBDSensorFromRosTopic :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IRGBDSensor
{
private:
    typedef yarp::os::Stamp                           Stamp;
    typedef yarp::os::Property                        Property;
    typedef yarp::sig::FlexImage                      FlexImage;

public:
    ~RGBDSensorFromRosTopic() override = default;

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
    bool   getImages(FlexImage& colorFrame, depthImage& depthFrame, Stamp* colorStamp = nullptr, Stamp* depthStamp = nullptr) override;

    RGBDSensor_status     getSensorStatus() override;
    std::string getLastErrorMsg(Stamp* timeStamp = nullptr) override;

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
    yarp::dev::RGBDRosConversionUtils::commonImageProcessor*   m_rgb_input_processor = nullptr;
    yarp::dev::RGBDRosConversionUtils::commonImageProcessor*   m_depth_input_processor = nullptr;

    std::string m_lastError;
};
#endif
