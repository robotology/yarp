/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameTransformGet_nwc_ros.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORGETNWCROS, "yarp.device.frameTransformGet_nwc_ros")
}

//------------------------------------------------------------------------------------------------------------------------------
FrameTransformGet_nwc_ros::FrameTransformGet_nwc_ros(double tperiod) :
PeriodicThread(tperiod),
m_period(tperiod)
{
}

bool FrameTransformGet_nwc_ros::open(yarp::os::Searchable& config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORGETNWCROS,"Error! YARP Network is not initialized");
        return false;
    }

    bool okGeneral = config.check("GENERAL");
    if(okGeneral)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        if (general_config.check("period"))
        {
            m_period = general_config.find("period").asFloat64();
            setPeriod(m_period);
        }
        if (general_config.check("refresh_interval"))       {m_refreshInterval = general_config.find("refresh_interval").asFloat64();}
    }
    m_ftContainer.m_timeout = m_refreshInterval;

    //ROS configuration
    if (config.check("ROS"))
    {
        yCInfo(FRAMETRANSFORGETNWCROS, "Configuring ROS params");
        Bottle ROS_config = config.findGroup("ROS");
        if (ROS_config.check("ft_topic")) {
            m_topic = ROS_config.find("ft_topic").asString();
        }
        if (ROS_config.check("ft_topic_static")) {
            m_topic_static = ROS_config.find("ft_topic_static").asString();
        }
        if (ROS_config.check("ft_node")) {
            m_nodeName = ROS_config.find("ft_node").asString();
        }

        //open ros publisher
        if (m_rosNode == nullptr)
        {
            m_rosNode = new yarp::os::Node(m_nodeName);
        }
        if (!m_rosSubscriberPort_tf_timed.topic(m_topic))
        {
            yCError(FRAMETRANSFORGETNWCROS) << "Unable to publish data on " << m_topic << " topic, check your yarp-ROS network configuration";
            return false;
        }
        if (!m_rosSubscriberPort_tf_static.topic(m_topic_static))
        {
            yCError(FRAMETRANSFORGETNWCROS) << "Unable to publish data on " << m_topic_static << " topic, check your yarp-ROS network configuration";
            return false;
        }
    }
    else
    {
        //no ROS options
        yCWarning(FRAMETRANSFORGETNWCROS) << "ROS Group not configured";
        return false;
    }

    start();

    return true;
}

bool FrameTransformGet_nwc_ros::close()
{
    yCTrace(FRAMETRANSFORGETNWCROS, "Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    m_rosSubscriberPort_tf_timed.close();
    m_rosSubscriberPort_tf_static.close();
    m_rosNode = nullptr;
    return true;
}

void FrameTransformGet_nwc_ros::run()
{
    std::lock_guard <std::mutex> lg(m_trf_mutex);
    if(!m_ftContainer.checkAndRemoveExpired())
    {
        yCError(FRAMETRANSFORGETNWCROS,"Unable to remove expired transforms");
        return;
    }
    receiveFrameTransforms();
}

bool FrameTransformGet_nwc_ros::getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const
{
    std::lock_guard<std::mutex> lock(m_trf_mutex);
    if(!m_ftContainer.checkAndRemoveExpired())
    {
        yCError(FRAMETRANSFORGETNWCROS,"Unable to remove expired transforms");
        return false;
    }
    if(!m_ftContainer.getTransforms(transforms))
    {
        yCError(FRAMETRANSFORGETNWCROS,"Unable to retrieve transforms");
        return false;
    }
    return true;
}

double FrameTransformGet_nwc_ros::yarpStampFromROS(const yarp::rosmsg::TickTime& rosTime)
{
    double yarpTime;
    double sec_part;
    double nsec_part;
    sec_part = (double)rosTime.sec;
    nsec_part = ((double)rosTime.nsec)/1000000000.0;
    yarpTime = sec_part+nsec_part;

    return yarpTime;
}

void  FrameTransformGet_nwc_ros::rosTransformToYARPTransform(const yarp::rosmsg::geometry_msgs::TransformStamped &input, yarp::math::FrameTransform &output, bool isStatic)
{
    output.dst_frame_id = input.child_frame_id;
    output.src_frame_id = input.header.frame_id;
    output.timestamp = yarpStampFromROS(input.header.stamp);
    output.rotation.x() = input.transform.rotation.x;
    output.rotation.y() = input.transform.rotation.y;
    output.rotation.z() = input.transform.rotation.z;
    output.rotation.w() = input.transform.rotation.w;
    output.translation.tX = input.transform.translation.x;
    output.translation.tY = input.transform.translation.y;
    output.translation.tZ = input.transform.translation.z;
    output.isStatic = isStatic;
}

void FrameTransformGet_nwc_ros::receiveFrameTransforms()
{
    yarp::rosmsg::tf2_msgs::TFMessage* rosInData_timed = nullptr;
    do
    {
        rosInData_timed = m_rosSubscriberPort_tf_timed.read(false);
        if (rosInData_timed != nullptr)
        {
            std::vector <yarp::rosmsg::geometry_msgs::TransformStamped> tfs = rosInData_timed->transforms;
            for (auto& tf : tfs)
            {
                FrameTransform t;
                rosTransformToYARPTransform(tf,t,false);
                m_ftContainer.setTransform(t);
            }
        }
    } while (rosInData_timed != nullptr);
    yarp::rosmsg::tf2_msgs::TFMessage* rosInData_static = nullptr;
    do
    {
        rosInData_static = m_rosSubscriberPort_tf_static.read(false);
        if (rosInData_static != nullptr)
        {
            std::vector <yarp::rosmsg::geometry_msgs::TransformStamped> tfs = rosInData_static->transforms;
            for (auto& tf : tfs)
            {
                FrameTransform t;
                rosTransformToYARPTransform(tf,t,true);
                m_ftContainer.setTransform(t);
            }
        }
    } while (rosInData_static != nullptr);
}
