/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_LOCALIZATION2DSERVER_H
#define YARP_DEV_LOCALIZATION2DSERVER_H


#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ILocalization2D.h>
#include <yarp/dev/OdometryData.h>
#include <yarp/rosmsg/nav_msgs/Odometry.h>
#include <yarp/rosmsg/tf2_msgs/TFMessage.h>
#include <math.h>

 /**
 * @ingroup dev_impl_network_wrapper dev_impl_navigation dev_impl_deprecated
 *
 * \section Localization2DServer
 *
 * \brief `localization2DServer` *deprecated*: A localization server which can be wrap multiple algorithms and devices to provide robot localization in a 2D World.
 *
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value       | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | GENERAL        |  period        | double  | s              | 0.01                | No           | The period of the working thread                                  |       |
 * | GENERAL        |  retrieve_position_periodically     | bool  | -  | true         | No           | If true, the subdevice is asked periodically to retrieve the current location. Otherwise the current location is obtained asynchronously when a getCurrentPosition() command is issued.     | -     |
 * | GENERAL        |  name          | string  |  -             | /localizationServer | No           | The name of the server, used as a prefix for the opened ports     | By default ports opened are /localizationServer/rpc and /localizationServer/streaming:o     |
 * | subdevice      |  -             | string  |  -             |  -                  | Yes          | The name of the of Localization device to be used                 | -     |
 * | ROS            |  publish_tf    | bool    |  -             |  false              | No           | If true, odometry data will be published on global ROS /tf topic      | -     |
 * | ROS            |  publish_odom  | bool    |  -             |  false              | No           | If true, odometry data will be published on a user-defined ROS topic  | The default name of the topic is built as: name+"/odom"     |
 */
class Localization2DServer :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public yarp::dev::IMultipleWrapper,
        public yarp::os::PortReader
{
protected:

    //general options
    bool m_ros_publish_odometry_on_topic;
    bool m_ros_publish_odometry_on_tf;

    //yarp
    std::string                               m_local_name;
    yarp::os::Port                            m_rpcPort;
    std::string                               m_rpcPortName;
    yarp::os::BufferedPort<yarp::dev::Nav2D::Map2DLocation>  m_2DLocationPort;
    std::string                               m_2DLocationPortName;
    yarp::os::BufferedPort<yarp::dev::OdometryData>  m_odometryPort;
    std::string                               m_odometryPortName;
    std::string                               m_robot_frame;
    std::string                               m_fixed_frame;

    //ROS
    std::string                                           m_child_frame_id;
    std::string                                           m_parent_frame_id;
    yarp::os::Node*                                       m_ros_node;
    yarp::os::Publisher<yarp::rosmsg::nav_msgs::Odometry> m_odometry_publisher;
    yarp::os::Publisher<yarp::rosmsg::tf2_msgs::TFMessage>  m_tf_publisher;

    //drivers and interfaces
    yarp::dev::PolyDriver                   pLoc;
    yarp::dev::Nav2D::ILocalization2D*      iLoc;

    double                                  m_stats_time_last;
    double                                  m_period;
    yarp::os::Stamp                         m_loc_stamp;
    yarp::os::Stamp                         m_odom_stamp;
    bool                                    m_getdata_using_periodic_thread;

    yarp::dev::OdometryData                     m_current_odometry;
    yarp::dev::Nav2D::Map2DLocation             m_current_position;
    yarp::dev::Nav2D::LocalizationStatusEnum    m_current_status;

private:
    void publish_2DLocation_on_yarp_port();
    void publish_odometry_on_yarp_port();
    void publish_odometry_on_ROS_topic();
    void publish_odometry_on_TF_topic();

public:
    Localization2DServer();

public:
    virtual bool open(yarp::os::Searchable& prop) override;
    virtual bool close() override;
    virtual bool detachAll() override;
    virtual bool attachAll(const yarp::dev::PolyDriverList &l) override;
    virtual void run() override;

    bool initialize_YARP(yarp::os::Searchable &config);
    bool initialize_ROS(yarp::os::Searchable& config);
    virtual bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_DEV_LOCALIZATION2DSERVER_H
