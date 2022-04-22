/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_MOBILEBASEVELOCITYCONTROL_NWS_ROS
#define YARP_DEV_MOBILEBASEVELOCITYCONTROL_NWS_ROS

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/os/Subscriber.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Node.h>
#include <yarp/rosmsg/geometry_msgs/Twist.h>
#include <yarp/dev/WrapperSingle.h>

#include <mutex>
#include <string>

 /**
  *  @ingroup dev_impl_network_clients dev_impl_navigation
  *
  * \section MobileBaseVelocityControl_nws_ros
  *
  * \brief `MobileBaseVelocityControl_nws_ros`: A device which allows a client application to control the velocity of a mobile base from ROS.
  * The device opens a topic of type `yarp::rosmsg::geometry_msgs::Twist` to receive user commands
  *
  *  Parameters required by this device are:
  * | Parameter name | SubParameter   | Type    | Units          | Default Value                  | Required     | Description                                                       | Notes |
  * |:--------------:|:--------------:|:-------:|:--------------:|:------------------------------:|:------------:|:-----------------------------------------------------------------:|:-----:|
  * | node_name      |      -         | string  | -              | /mobileBase_VelControl_nws_ros | No           | Full name of the opened ROS node                                  |       |
  * | topic_name     |     -          | string  | -              | /velocity_input                | No           | Full name of the opened ROS topic                                 |       |
  * | subdevice      |      -         | string  | -              |   -                            | No           | name of the subdevice to instantiate                              | when used, parameters for the subdevice must be provided as well |
  */

class commandSubscriber :
    public yarp::os::Subscriber<yarp::rosmsg::geometry_msgs::Twist>
{
    public:
    void init(yarp::dev::Nav2D::INavigation2DVelocityActions* m_iNavVel);
    void deinit();

    ~commandSubscriber ();
    commandSubscriber ();
    yarp::dev::Nav2D::INavigation2DVelocityActions* m_iNavVel = nullptr;

    using yarp::os::Subscriber<yarp::rosmsg::geometry_msgs::Twist>::onRead;
    virtual void onRead (yarp::rosmsg::geometry_msgs::Twist& v) override;
};

class MobileBaseVelocityControl_nws_ros :
    public yarp::dev::DeviceDriver,
    public yarp::dev::WrapperSingle
{
protected:
    std::string                   m_ros_node_name = "/mobileBase_VelControl_nws_ros";
    std::string                   m_ros_topic_name = "/velocity_input";
    yarp::os::Node*               m_ros_node = nullptr;
    commandSubscriber*            m_command_subscriber = nullptr;

    yarp::dev::PolyDriver         m_subdev;

public:
    virtual ~MobileBaseVelocityControl_nws_ros () {};
    MobileBaseVelocityControl_nws_ros() {};

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

private:
    bool detach() override;
    bool attach(yarp::dev::PolyDriver* driver) override;
};

#endif // YARP_DEV_MOBILEBASEVELOCITYCONTROL_NWS_ROS
