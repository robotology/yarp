/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_FRAMETRANSFORMSETNWCROS_H
#define YARP_DEV_FRAMETRANSFORMSETNWCROS_H


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

#define ROSNODENAME "/tfNodeSet"
#define ROSTOPICNAME_TF "/tf"
#define ROSTOPICNAME_TF_STATIC "/tf_static"

/*
 * \section FrameTransformSet_nwc_ros_device_parameters Description of input parameters
 *
 *   Parameters required by this device are:
 * | Parameter name | SubParameter         | Type    | Units          | Default Valu          | Required     | Description                                                                                             |
 * |:--------------:|:--------------------:|:-------:|:--------------:|:---------------------:|:-----------: |:-------------------------------------------------------------------------------------------------------:|
 * | GENERAL        |      -               | group   | -              | -                     | No           |                                                                                                         |
 * | -              | period               | double  | seconds        | 0.01                  | No           | The PeriodicThread period in seconds                                                                    |
 * | -              | refresh_interval     | double  | seconds        | 0.1                   | No           | The time interval outside which timed fts will be deleted                                               |
 * | -              | asynch_pub           | int     | -              | 1                     | No           | If 1, the fts will be published not only every "period" seconds but also when set functions are called  |
 * | ROS            |      -               | group   | -              | -                     | No           |                                                                                                         |
 * | -              | ft_topic             | string  | -              | /tf                   | No           | The name of the ROS topic on which fts will be published                                                |
 * | -              | ft_topic_static      | string  | -              | /tf_static            | No           | The name of the ROS topic on which static fts will be published                                         |
 * | -              | ft_node              | string  | -              | /tfNodeSet            | No           | The of the ROS node                                                                                     |
 *
 * Some example of configuration files:
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device frameTransformSet_nwc_yarp
 * [GENERAL]
 * period 0.05
 * refresh_interval 0.2
 * [ROS]
 * ft_topic /tf
 * ft_topic_static /tf_static
 * ft_node /tfNodeSet
 * \endcode
 */

class FrameTransformSet_nwc_ros :
    public yarp::dev::DeviceDriver,
    public yarp::os::PeriodicThread,
    public yarp::dev::IFrameTransformStorageSet
{
public:
    FrameTransformSet_nwc_ros(double tperiod=0.010);
    ~FrameTransformSet_nwc_ros()=default;

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //periodicThread
    void run() override;

    //IFrameTransformStorageSet interface
    bool setTransforms(const std::vector<yarp::math::FrameTransform>& transforms) override;
    bool setTransform(const yarp::math::FrameTransform& transform) override;

    //own
    void publishFrameTransforms(const std::vector<yarp::math::FrameTransform>& transforms);
    void yarpTransformToROSTransform(const yarp::math::FrameTransform &input, yarp::rosmsg::geometry_msgs::TransformStamped& output);

private:
    mutable std::mutex                                     m_trf_mutex;
    std::string                                            m_nodeName{ROSNODENAME};
    std::string                                            m_topic{ROSTOPICNAME_TF};
    std::string                                            m_topic_static{ROSTOPICNAME_TF_STATIC};
    double                                                 m_period{0.01};
    double                                                 m_refreshInterval{0.1};
    bool                                                   m_asynchPub{true};
    yarp::os::Node*                                        m_rosNode{nullptr};
    yarp::os::Publisher<yarp::rosmsg::tf2_msgs::TFMessage> m_rosPublisherPort_tf_timed;
    yarp::os::Publisher<yarp::rosmsg::tf2_msgs::TFMessage> m_rosPublisherPort_tf_static;
    FrameTransformContainer                                m_ftContainer;
};

#endif // YARP_DEV_FRAMETRANSFORMSETNWCROS_H
