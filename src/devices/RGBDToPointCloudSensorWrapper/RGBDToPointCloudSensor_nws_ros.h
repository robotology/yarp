/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_RGBDTOPOINTCLOUDSENSOR_NWS_ROS_H
#define YARP_DEV_RGBDTOPOINTCLOUDSENSOR_NWS_ROS_H

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
#include <yarp/rosmsg/sensor_msgs/PointCloud2.h>

constexpr double DEFAULT_THREAD_PERIOD = 0.033; // s

namespace RGBDToPointCloudImpl{
    const std::string frameId_param                 = "frame_Id";
    const std::string nodeName_param                = "nodeName";
    const std::string pointCloudTopicName_param     = "pointCloudTopicName";
}
/**
 *  @ingroup dev_impl_nws_ros
 *
 * \section RGBDToPointCloudSensor_nws_ros_device_parameters Description of input parameters
 *
 * \brief `RGBDToPointCloudSensor_nws_ros`: A Network grabber for kinect-like devices.
 * This device will produce one stream of data for the point cloud
 * derived fron the combination of the data derived from Framegrabber and IDepthSensor interfaces.
 * See they documentation for more details about each interface.
 *
 *   Parameters required by this device are:
 * | Parameter name         | SubParameter            | Type    | Units          | Default Value | Required                        | Description                                                                                         | Notes |
 * |:----------------------:|:-----------------------:|:-------:|:--------------:|:-------------:|:------------------------------: |:---------------------------------------------------------------------------------------------------:|:-----:|
 * | period                 |      -                  | double  |  s             |   0.033       |  No                             | refresh period of the broadcasted values in s                                                       | default 0.033 s |
 * | subdevice              |      -                  | string  |  -             |   -           |  alternative to 'attach' action | name of the subdevice to use as a data source                                                       | when used, parameters for the subdevice must be provided as well |
 * | topic_name             |      -                  | string  |  -             |               |  Yes                            | set the name for ROS point cloud topic                                                              | must start with a leading '/' |
 * | frame_id               |      -                  | string  |  -             |               |  Yes                            | set the name of the reference frame                                                                 |                               |
 * | node_name              |      -                  | string  |  -             |   -           |  Yes                            | set the name for ROS node                                                                           | must start with a leading '/' |
 *
 * ROS message type used is sensor_msgs/Image.msg ( http://docs.ros.org/en/api/sensor_msgs/html/msg/PointCloud2.html)
 * Some example of configuration files:
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device RGBDToPointCloudSensor_nws_ros
 * subdevice <RGBDsensor>
 * period 30
 * topic_name /camera/points
 * frame_id depth_center
 * node_name /<robotName>/RGBDToPointCloudSensorNode
 * \endcode
 */

class RGBDToPointCloudSensor_nws_ros :
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PeriodicThread
{
private:
    // defining types for shorter names
    typedef yarp::sig::ImageOf<yarp::sig::PixelFloat>                   DepthImage;
    typedef yarp::os::Publisher<yarp::rosmsg::sensor_msgs::PointCloud2> PointCloudTopicType;
    typedef yarp::rosmsg::sensor_msgs::PointCloud2                      PointCloud2Type;
    typedef unsigned int                                                UInt;

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

    PointCloudTopicType   publisherPort_pointCloud;
    yarp::os::Node*       m_node = nullptr;
    std::string           nodeName;
    std::string           pointCloudTopicName;
    std::string           frameId;

    // images from device
    yarp::sig::FlexImage  colorImage;
    DepthImage            depthImage;
    UInt                  nodeSeq = 0;


    // this is the sub device or the real device

    double                                      period = DEFAULT_THREAD_PERIOD;
    std::string                                 sensorId;
    yarp::dev::IRGBDSensor*                     sensor_p = nullptr;
    yarp::dev::IFrameGrabberControls*           fgCtrl = nullptr;
    yarp::dev::IRGBDSensor::RGBDSensor_status   sensorStatus = yarp::dev::IRGBDSensor::RGBD_SENSOR_NOT_READY;

    int         verbose = 4;
    bool        forceInfoSync = true;
    bool        initialize_ROS(yarp::os::Searchable& config);

    // Open the wrapper only, the attach method needs to be called before using it
    // Typical usage: yarprobotinterface
    bool                           openDeferredAttach(yarp::os::Searchable& prop);

    // If a subdevice parameter is given, the wrapper will open it and attach to immediately.
    // Typical usage: simulator or command line
    bool                           isSubdeviceOwned = false;
    yarp::dev::PolyDriver*         subDeviceOwned = nullptr;
    bool                           openAndAttachSubDevice(yarp::os::Searchable& prop);

    // Synch
    yarp::os::Stamp                colorStamp;
    yarp::os::Stamp                depthStamp;
    yarp::os::Property             m_conf;

    bool writeData();

public:
    RGBDToPointCloudSensor_nws_ros();
    RGBDToPointCloudSensor_nws_ros(const RGBDToPointCloudSensor_nws_ros&) = delete;
    RGBDToPointCloudSensor_nws_ros(RGBDToPointCloudSensor_nws_ros&&) = delete;
    RGBDToPointCloudSensor_nws_ros& operator=(const RGBDToPointCloudSensor_nws_ros&) = delete;
    RGBDToPointCloudSensor_nws_ros& operator=(RGBDToPointCloudSensor_nws_ros&&) = delete;
    ~RGBDToPointCloudSensor_nws_ros() override;

    bool        open(yarp::os::Searchable &params) override;
    bool        fromConfig(yarp::os::Searchable &params);
    bool        close() override;

    /**
      * Specify which sensor this thread has to read from.
      */
    bool        attach(yarp::dev::PolyDriver *poly) override;
    bool        detach() override;

    bool        threadInit() override;
    void        threadRelease() override;
    void        run() override;
};

#endif   // YARP_DEV_RGBDTOPOINTCLOUDSENSOR_NWS_ROS_H
