/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_FRAMETRANSFORMGETNWCROS_H
#define YARP_DEV_FRAMETRANSFORMGETNWCROS_H


#include <yarp/os/Network.h>
#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Node.h>
#include <mutex>
#include <map>

#include <yarp/math/FrameTransform.h>
#include <frameTransformContainer.h>

#include <yarp/rosmsg/geometry_msgs/TransformStamped.h>
#include <yarp/rosmsg/tf2_msgs/TFMessage.h>

#define ROSNODENAME "/tfNodeGet"
#define ROSTOPICNAME_TF "/tf"
#define ROSTOPICNAME_TF_STATIC "/tf_static"

/*
 * \section FrameTransformGet_nwc_ros_device_parameters Description of input parameters
 *
 *   Parameters required by this device are:
 * | Parameter name | SubParameter         | Type    | Units          | Default Value         | Required     | Description                                    -------            |
 * |:--------------:|:--------------------:|:-------:|:--------------:|:---------------------:|:-----------: |:-----------------------------------------------------------------:|
 * | GENERAL        |      -               | group   | -              | -                     | No           |                                                                   |
 * | -              | period               | double  | seconds        | 0.01                  | No           | The PeriodicThread period in seconds                              |
 * | -              | refresh_interval     | double  | seconds        | 0.1                   | No           | The time interval outside which timed ft will be deleted          |
 * | ROS            |      -               | group   | -              | -                     | No           |                                                                   |
 * | -              | ft_topic             | string  | -              | /tf                   | No           | The name of the ROS topic from which fts will be received         |
 * | -              | ft_topic_static      | string  | -              | /tf_static            | No           | The name of the ROS topic from which static fts will be received  |
 * | -              | ft_node              | string  | -              | /tfNodeGet            | No           | The of the ROS node                                               |
 *
 * Some example of configuration files:
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device frameTransformGet_nwc_yarp
 * [GENERAL]
 * period 0.05
 * refresh_interval 0.2
 * [ROS]
 * ft_topic /tf
 * ft_topic_static /tf_static
 * ft_node /tfNodeGet
 * \endcode
 */

class FrameTransformGet_nwc_ros :
    public yarp::dev::DeviceDriver,
    public yarp::os::PeriodicThread,
    public yarp::dev::IFrameTransformStorageGet
{
public:
    FrameTransformGet_nwc_ros(double tperiod=0.010);
    ~FrameTransformGet_nwc_ros()=default;

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //periodicThread
    void run() override;

    //IFrameTransformStorageGet interface
    bool getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const override;

    //own
    void receiveFrameTransforms();
    void rosTransformToYARPTransform(const yarp::rosmsg::geometry_msgs::TransformStamped &input, yarp::math::FrameTransform &output, bool isStatic);
    double yarpStampFromROS(const yarp::rosmsg::TickTime& rosTime);

private:
    mutable std::mutex                                       m_trf_mutex;
    std::string                                              m_nodeName{ROSNODENAME};
    std::string                                              m_topic{ROSTOPICNAME_TF};
    std::string                                              m_topic_static{ROSTOPICNAME_TF_STATIC};
    double                                                   m_period{0.01};
    double                                                   m_refreshInterval{0.1};
    yarp::os::Node*                                          m_rosNode{nullptr};
    yarp::os::Subscriber<yarp::rosmsg::tf2_msgs::TFMessage>  m_rosSubscriberPort_tf_timed;
    yarp::os::Subscriber<yarp::rosmsg::tf2_msgs::TFMessage>  m_rosSubscriberPort_tf_static;
    FrameTransformContainer                                  m_ftContainer;
};

#endif // YARP_DEV_FRAMETRANSFORMGETNWCROS_H
