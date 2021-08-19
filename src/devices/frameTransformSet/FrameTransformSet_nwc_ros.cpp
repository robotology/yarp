/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameTransformSet_nwc_ros.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORSETNWCROS, "yarp.device.frameTransformSet_nwc_ros")
}

//------------------------------------------------------------------------------------------------------------------------------
FrameTransformSet_nwc_ros::FrameTransformSet_nwc_ros(double tperiod) :
PeriodicThread(tperiod),
m_period(tperiod)
{
}

bool FrameTransformSet_nwc_ros::open(yarp::os::Searchable& config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORSETNWCROS,"Error! YARP Network is not initialized");
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
        if (general_config.check("refresh_interval"))  {m_refreshInterval = general_config.find("refresh_interval").asFloat64();}
        if (general_config.check("asynch_pub"))        {m_asynchPub = general_config.find("asynch_pub").asInt16();}
    }
    m_ftContainer.m_timeout = m_refreshInterval;

    //ROS configuration
    if (config.check("ROS"))
    {
        yCInfo(FRAMETRANSFORSETNWCROS, "Configuring ROS params");
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
        if (!m_rosPublisherPort_tf_timed.topic(m_topic))
        {
            yCError(FRAMETRANSFORSETNWCROS) << "Unable to publish data on " << m_topic << " topic, check your yarp-ROS network configuration";
            return false;
        }
        if (!m_rosPublisherPort_tf_static.topic(m_topic_static))
        {
            yCError(FRAMETRANSFORSETNWCROS) << "Unable to publish data on " << m_topic_static << " topic, check your yarp-ROS network configuration";
            return false;
        }
    }
    else
    {
        //no ROS options
        yCWarning(FRAMETRANSFORSETNWCROS) << "ROS Group not configured";
        return false;
    }

    start();

    return true;
}

bool FrameTransformSet_nwc_ros::close()
{
    yCTrace(FRAMETRANSFORSETNWCROS, "Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    m_rosPublisherPort_tf_timed.close();
    m_rosPublisherPort_tf_static.close();
    m_rosNode = nullptr;
    return true;
}

void FrameTransformSet_nwc_ros::run()
{
    std::lock_guard <std::mutex> lg(m_trf_mutex);
    if(!m_ftContainer.checkAndRemoveExpired())
    {
        yCError(FRAMETRANSFORSETNWCROS,"Unable to remove expired transforms");
        return;
    }
    std::vector<yarp::math::FrameTransform> tempTfs;
    if(!m_ftContainer.getTransforms(tempTfs))
    {
        yCError(FRAMETRANSFORSETNWCROS,"Unable to get the transform vector)");
        return;
    }
    publishFrameTransforms(tempTfs);
}

bool FrameTransformSet_nwc_ros::setTransforms(const std::vector<yarp::math::FrameTransform>& transforms)
{
    std::lock_guard<std::mutex> lock(m_trf_mutex);
    if(!m_ftContainer.setTransforms(transforms))
    {
        yCError(FRAMETRANSFORSETNWCROS,"Unable to set transforms");
        return false;
    }
    if(m_asynchPub)
    {
        if(!m_ftContainer.checkAndRemoveExpired())
        {
            yCError(FRAMETRANSFORSETNWCROS,"Unable to remove expired transforms");
            return false;
        }

        std::vector<yarp::math::FrameTransform> tempTfs;
        if(!m_ftContainer.getTransforms(tempTfs))
        {
            yCError(FRAMETRANSFORSETNWCROS,"Unable to get the transform vector)");
            return false;
        }
        publishFrameTransforms(tempTfs);
    }
    return true;
}

bool FrameTransformSet_nwc_ros::setTransform(const yarp::math::FrameTransform& t)
{
    std::lock_guard<std::mutex> lock(m_trf_mutex);
    if(!m_ftContainer.setTransform(t))
    {
        yCError(FRAMETRANSFORSETNWCROS,"Unable to set transform");
        return false;
    }
    if(m_asynchPub)
    {
        if(!m_ftContainer.checkAndRemoveExpired())
        {
            yCError(FRAMETRANSFORSETNWCROS,"Unable to remove expired transforms");
            return false;
        }

        std::vector<yarp::math::FrameTransform> tempTfs;
        if(!m_ftContainer.getTransforms(tempTfs))
        {
            yCError(FRAMETRANSFORSETNWCROS,"Unable to get the transform vector)");
            return false;
        }
        publishFrameTransforms(tempTfs);
    }
    return true;
}

void  FrameTransformSet_nwc_ros::yarpTransformToROSTransform(const yarp::math::FrameTransform &input, yarp::rosmsg::geometry_msgs::TransformStamped& output)
{
    output.child_frame_id = input.dst_frame_id;
    output.header.frame_id = input.src_frame_id;;
    output.header.stamp = input.isStatic ? yarp::os::Time::now() : input.timestamp; ; //@@@check timestamp of static transform?
    output.transform.rotation.x = input.rotation.x();
    output.transform.rotation.y = input.rotation.y();
    output.transform.rotation.z = input.rotation.z();
    output.transform.rotation.w = input.rotation.w();
    output.transform.translation.x = input.translation.tX;
    output.transform.translation.y = input.translation.tY;
    output.transform.translation.z = input.translation.tZ;
}

void FrameTransformSet_nwc_ros::publishFrameTransforms(const std::vector<yarp::math::FrameTransform>& transforms)
{
    static int rosMsgCounter = 0; // Ask for clarification!

    yarp::rosmsg::tf2_msgs::TFMessage& rosOutTimedData = m_rosPublisherPort_tf_timed.prepare();
    yarp::rosmsg::tf2_msgs::TFMessage& rosOutStaticData = m_rosPublisherPort_tf_static.prepare();
    yarp::rosmsg::geometry_msgs::TransformStamped transform_timed;
    yarp::rosmsg::geometry_msgs::TransformStamped transform_static;
    rosOutTimedData.transforms.clear();
    rosOutStaticData.transforms.clear();

    for(auto& tf : transforms)
    {
        if(tf.isStatic)
        {
            yarpTransformToROSTransform(tf,transform_static);
            transform_static.header.seq = rosMsgCounter;
            rosOutStaticData.transforms.push_back(transform_static);
        }
        else
        {
            yarpTransformToROSTransform(tf,transform_timed);
            transform_timed.header.seq = rosMsgCounter;
            rosOutTimedData.transforms.push_back(transform_timed);
        }
    }
    m_rosPublisherPort_tf_timed.write();
    m_rosPublisherPort_tf_static.write();

    rosMsgCounter++;
}

bool FrameTransformSet_nwc_ros::deleteTransform(std::string t1, std::string t2)
{
    // Not yet implemented
    yCError(FRAMETRANSFORSETNWCROS, "deleteTransform not yet implemented");
    return false;
}

bool FrameTransformSet_nwc_ros::clearAll()
{
    // Not yet implemented
    yCError(FRAMETRANSFORSETNWCROS, "clearAll not yet implemented");
    return false;
}
