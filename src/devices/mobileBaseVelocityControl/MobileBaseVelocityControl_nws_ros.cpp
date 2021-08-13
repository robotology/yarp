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

    m_ros_node = new yarp::os::Node (m_ros_node_name);

    if (!m_ros_subscriber.topic(m_ros_topic_name))
    {
        yCError(MOBVEL_NWS_ROS) << " opening " << m_ros_topic_name << " Topic, check your yarp-ROS network configuration\n";
        return false;
    }

    if (config.check("period"))
    {
        m_period = config.find("period").asFloat32();
        this->setPeriod (m_period);
    }
    else
    {
        yCWarning(MOBVEL_NWS_ROS, "Using default period of %f s", m_period);
    }

    return true;
}

bool MobileBaseVelocityControl_nws_ros::close()
{
    m_ros_subscriber.close();
    delete m_ros_node;
    return true;
}

bool MobileBaseVelocityControl_nws_ros::threadInit()
{
    return true;
}

void  MobileBaseVelocityControl_nws_ros::run()
{
    if (yarp::rosmsg::geometry_msgs::Twist* rosTwist = m_ros_subscriber.read(false))
    {
        this->m_iNavVel->applyVelocityCommand(rosTwist->linear.x,
                                              rosTwist->linear.y,
                                              rosTwist->angular.z * 180 / M_PI);
    }
}

bool MobileBaseVelocityControl_nws_ros::detach()
{
    m_iNavVel = nullptr;
    return true;
}

bool MobileBaseVelocityControl_nws_ros::attach(PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(m_iNavVel);
    }

    if (nullptr == m_iNavVel)
    {
        yCError(MOBVEL_NWS_ROS, "Subdevice passed to attach method is invalid");
        return false;
    }

    return true;
}
