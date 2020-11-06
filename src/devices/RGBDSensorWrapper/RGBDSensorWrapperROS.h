/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_RGBDSENSORWRAPPER_ROS_H
#define YARP_DEV_RGBDSENSORWRAPPER_ROS_H

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

#include <yarp/dev/IWrapper.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/FrameGrabberControlImpl.h>

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
    const std::string frameId_param            = "ROS_frame_Id";
    const std::string nodeName_param           = "ROS_nodeName";
    const std::string colorTopicName_param     = "ROS_colorTopicName";
    const std::string depthTopicName_param     = "ROS_depthTopicName";
    const std::string depthInfoTopicName_param = "ROS_depthInfoTopicName";
    const std::string colorInfoTopicName_param = "ROS_colorInfoTopicName";
}

/**
 *  @ingroup dev_impl_wrapper
 *
 * \section RGBDSensorWrapperROS_device_parameters Description of input parameters
 * A Network grabber for kinect-like devices.
 * This device will produce two streams of data through different ports, one for the color frame and the other one
 * for depth image following Framegrabber and IDepthSensor interfaces specification respectively.
 * See they documentation for more details about each interface.
 *
 * This device is paired with its client called RGBDSensorWrapperROS to receive the data streams.
 *
 *   Parameters required by this device are:
 * | Parameter name         | SubParameter            | Type    | Units          | Default Value | Required                        | Description                                                                                         | Notes |
 * |:----------------------:|:-----------------------:|:-------:|:--------------:|:-------------:|:------------------------------: |:---------------------------------------------------------------------------------------------------:|:-----:|
 * | period                 |      -                  | int     |  ms            |   20          |  No                             | refresh period of the broadcasted values in ms                                                      | default 20ms |
 * | subdevice              |      -                  | string  |  -             |   -           |  alternative to 'attach' action | name of the subdevice to use as a data source                                                       | when used, parameters for the subdevice must be provided as well |
 * | forceInfoSync          |      -                  | string  | bool           |   -           |  no                             | set 'true' to force the timestamp on the camera_info message to match the image one                 |  - |
 * | ROS_colorTopicName     |      -                  | string  |  -             |   -           |  Yes                            | set the name for ROS image topic                                                                    | must start with a leading '/' |
 * | ROS_depthTopicName     |      -                  | string  |  -             |   -           |  Yes                            | set the name for ROS depth topic                                                                    | must start with a leading '/' |
 * | ROS_colorInfoTopicName |      -                  | string  |  -             |   -           |  Yes                            | set the name for ROS imageInfo topic                                                                | must start with a leading '/' |
 * | ROS_depthInfoTopicName |      -                  | string  |  -             |   -           |  Yes                            | set the name for ROS depthInfo topic                                                                | must start with a leading '/' |
 * | ROS_frame_Id           |      -                  | string  |  -             |               |  Yes                            | set the name of the reference frame                                                                 |                               |
 * | ROS_nodeName           |      -                  | string  |  -             |   -           |  Yes                            | set the name for ROS node                                                                           | must start with a leading '/' |
 *
 * ROS message type used is sensor_msgs/Image.msg ( http://docs.ros.org/api/sensor_msgs/html/msg/Image.html)
 * Some example of configuration files:
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device RGBDSensorWrapperROS
 * subdevice <RGBDsensor>
 * period 30
 * ROS_colorTopicName /<robotName>/RGBDSensorColor
 * ROS_depthTopicName /<robotName>/RGBDSensorDepth
 * ROS_colorInfoTopicName /<robotName>/RGBDSensorColorInfo
 * ROS_depthInfoTopicName /<robotName>/RGBDSensorDepthInfo
 * ROS_frame_Id /<robotName>/<framed_Id>
 * ROS_nodeName /<robotName>/RGBDSensorNode
 * \endcode
 */

class RGBDSensorWrapperROS :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IWrapper,
        public yarp::dev::IMultipleWrapper,
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

    ImageTopicType        rosPublisherPort_color;
    ImageTopicType        rosPublisherPort_depth;
    DepthTopicType        rosPublisherPort_colorCaminfo;
    DepthTopicType        rosPublisherPort_depthCaminfo;
    yarp::os::Node*       rosNode;
    std::string           nodeName;
    std::string           depthTopicName;
    std::string           colorTopicName;
    std::string           dInfoTopicName;
    std::string           cInfoTopicName;
    std::string           rosFrameId;
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
    bool                           read(yarp::os::ConnectionReader& connection);

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
    yarp::os::Property             m_conf;

    bool writeData();
    bool setCamInfo(yarp::rosmsg::sensor_msgs::CameraInfo& cameraInfo,
                    const std::string&                     frame_id,
                    const UInt&                            seq,
                    const SensorType&                      sensorType);

    static std::string yarp2RosPixelCode(int code);

public:
    RGBDSensorWrapperROS();
    RGBDSensorWrapperROS(const RGBDSensorWrapperROS&) = delete;
    RGBDSensorWrapperROS(RGBDSensorWrapperROS&&) = delete;
    RGBDSensorWrapperROS& operator=(const RGBDSensorWrapperROS&) = delete;
    RGBDSensorWrapperROS& operator=(RGBDSensorWrapperROS&&) = delete;
    ~RGBDSensorWrapperROS() override;

    bool        open(yarp::os::Searchable &params) override;
    bool        fromConfig(yarp::os::Searchable &params);
    bool        close() override;

    void        setId(const std::string &id);
    std::string getId();

    /**
      * Specify which sensor this thread has to read from.
      */
    bool        attachAll(const yarp::dev::PolyDriverList &p) override;
    bool        detachAll() override;

    bool        attach(yarp::dev::PolyDriver *poly) override;
    bool        attach(yarp::dev::IRGBDSensor *s);
    bool        detach() override;

    bool        threadInit() override;
    void        threadRelease() override;
    void        run() override;
};

#endif   // YARP_DEV_RGBDSENSORWRAPPER_ROS_H
