/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#define _USE_MATH_DEFINES

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IFrameTransform.h>
#include <yarp/math/Math.h>
#include "localization2D_nws_ros.h"

#include <cmath>

/*! \file Localization2D_nws_ros.cpp */

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace std;

#define DEFAULT_THREAD_PERIOD 0.01

namespace {
YARP_LOG_COMPONENT(LOCALIZATION2D_NWS_ROS, "yarp.device.localization2D_nws_ros")
}

//------------------------------------------------------------------------------------------------------------------------------

Localization2D_nws_ros::Localization2D_nws_ros() : PeriodicThread(DEFAULT_THREAD_PERIOD),
                                                   m_period(DEFAULT_THREAD_PERIOD)
{
    m_stats_time_last = yarp::os::Time::now();
}

bool Localization2D_nws_ros::attachAll(const PolyDriverList &device2attach)
{
    if (device2attach.size() != 1)
    {
        yCError(LOCALIZATION2D_NWS_ROS, "Cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(iLoc);
    }

    if (nullptr == iLoc)
    {
        yCError(LOCALIZATION2D_NWS_ROS, "Subdevice passed to attach method is invalid");
        return false;
    }

    //initialize m_current_position and m_current_status, if available
    bool ret = true;
    yarp::dev::Nav2D::LocalizationStatusEnum status;
    Map2DLocation loc;
    ret &= iLoc->getLocalizationStatus(status);
    ret &= iLoc->getCurrentPosition(loc);
    if (ret)
    {
        m_current_status = status;
        m_current_position = loc;
    }
    else
    {
        yCWarning(LOCALIZATION2D_NWS_ROS) << "Localization data not yet available during server initialization";
    }

    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}

bool Localization2D_nws_ros::detachAll()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    iLoc = nullptr;
    return true;
}

bool Localization2D_nws_ros::open(Searchable& config)
{
    Property params;
    params.fromString(config.toString().c_str());
    yCDebug(LOCALIZATION2D_NWS_ROS) << "Configuration: \n" << config.toString().c_str();

    if (config.check("GENERAL") == false)
    {
        yCWarning(LOCALIZATION2D_NWS_ROS) << "Missing GENERAL group, assuming default options";
    }

    Bottle& general_group = config.findGroup("GENERAL");
    if (!general_group.check("period"))
    {
        yCInfo(LOCALIZATION2D_NWS_ROS) << "Missing 'period' parameter. Using default value: " << DEFAULT_THREAD_PERIOD;
        m_period = DEFAULT_THREAD_PERIOD;
    }
    else
    {
        m_period = general_group.find("period").asFloat64();
        yCInfo(LOCALIZATION2D_NWS_ROS) << "Period requested: " << m_period;
    }

    if (!general_group.check("publish_odometry"))
    {
        m_enable_publish_odometry_topic = general_group.find("publish_odometry").asBool();
        yCInfo(LOCALIZATION2D_NWS_ROS) << "publish_odometry=" << m_enable_publish_odometry_topic;
    }
    if (!general_group.check("publish_tf"))
    {
        m_enable_publish_odometry_tf = general_group.find("publish_tf").asBool();
        yCInfo(LOCALIZATION2D_NWS_ROS) << "publish_tf=" << m_enable_publish_odometry_tf;
    }

    if (!general_group.check("name"))
    {
        yCInfo(LOCALIZATION2D_NWS_ROS) << "Missing 'name' parameter. Using default value: " << m_local_name;
    }
    else
    {
        m_local_name = general_group.find("name").asString();
        if (m_local_name.c_str()[0] != '/') { yCError(LOCALIZATION2D_NWS_ROS) << "Missing '/' in name parameter" ;  return false; }
        yCInfo(LOCALIZATION2D_NWS_ROS) << "Using local name:" << m_local_name;
    }

    m_rpcPortName = m_local_name + "/rpc";

    if (config.check("subdevice"))
    {
        Property       p;
        PolyDriverList driverlist;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if (!pLoc.open(p) || !pLoc.isValid())
        {
            yCError(LOCALIZATION2D_NWS_ROS) << "Failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&pLoc, "1");
        if (!attachAll(driverlist))
        {
            yCError(LOCALIZATION2D_NWS_ROS) << "Failed to open subdevice.. check params";
            return false;
        }
    }
    else
    {
        yCInfo(LOCALIZATION2D_NWS_ROS) << "Waiting for device to attach";
    }
    m_stats_time_last = yarp::os::Time::now();

    if (!initialize_YARP(config))
    {
        yCError(LOCALIZATION2D_NWS_ROS) << "Error initializing YARP ports";
        return false;
    }

    if (!initialize_ROS(config))
    {
        yCError(LOCALIZATION2D_NWS_ROS) << "Error initializing ROS system";
        return false;
    }

    return true;
}

bool Localization2D_nws_ros::initialize_ROS(yarp::os::Searchable& params)
{
    m_ros_node_name = m_local_name + "_ROSnode";
    m_odom_topic_name = m_local_name + "/odom";

    if (params.check("ROS"))
    {
        Bottle& ros_group = params.findGroup("ROS");
        if (ros_group.check("parent_frame_id"))
        {
            m_parent_frame_id = ros_group.find("parent_frame_id").asString();
        }

        if (ros_group.check("child_frame_id"))
        {
            m_child_frame_id = ros_group.find("child_frame_id").asString();
        }

        if (ros_group.check("odometry_topic"))
        {
            m_odom_topic_name = ros_group.find("odometry_topic").asString();
        }

        if (ros_group.check("node_name"))
        {
            m_ros_node_name = ros_group.find("node_name").asString();
        }
    }

    if (m_ros_node == nullptr)
    {
        bool b= false;
        m_ros_node = new yarp::os::Node(m_ros_node_name);
        if (m_ros_node == nullptr)
        {
            yCError(LOCALIZATION2D_NWS_ROS) << "Opening " << m_ros_node_name << " Node, check your yarp-ROS network configuration";
        }

        b = m_odometry_publisher.topic(m_odom_topic_name);
        if (!b)
        {
            yCError(LOCALIZATION2D_NWS_ROS) << "Unable to publish data on" << m_odom_topic_name << "topic";
        }
        b = m_tf_publisher.topic("/tf");
        if (!b)
        {
            yCError(LOCALIZATION2D_NWS_ROS) << "Unable to publish data on /tf topic";
        }
        yCInfo(LOCALIZATION2D_NWS_ROS) << "ROS initialized";
    }
    return true;
}

bool Localization2D_nws_ros::initialize_YARP(yarp::os::Searchable &params)
{
    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yCError(LOCALIZATION2D_NWS_ROS, "Failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    return true;
}

bool Localization2D_nws_ros::close()
{
    yCTrace(LOCALIZATION2D_NWS_ROS, "Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    detachAll();

    m_rpcPort.interrupt();
    m_rpcPort.close();

    if (m_ros_node)
    {
        m_tf_publisher.close();
        m_odometry_publisher.close();
        delete m_ros_node;
        m_ros_node = nullptr;
    }

    yCDebug(LOCALIZATION2D_NWS_ROS) << "Execution terminated";
    return true;
}

bool Localization2D_nws_ros::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) return false;

    reply.clear();

    if (command.get(0).isString() && command.get(0).asString() == "help")
    {
        reply.addVocab(Vocab::encode("many"));
        reply.addString("No commands currently available:");
    }
    else
    {
        yCError(LOCALIZATION2D_NWS_ROS) << "Invalid command. Try `help`";
        reply.addVocab(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }

    return true;
}

void Localization2D_nws_ros::run()
{
    double m_stats_time_curr = yarp::os::Time::now();
    if (m_stats_time_curr - m_stats_time_last > 5.0)
    {
        yCInfo(LOCALIZATION2D_NWS_ROS) << "Running";
        m_stats_time_last = yarp::os::Time::now();
    }

    bool ret = iLoc->getLocalizationStatus(m_current_status);
    if (ret == false)
    {
        yCError(LOCALIZATION2D_NWS_ROS) << "getLocalizationStatus() failed";
    }

    if (m_current_status == LocalizationStatusEnum::localization_status_localized_ok)
    {
        bool ret2 = iLoc->getCurrentPosition(m_current_position);
        if (ret2 == false)
        {
            yCError(LOCALIZATION2D_NWS_ROS) << "getCurrentPosition() failed";
        }
        else
        {
            m_loc_stamp.update();
        }
        bool ret3 = iLoc->getEstimatedOdometry(m_current_odometry);
        if (ret3 == false)
        {
            //yCError(LOCALIZATION2D_NWS_ROS) << "getEstimatedOdometry() failed";
        }
        else
        {
            m_odom_stamp.update();
        }
    }
    else
    {
        yCWarning(LOCALIZATION2D_NWS_ROS, "The system is not properly localized!");
    }

    if (m_enable_publish_odometry_topic) publish_odometry_on_ROS_topic();
    if (m_enable_publish_odometry_tf)    publish_odometry_on_TF_topic();

}

void Localization2D_nws_ros::publish_odometry_on_TF_topic()
{
    yarp::rosmsg::tf2_msgs::TFMessage& rosData = m_tf_publisher.prepare();
    yarp::rosmsg::geometry_msgs::TransformStamped transform;
    transform.child_frame_id = m_child_frame_id;
    transform.header.frame_id = m_parent_frame_id;
    transform.header.seq = m_odom_stamp.getCount();
    transform.header.stamp = m_odom_stamp.getTime();
    double halfYaw = m_current_odometry.odom_theta / 180.0 * M_PI * 0.5;
    double cosYaw = cos(halfYaw);
    double sinYaw = sin(halfYaw);
    transform.transform.rotation.x = 0;
    transform.transform.rotation.y = 0;
    transform.transform.rotation.z = sinYaw;
    transform.transform.rotation.w = cosYaw;
    transform.transform.translation.x = m_current_odometry.odom_x;
    transform.transform.translation.y = m_current_odometry.odom_y;
    transform.transform.translation.z = 0;
    if (rosData.transforms.size() == 0)
    {
        rosData.transforms.push_back(transform);
    }
    else
    {
        rosData.transforms[0] = transform;
    }

    m_tf_publisher.write();
}

void Localization2D_nws_ros::publish_odometry_on_ROS_topic()
{
    if (m_ros_node && m_odometry_publisher.asPort().getOutputCount() > 0)
    {
        yarp::rosmsg::nav_msgs::Odometry& odom = m_odometry_publisher.prepare();
        odom.clear();
        odom.header.frame_id = m_fixed_frame;
        odom.header.seq = m_odom_stamp.getCount();
        odom.header.stamp = m_odom_stamp.getTime();
        odom.child_frame_id = m_robot_frame;

        odom.pose.pose.position.x = m_current_odometry.odom_x;
        odom.pose.pose.position.y = m_current_odometry.odom_y;
        odom.pose.pose.position.z = 0;
        yarp::sig::Vector vecrpy(3);
        vecrpy[0] = 0;
        vecrpy[1] = 0;
        vecrpy[2] = m_current_odometry.odom_theta;
        yarp::sig::Matrix matrix = yarp::math::rpy2dcm(vecrpy);
        yarp::math::Quaternion q; q.fromRotationMatrix(matrix);
        odom.pose.pose.orientation.x = q.x();
        odom.pose.pose.orientation.y = q.y();
        odom.pose.pose.orientation.z = q.z();
        odom.pose.pose.orientation.w = q.w();
        //odom.pose.covariance = 0;

        odom.twist.twist.linear.x = m_current_odometry.base_vel_x;
        odom.twist.twist.linear.y = m_current_odometry.base_vel_y;
        odom.twist.twist.linear.z = 0;
        odom.twist.twist.angular.x = 0;
        odom.twist.twist.angular.y = 0;
        odom.twist.twist.angular.z = m_current_odometry.base_vel_theta;
        //odom.twist.covariance = 0;

        m_odometry_publisher.write();
    }
}
