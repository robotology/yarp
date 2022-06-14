/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Odometry2D_nws_ros.h"
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Stamp.h>
#include <cmath>

YARP_LOG_COMPONENT(ODOMETRY2D_NWS_ROS, "yarp.devices.Odometry2D_nws_ros")

Odometry2D_nws_ros::Odometry2D_nws_ros() : yarp::os::PeriodicThread(DEFAULT_THREAD_PERIOD)
{
}

Odometry2D_nws_ros::~Odometry2D_nws_ros()
{
    m_odometry2D_interface = nullptr;
}


bool Odometry2D_nws_ros::attach(yarp::dev::PolyDriver* driver)
{

    if (driver->isValid())
    {
        driver->view(m_odometry2D_interface);
    } else {
        yCError(ODOMETRY2D_NWS_ROS) << "not valid driver";
    }

    if (m_odometry2D_interface == nullptr)
    {
        yCError(ODOMETRY2D_NWS_ROS, "Subdevice passed to attach method is invalid");
        return false;
    }

    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}


bool Odometry2D_nws_ros::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    m_odometry2D_interface = nullptr;
    return true;
}

bool Odometry2D_nws_ros::threadInit()
{
    return true;
}

bool Odometry2D_nws_ros::open(yarp::os::Searchable &config)
{
    yarp::os::Property params;
    params.fromString(config.toString());

    if (config.check("publish_tf_topic")) {
        m_enable_publish_tf = true;
    }
    if (config.check("skip_tf_topic")) {
        m_enable_publish_tf = false;
    }

    if (!config.check("period")) {
        yCWarning(ODOMETRY2D_NWS_ROS) << "missing 'period' parameter, using default value of" << DEFAULT_THREAD_PERIOD;
    } else {
        m_period = config.find("period").asFloat64();
    }

    if (!config.check("node_name")) {
        yCError(ODOMETRY2D_NWS_ROS) << "missing node_name parameter";
        return false;
    }
    m_nodeName = config.find("node_name").asString();
    if (m_nodeName[0] != '/') {
        yCError(ODOMETRY2D_NWS_ROS) << "missing initial / in node_name parameter";
        return false;
    }

    if (!config.check("topic_name")) {
        yCError(ODOMETRY2D_NWS_ROS) << "missing topic_name parameter";
        return false;
    }
    m_topicName = config.find("topic_name").asString();
    if (m_topicName[0] != '/') {
        yCError(ODOMETRY2D_NWS_ROS) << "missing initial / in topic_name parameter";
        return false;
    }

    if (!config.check("odom_frame")) {
        yCError(ODOMETRY2D_NWS_ROS) << "missing odom_frame parameter";
        return false;
    }
    m_odomFrame = config.find("odom_frame").asString();


    if (!config.check("base_frame")) {
        yCError(ODOMETRY2D_NWS_ROS) << "missing base_frame parameter";
        return false;
    }
    m_baseFrame = config.find("base_frame").asString();

    if (config.check("subdevice")) {
        yarp::os::Property p;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if (!m_driver.open(p) || !m_driver.isValid()) {
            yCError(ODOMETRY2D_NWS_ROS) << "failed to open subdevice.. check params";
            return false;
        }

        if (!attach(&m_driver)) {
            yCError(ODOMETRY2D_NWS_ROS) << "failed to open subdevice.. check params";
            return false;
        }
    }
    m_node = new yarp::os::Node(m_nodeName);
    if (m_node == nullptr) {
        yCError(ODOMETRY2D_NWS_ROS) << " opening " << m_nodeName << " Node, check your yarp-ROS network configuration\n";
        return false;
    }
    if (!rosPublisherPort_odometry.topic(m_topicName)) {
        yCError(ODOMETRY2D_NWS_ROS) << " opening " << m_topicName << " Topic, check your yarp-ROS network configuration\n";
        return false;
    }

    if (m_enable_publish_tf)
    {
        if (!rosPublisherPort_tf.topic("/tf")) {
            yCError(ODOMETRY2D_NWS_ROS) << " opening " << "/tf" << " Topic, check your yarp-ROS network configuration\n";
            return false;
        }
    }
    return true;
}

void Odometry2D_nws_ros::threadRelease()
{
}

void Odometry2D_nws_ros::run()
{
    if (m_odometry2D_interface!=nullptr)
    {
        yarp::dev::OdometryData odometryData;
        double synchronized_timestamp = 0;
        m_odometry2D_interface->getOdometry(odometryData, &synchronized_timestamp);

        if (std::isnan(synchronized_timestamp) == false)
        {
            m_lastStateStamp.update(synchronized_timestamp);
        }
        else
        {
            m_lastStateStamp.update(yarp::os::Time::now());
        }

        if (1)
        {
            yarp::rosmsg::nav_msgs::Odometry& rosData = rosPublisherPort_odometry.prepare();
            rosData.header.seq = m_lastStateStamp.getCount();
            rosData.header.stamp = m_lastStateStamp.getTime();
            rosData.header.frame_id = m_odomFrame;
            rosData.child_frame_id = m_baseFrame;

            rosData.pose.pose.position.x = odometryData.odom_x;
            rosData.pose.pose.position.y = odometryData.odom_y;
            rosData.pose.pose.position.z = 0.0;
            yarp::rosmsg::geometry_msgs::Quaternion odom_quat;
            double halfYaw = odometryData.odom_theta * DEG2RAD * 0.5;
            double cosYaw = cos(halfYaw);
            double sinYaw = sin(halfYaw);
            odom_quat.x = 0;
            odom_quat.y = 0;
            odom_quat.z = sinYaw;
            odom_quat.w = cosYaw;
            rosData.pose.pose.orientation = odom_quat;
            rosData.twist.twist.linear.x = odometryData.base_vel_x;
            rosData.twist.twist.linear.y = odometryData.base_vel_y;
            rosData.twist.twist.linear.z = 0;
            rosData.twist.twist.angular.x = 0;
            rosData.twist.twist.angular.y = 0;
            rosData.twist.twist.angular.z = odometryData.base_vel_theta * DEG2RAD;
            rosPublisherPort_odometry.write();
        }

        if (m_enable_publish_tf)
        {
            yarp::rosmsg::tf2_msgs::TFMessage& rosData = rosPublisherPort_tf.prepare();
            yarp::rosmsg::geometry_msgs::TransformStamped transform;
            transform.header.frame_id = m_odomFrame;
            transform.child_frame_id = m_baseFrame;
            transform.header.seq = m_lastStateStamp.getCount();
            transform.header.stamp = m_lastStateStamp.getTime();
            double halfYaw = odometryData.odom_theta * DEG2RAD * 0.5;
            double cosYaw = cos(halfYaw);
            double sinYaw = sin(halfYaw);
            transform.transform.rotation.x = 0;
            transform.transform.rotation.y = 0;
            transform.transform.rotation.z = sinYaw;
            transform.transform.rotation.w = cosYaw;
            transform.transform.translation.x = odometryData.odom_x;
            transform.transform.translation.y = odometryData.odom_y;
            transform.transform.translation.z = 0;
            if (rosData.transforms.size() == 0)
            {
                rosData.transforms.push_back(transform);
            }
            else if (rosData.transforms.size() == 1)
            {
                rosData.transforms[0] = transform;
            }
            else
            {
                yCWarning(ODOMETRY2D_NWS_ROS) << "Size of /tf topic should be 1, instead it is:" << rosData.transforms.size();
            }
            rosPublisherPort_tf.write();
        }

    } else{
        yCError(ODOMETRY2D_NWS_ROS) << "the interface is not valid";
    }
}

bool Odometry2D_nws_ros::close()
{
    yCTrace(ODOMETRY2D_NWS_ROS, "Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    detach();

    if (m_node)
    {
        rosPublisherPort_odometry.close();
        if (m_enable_publish_tf)
        {
           rosPublisherPort_tf.close();
        }
        delete m_node;
        m_node = nullptr;
    }

    yCDebug(ODOMETRY2D_NWS_ROS) << "Execution terminated";
    return true;
}
