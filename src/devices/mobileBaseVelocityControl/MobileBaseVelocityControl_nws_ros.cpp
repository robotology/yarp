/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define _USE_MATH_DEFINES
#include <cmath>

#include "MobileBaseVelocityControl_nws_ros.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>

/*! \file MobileBaseVelocityControl_nws_ros.cpp */

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
    YARP_LOG_COMPONENT(MOBVEL_NWS_ROS, "yarp.device.MobileBaseVelocityControl_nws_ros")
}

//------------------------------------------------------------------------------------------------------------------------------

void commandSubscriber::init(yarp::dev::Nav2D::INavigation2DVelocityActions* _iNavVel)
{
    m_iNavVel = _iNavVel;
}

void commandSubscriber::deinit()
{
    m_iNavVel = nullptr;
}

commandSubscriber::commandSubscriber()
{
    //this->setStrict();
    this->useCallback();
}

commandSubscriber::~commandSubscriber()
{
    this->close();
}

void commandSubscriber::onRead(yarp::rosmsg::geometry_msgs::Twist& v)
{
    if (m_iNavVel)
    {
        m_iNavVel->applyVelocityCommand(v.linear.x, v.linear.y, v.angular.z * 180 / M_PI);
    }
    else
    {
        yCError(MOBVEL_NWS_ROS, "Subdevice interface not yet initialized");
    }
}

//------------------------------------------------------------------------------------------------------------------------------

bool MobileBaseVelocityControl_nws_ros::open(yarp::os::Searchable& config)
{
    if (config.check("node_name") == true)
    {
        m_ros_node_name = config.find("node_name").asString();
    }

    if (config.check("topic_name") == true)
    {
        m_ros_topic_name = config.find("topic_name").asString();
    }

    //attach subdevice if required
    if (config.check("subdevice"))
    {
        Property       p;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if (!m_subdev.open(p) || !m_subdev.isValid())
        {
            yCError(MOBVEL_NWS_ROS) << "Failed to open subdevice.. check params";
            return false;
        }

        if (!attach(&m_subdev))
        {
            yCError(MOBVEL_NWS_ROS) << "Failed to attach subdevice.. check params";
            return false;
        }
    }
    else
    {
        yCInfo(MOBVEL_NWS_ROS) << "Waiting for device to attach";
    }

    //open the subscriber
    m_ros_node = new yarp::os::Node(m_ros_node_name);
    m_command_subscriber = new commandSubscriber();

    if (!m_command_subscriber->topic(m_ros_topic_name))
    {
        yCError(MOBVEL_NWS_ROS) << " opening " << m_ros_topic_name << " Topic, check your yarp-ROS network configuration\n";
        return false;
    }

    return true;
}

bool MobileBaseVelocityControl_nws_ros::close()
{
    if (m_command_subscriber) {delete m_command_subscriber;}
    if (m_ros_node) {delete m_ros_node;}
    if (m_subdev.isValid()) { m_subdev.close(); }
    return true;
}


bool MobileBaseVelocityControl_nws_ros::detach()
{
    m_command_subscriber->deinit();
    return true;
}

bool MobileBaseVelocityControl_nws_ros::attach(PolyDriver* driver)
{
    yarp::dev::Nav2D::INavigation2DVelocityActions* iNavVel=nullptr;

    if (driver->isValid())
    {
        driver->view(iNavVel);
    }

    if (nullptr == iNavVel)
    {
        yCError(MOBVEL_NWS_ROS, "Subdevice passed to attach method is invalid");
        return false;
    }

    m_command_subscriber->init(iNavVel);

    return true;
}
