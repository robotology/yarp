/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_ODOMETRY2D_NWS_YARP_H
#define YARP_ODOMETRY2D_NWS_YARP_H

#include <yarp/os/Node.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Stamp.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IOdometry2D.h>
#include <yarp/dev/WrapperSingle.h>

#include <yarp/rosmsg/geometry_msgs/PolygonStamped.h>
#include <yarp/rosmsg/nav_msgs/Odometry.h>
#include <yarp/rosmsg/tf2_msgs/TFMessage.h>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#define DEG2RAD M_PI/180.0
#define DEFAULT_THREAD_PERIOD 0.02 //s

/**
 * @ingroup dev_impl_network_clients dev_impl_navigation
 *
 * \section Odometry2D_nws_ros_parameters Device description
 * \brief `Odometry2D_nws_ros`: A ros nws to get odometry and publish it on a ros topic.
 * The attached device must implement a `yarp::dev::Nav2D::IOdometry2D` interface.
 *
 * Parameters required by this device are:
 * | Parameter name      | SubParameter            | Type    | Units          | Default Value | Required                       | Description                                             | Notes |
 * |:-------------------:|:-----------------------:|:-------:|:--------------:|:-------------:|:-----------------------------: |:-------------------------------------------------------:|:-----:|
 * | period              |      -                  | double  | s              |   0.02        | No                             | refresh period of the broadcasted values in s           | default 0.02s |
 * | node_name           |      -                  | string  | -              |   -           | Yes                            | name of the ros node                                    | must begin with an initial '/'     |
 * | topic_name          |      -                  | string  | -              |   -           | Yes                            | name of the topic where the device must publish the data| must begin with an initial '/'     |
 * | odom_frame          |      -                  | string  | -              |   -           | Yes                            | name of the reference frame for odometry                |      |
 * | base_frame          |      -                  | string  | -              |   -           | Yes                            | name of the base frame for odometry                     |      |
 * | subdevice           |      -                  | string  | -              |   -           | alternative to 'attach' action | name of the subdevice to use as a data source           | when used, parameters for the subdevice must be provided as well |
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device odometry2D_nws_ros
 * period 0.02
 * node_name /odometry_ros
 * topic_name /odometry
 * odom_frame odom
 * base_frame base
 *
 * subdevice fakeOdometry
 * \endcode
 *
 * example of xml file with a fake odometer
 *
 * \code{.unparsed}
 * <?xml version="1.0" encoding="UTF-8" ?>
 * <!DOCTYPE robot PUBLIC "-//YARP//DTD yarprobotinterface 3.0//EN" "http://www.yarp.it/DTD/yarprobotinterfaceV3.0.dtd">
 * <robot name="fakeOdometry" build="2" portprefix="test" xmlns:xi="http://www.w3.org/2001/XInclude">
 *   <devices>
 *     <device xmlns:xi="http://www.w3.org/2001/XInclude" name="fakeOdometry_device" type="fakeOdometry">
 *     </device>
 *     <device xmlns:xi="http://www.w3.org/2001/XInclude" name="odometry2D_nws_ros" type="odometry2D_nws_ros">
 *       <param name="node_name"> /odometry_ros </param>
 *       <param name="topic_name"> /odometry </param>
 *       <param name="odom_frame">odom</param>
 *       <param name="base_frame">base</param>
 *       <action phase="startup" level="5" type="attach">
 *         <paramlist name="networks">
 *           <elem name="subdevice_odometry"> fakeOdometry_device </elem>
 *         </paramlist>
 *       </action>
 *       <action phase="shutdown" level="5" type="detach" />
 *     </device>
 *   </devices>
 * </robot>
 * \endcode
 *
 * example of command via terminal.
 *
 * \code{.unparsed}
 * yarpdev --device odometry2D_nws_ros --node_name /odometry_ros --topic_name /odometry --odom_frame odom --base_frame base --subdevice fakeOdometry
 * \endcode
 */

class Odometry2D_nws_ros :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle
//        public yarp::os::PortReader
{
public:
    Odometry2D_nws_ros();
    ~Odometry2D_nws_ros();

    // DeviceDriver
    bool open(yarp::os::Searchable &params) override;
    bool close() override;

    // WrapperSingle
    bool attach(yarp::dev::PolyDriver* driver) override;
    bool detach() override;

    // PeriodicThread
    bool threadInit() override;
    void threadRelease() override;
    void run() override;


private:
    // parameters from configuration
    std::string m_topicName;
    std::string m_nodeName;
    std::string m_odomFrame;
    std::string m_baseFrame;
    bool m_enable_publish_tf = true;

    // timestamp
    yarp::os::Stamp m_lastStateStamp;

    // period for thread
    double m_period{DEFAULT_THREAD_PERIOD};

    //ros node
    yarp::os::Node* m_node;

    //interfaces
    yarp::dev::PolyDriver m_driver;
    yarp::dev::Nav2D::IOdometry2D *m_odometry2D_interface{nullptr};
    yarp::os::Publisher<yarp::rosmsg::nav_msgs::Odometry>          rosPublisherPort_odometry;
    yarp::os::Publisher<yarp::rosmsg::tf2_msgs::TFMessage>         rosPublisherPort_tf;

};

#endif // YARP_ODOMETRY2D_NWS_YARP_H
