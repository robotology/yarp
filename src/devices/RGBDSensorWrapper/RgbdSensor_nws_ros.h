/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_RGBDSENSOR_NWS_ROS_H
#define YARP_DEV_RGBDSENSOR_NWS_ROS_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include <yarp/os/Port.h>
#include <yarp/os/Time.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/PeriodicThread.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/IFrameGrabberControls.h>

// ROS stuff
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/rosmsg/TickTime.h>
#include <yarp/rosmsg/sensor_msgs/CameraInfo.h>
#include <yarp/rosmsg/sensor_msgs/Image.h>

#define DEFAULT_THREAD_PERIOD   0.03 // s

namespace RGBDImpl
{
    const std::string frameId_param            = "frame_Id";
    const std::string nodeName_param           = "nodeName";
    const std::string colorTopicName_param     = "colorTopicName";
    const std::string depthTopicName_param     = "depthTopicName";
    const std::string depthInfoTopicName_param = "depthInfoTopicName";
    const std::string colorInfoTopicName_param = "colorInfoTopicName";
}

/**
 *  @ingroup dev_impl_nws_ros
 *
 * \section RgbdSensor_nws_ros_device_parameters Description of input parameters
 * \brief `rgbdSensor_nws_ros`: A Network grabber for kinect-like devices.
 * This device will produce two streams of data through different ports, one for the color frame and the other one
 * for depth image following Framegrabber and IDepthSensor interfaces specification respectively.
 * See they documentation for more details about each interface.
 *
 * This device is paired with its client called RgbdSensor_nws_ros to receive the data streams.
 *
 *   Parameters required by this device are:
 * | Parameter name         | SubParameter            | Type    | Units          | Default Value | Required                        | Description                                                                                         | Notes |
 * |:----------------------:|:-----------------------:|:-------:|:--------------:|:-------------:|:------------------------------: |:---------------------------------------------------------------------------------------------------:|:-----:|
 * | period                 |      -                  | double  | s              |   0.03        |  No                             | refresh period of the broadcasted values in s                                                       | default 0.03s |
 * | subdevice              |      -                  | string  |  -             |   -           |  alternative to 'attach' action | name of the subdevice to use as a data source                                                       | when used, parameters for the subdevice must be provided as well |
 * | forceInfoSync          |      -                  | string  | bool           |   -           |  no                             | set 'true' to force the timestamp on the camera_info message to match the image one                 |  - |
 * | color_topic_name       |      -                  | string  | -              |   -           |  Yes                            | the color topic                                                                                     | recommended value /camera/color/image_rect_color  |
 * | depth_topic_name       |      -                  | string  | -              |   -           |  Yes                            | the depth topic                                                                                     | recommended value /camera/depth/image_rect  |
 * | color_frame_id         |      -                  | string  |  -             |   -           |  Yes                            | set the name of the reference frame for the color camera                                            |                               |
 * | depth_frame_id         |      -                  | string  |  -             |   -           |  Yes                            | set the name of the reference frame for the depth camera                                            |                               |
 * | node_name              |      -                  | string  |  -             |   -           |  Yes                            | set the name for ROS node                                                                           | must start with a leading '/' |
 *
 * ROS message type used is sensor_msgs/Image.msg ( http://docs.ros.org/api/sensor_msgs/html/msg/Image.html)
 * Some example of configuration files:
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device rgbdSensor_nws_ros
 * subdevice <RGBDsensor>
 * period 30
 * color_topic_name /<robotName>/camera/color/image_raw
 * depth_topic_name /<robotName>/camera/depth/image_raw
 * color_frame_id rgbd_color_frame
 * depth_frame_id rgbd_depth_frame
 * node_name rgbdsensor
 * \endcode
 */

class RgbdSensor_nws_ros :
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PeriodicThread
{
private:
    typedef yarp::sig::ImageOf<yarp::sig::PixelFloat>    DepthImage;
    typedef yarp::os::Publisher<yarp::rosmsg::sensor_msgs::Image>       ImageTopicType;
    typedef yarp::os::Publisher<yarp::rosmsg::sensor_msgs::CameraInfo>  DepthTopicType;
    typedef unsigned int                                 UInt;

    enum SensorType{COLOR_SENSOR, DEPTH_SENSOR};

    template <class T>
    struct param
    {
        param(T& inVar, std::string inName)
        {
            var          = &inVar;
            parname      = inName;
        }
        T*              var;
        std::string     parname;
    };

    ImageTopicType        publisherPort_color;
    ImageTopicType        publisherPort_depth;
    DepthTopicType        publisherPort_colorCaminfo;
    DepthTopicType        publisherPort_depthCaminfo;
    yarp::os::Node*       m_node;
    std::string           nodeName;
    std::string           m_color_frame_id;
    std::string           m_depth_frame_id;
    yarp::sig::FlexImage  colorImage;
    DepthImage            depthImage;
    UInt                  nodeSeq;

    // Image data specs
    // int hDim, vDim;
    double                         period;
    std::string                    sensorId;
    yarp::dev::IRGBDSensor*        sensor_p;
    yarp::dev::IFrameGrabberControls* fgCtrl;
    yarp::dev::IRGBDSensor::RGBDSensor_status sensorStatus;
    int                            verbose;
    bool                           forceInfoSync;
    bool                           initialize_ROS(yarp::os::Searchable& config);

    // Open the wrapper only, the attach method needs to be called before using it
    // Typical usage: yarprobotinterface
    bool                           openDeferredAttach(yarp::os::Searchable& prop);

    // If a subdevice parameter is given, the wrapper will open it and attach to immediately.
    // Typical usage: simulator or command line
    bool                           isSubdeviceOwned;
    yarp::dev::PolyDriver*         subDeviceOwned;
    bool                           openAndAttachSubDevice(yarp::os::Searchable& prop);

    // Synch
    yarp::os::Stamp                colorStamp;
    yarp::os::Stamp                depthStamp;

    bool writeData();
    bool setCamInfo(yarp::rosmsg::sensor_msgs::CameraInfo& cameraInfo,
                    const std::string&                     frame_id,
                    const UInt&                            seq,
                    const SensorType&                      sensorType);

public:
    RgbdSensor_nws_ros();
    RgbdSensor_nws_ros(const RgbdSensor_nws_ros&) = delete;
    RgbdSensor_nws_ros(RgbdSensor_nws_ros&&) = delete;
    RgbdSensor_nws_ros& operator=(const RgbdSensor_nws_ros&) = delete;
    RgbdSensor_nws_ros& operator=(RgbdSensor_nws_ros&&) = delete;
    ~RgbdSensor_nws_ros() override;

    bool        open(yarp::os::Searchable &params) override;
    bool        close() override;

    void        setId(const std::string &id);
    std::string getId();

    /**
      * Specify which sensor this thread has to read from.
      */
    bool        attach(yarp::dev::PolyDriver *poly) override;
    bool        detach() override;

    bool        threadInit() override;
    void        threadRelease() override;
    void        run() override;
};

#endif   // YARP_DEV_RGBDSENSOR_NWS_ROS_H
