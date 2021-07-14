/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

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
#include "Localization2DServer.h"

#include <cmath>

/*! \file Localization2DServer.cpp */

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace std;

#define DEFAULT_THREAD_PERIOD 0.01

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {
YARP_LOG_COMPONENT(LOCALIZATION2DSERVER, "yarp.device.localization2DServer")
}

//------------------------------------------------------------------------------------------------------------------------------

Localization2DServer::Localization2DServer() : PeriodicThread(DEFAULT_THREAD_PERIOD)
{
    m_ros_node = nullptr;
    m_current_status = yarp::dev::Nav2D::LocalizationStatusEnum::localization_status_not_yet_localized;
    m_period = DEFAULT_THREAD_PERIOD;
    m_stats_time_last = yarp::os::Time::now();
    iLoc = nullptr;
    m_getdata_using_periodic_thread = true;
    m_ros_publish_odometry_on_topic = false;
    m_ros_publish_odometry_on_tf = false;
}

bool Localization2DServer::attachAll(const PolyDriverList &device2attach)
{
    if (device2attach.size() != 1)
    {
        yCError(LOCALIZATION2DSERVER, "Cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(iLoc);
    }

    if (nullptr == iLoc)
    {
        yCError(LOCALIZATION2DSERVER, "Subdevice passed to attach method is invalid");
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
        yCWarning(LOCALIZATION2DSERVER) << "Localization data not yet available during server initialization";
    }

    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}

bool Localization2DServer::detachAll()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    iLoc = nullptr;
    return true;
}

bool Localization2DServer::open(Searchable& config)
{
    yCWarning(LOCALIZATION2DSERVER) << "The 'localization2DServer' device is deprecated in favour of 'localization2D_nws_yarp'.";
    yCWarning(LOCALIZATION2DSERVER) << "The old device is no longer supported, and it will be deprecated in YARP 3.6 and removed in YARP 4.";
    yCWarning(LOCALIZATION2DSERVER) << "Please update your scripts.";

    Property params;
    params.fromString(config.toString().c_str());
    yCDebug(LOCALIZATION2DSERVER) << "Configuration: \n" << config.toString().c_str();

    if (config.check("GENERAL") == false)
    {
        yCWarning(LOCALIZATION2DSERVER) << "Missing GENERAL group, assuming default options";
    }

    Bottle& general_group = config.findGroup("GENERAL");
    if (!general_group.check("period"))
    {
        yCInfo(LOCALIZATION2DSERVER) << "Missing 'period' parameter. Using default value: " << DEFAULT_THREAD_PERIOD;
        m_period = DEFAULT_THREAD_PERIOD;
    }
    else
    {
        m_period = general_group.find("period").asFloat64();
        yCInfo(LOCALIZATION2DSERVER) << "Period requested: " << m_period;
    }

    if (!general_group.check("retrieve_position_periodically"))
    {
        yCInfo(LOCALIZATION2DSERVER) << "Missing 'retrieve_position_periodically' parameter. Using default value: true. Period:" << m_period ;
        m_getdata_using_periodic_thread = true;
    }
    else
    {
        m_getdata_using_periodic_thread = general_group.find("retrieve_position_periodically").asBool();
        if (m_getdata_using_periodic_thread)
            { yCInfo(LOCALIZATION2DSERVER) << "retrieve_position_periodically requested, Period:" << m_period; }
        else
            { yCInfo(LOCALIZATION2DSERVER) << "retrieve_position_periodically NOT requested. Localization data obtained asynchronously."; }
    }


    m_local_name = "/localizationServer";
    if (!general_group.check("name"))
    {
        yCInfo(LOCALIZATION2DSERVER) << "Missing 'name' parameter. Using default value: /localizationServer";
    }
    else
    {
        m_local_name = general_group.find("name").asString();
        if (m_local_name.c_str()[0] != '/') { yCError(LOCALIZATION2DSERVER) << "Missing '/' in name parameter" ;  return false; }
        yCInfo(LOCALIZATION2DSERVER) << "Using local name:" << m_local_name;
    }

    m_rpcPortName = m_local_name + "/rpc";
    m_2DLocationPortName = m_local_name + "/streaming:o";
    m_odometryPortName = m_local_name + "/odometry:o";

    if (config.check("subdevice"))
    {
        Property       p;
        PolyDriverList driverlist;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if (!pLoc.open(p) || !pLoc.isValid())
        {
            yCError(LOCALIZATION2DSERVER) << "Failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&pLoc, "1");
        if (!attachAll(driverlist))
        {
            yCError(LOCALIZATION2DSERVER) << "Failed to open subdevice.. check params";
            return false;
        }
    }
    else
    {
        yCInfo(LOCALIZATION2DSERVER) << "Waiting for device to attach";
    }
    m_stats_time_last = yarp::os::Time::now();

    if (!initialize_YARP(config))
    {
        yCError(LOCALIZATION2DSERVER) << "Error initializing YARP ports";
        return false;
    }

    if (!initialize_ROS(config))
    {
        yCError(LOCALIZATION2DSERVER) << "Error initializing ROS system";
        return false;
    }

    return true;
}

bool Localization2DServer::initialize_ROS(yarp::os::Searchable& params)
{
    if (params.check("ROS"))
    {
        Bottle& ros_group = params.findGroup("ROS");
        if (ros_group.check("publish_tf"))
        {
            m_ros_publish_odometry_on_tf=true;
        }
        if (ros_group.check("publish_odom"))
        {
            m_ros_publish_odometry_on_topic=true;
        }

        if (!ros_group.check("parent_frame_id"))
        {
            yCError(LOCALIZATION2DSERVER) << "Missing 'parent_frame_id' parameter";
            //return false;
        }
        else
        {
            m_parent_frame_id = ros_group.find("parent_frame_id").asString();
        }
        if (!ros_group.check("child_frame_id"))
        {
            yCError(LOCALIZATION2DSERVER) << "Missing 'child_frame_id' parameter";
            //return false;
        }
        else
        {
            m_child_frame_id = ros_group.find("child_frame_id").asString();
        }

    }
    else
    {
        yCInfo(LOCALIZATION2DSERVER) << "ROS initialization not requested";
        return true;
    }

    if (m_ros_node == nullptr)
    {
        bool b= false;
        m_ros_node = new yarp::os::Node(m_local_name+"_ROSnode");
        if (m_ros_node == nullptr)
        {
            yCError(LOCALIZATION2DSERVER) << "Opening " << m_local_name << " Node, check your yarp-ROS network configuration";
        }

        string ros_odom_topic = m_local_name + string("/odom");
        b = m_odometry_publisher.topic(ros_odom_topic);
        if (!b)
        {
            yCError(LOCALIZATION2DSERVER) << "Unable to publish data on" << ros_odom_topic << "topic";
        }
        b = m_tf_publisher.topic("/tf");
        if (!b)
        {
            yCError(LOCALIZATION2DSERVER) << "Unable to publish data on /tf topic";
        }
        yCInfo(LOCALIZATION2DSERVER) << "ROS initialized";
    }
    return true;
}

bool Localization2DServer::initialize_YARP(yarp::os::Searchable &params)
{
    if (!m_2DLocationPort.open(m_2DLocationPortName.c_str()))
    {
        yCError(LOCALIZATION2DSERVER, "Failed to open port %s", m_2DLocationPortName.c_str());
        return false;
    }

    if (!m_odometryPort.open(m_odometryPortName.c_str()))
    {
        yCError(LOCALIZATION2DSERVER, "Failed to open port %s", m_odometryPortName.c_str());
        return false;
    }

    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yCError(LOCALIZATION2DSERVER, "Failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    return true;
}

bool Localization2DServer::close()
{
    yCTrace(LOCALIZATION2DSERVER, "Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    detachAll();

    m_2DLocationPort.interrupt();
    m_2DLocationPort.close();
    m_odometryPort.interrupt();
    m_odometryPort.close();
    m_rpcPort.interrupt();
    m_rpcPort.close();

    if (m_ros_node)
    {
        m_tf_publisher.close();
        m_odometry_publisher.close();
        delete m_ros_node;
        m_ros_node = nullptr;
    }

    yCDebug(LOCALIZATION2DSERVER) << "Execution terminated";
    return true;
}

bool Localization2DServer::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) {
        return false;
    }

    reply.clear();

    if (command.get(0).isVocab32())
    {
        if (command.get(0).asVocab32() == VOCAB_INAVIGATION && command.get(1).isVocab32())
        {
            int request = command.get(1).asVocab32();
            if (request == VOCAB_NAV_GET_CURRENT_POS)
            {
                bool b = true;
                if (m_getdata_using_periodic_thread)
                {
                    //m_current_position is obtained by run()
                }
                else
                {
                    //m_current_position is obtained by getCurrentPosition()
                    b = iLoc->getCurrentPosition(m_current_position);
                }
                if (b)
                {
                    reply.addVocab32(VOCAB_OK);
                    reply.addString(m_current_position.map_id);
                    reply.addFloat64(m_current_position.x);
                    reply.addFloat64(m_current_position.y);
                    reply.addFloat64(m_current_position.theta);
                }
                else
                {
                    reply.addVocab32(VOCAB_ERR);
                }
            }
            else if (request == VOCAB_NAV_GET_ESTIMATED_ODOM)
            {
                bool b = true;
                if (m_getdata_using_periodic_thread)
                {
                    //m_current_position is obtained by run()
                }
                else
                {
                    //m_current_position is obtained by getCurrentPosition()
                    b = iLoc->getEstimatedOdometry(m_current_odometry);
                }
                if (b)
                {
                    reply.addVocab32(VOCAB_OK);
                    reply.addFloat64(m_current_odometry.odom_x);
                    reply.addFloat64(m_current_odometry.odom_y);
                    reply.addFloat64(m_current_odometry.odom_theta);
                    reply.addFloat64(m_current_odometry.base_vel_x);
                    reply.addFloat64(m_current_odometry.base_vel_y);
                    reply.addFloat64(m_current_odometry.base_vel_theta);
                    reply.addFloat64(m_current_odometry.odom_vel_x);
                    reply.addFloat64(m_current_odometry.odom_vel_y);
                    reply.addFloat64(m_current_odometry.odom_vel_theta);
                }
                else
                {
                    reply.addVocab32(VOCAB_ERR);
                }
            }
            else if (request == VOCAB_NAV_SET_INITIAL_POS)
            {
                Map2DLocation init_loc;
                init_loc.map_id = command.get(2).asString();
                init_loc.x = command.get(3).asFloat64();
                init_loc.y = command.get(4).asFloat64();
                init_loc.theta = command.get(5).asFloat64();
                iLoc->setInitialPose(init_loc);
                reply.addVocab32(VOCAB_OK);
            }
            else if (request == VOCAB_NAV_GET_CURRENT_POSCOV)
            {
                Map2DLocation init_loc;
                yarp::sig::Matrix cov(3, 3);
                iLoc->getCurrentPosition(init_loc, cov);
                reply.addVocab32(VOCAB_OK);
                reply.addString(m_current_position.map_id);
                reply.addFloat64(m_current_position.x);
                reply.addFloat64(m_current_position.y);
                reply.addFloat64(m_current_position.theta);
                yarp::os::Bottle& mc = reply.addList();
                for (size_t i = 0; i < 3; i++) { for (size_t j = 0; j < 3; j++) { mc.addFloat64(cov[i][j]); } }
            }
            else if (request == VOCAB_NAV_SET_INITIAL_POSCOV)
            {
                Map2DLocation init_loc;
                yarp::sig::Matrix cov(3,3);
                init_loc.map_id = command.get(2).asString();
                init_loc.x = command.get(3).asFloat64();
                init_loc.y = command.get(4).asFloat64();
                init_loc.theta = command.get(5).asFloat64();
                Bottle* mc = command.get(6).asList();
                if (mc!=nullptr && mc->size() == 9)
                {
                    for (size_t i = 0; i < 3; i++) { for (size_t j = 0; j < 3; j++) { cov[i][j] = mc->get(i * 3 + j).asFloat64(); } }
                    bool ret = iLoc->setInitialPose(init_loc, cov);
                    if (ret) { reply.addVocab32(VOCAB_OK); }
                    else     { reply.addVocab32(VOCAB_ERR); }
                }
                else
                {
                    reply.addVocab32(VOCAB_ERR);
                }
            }
            else if (request == VOCAB_NAV_LOCALIZATION_START)
            {
                iLoc->startLocalizationService();
                reply.addVocab32(VOCAB_OK);
            }
            else if (request == VOCAB_NAV_LOCALIZATION_STOP)
            {
                iLoc->stopLocalizationService();
                reply.addVocab32(VOCAB_OK);
            }
            else if (request == VOCAB_NAV_GET_LOCALIZER_STATUS)
            {
                if (m_getdata_using_periodic_thread)
                {
                    //m_current_status is obtained by run()
                    reply.addVocab32(VOCAB_OK);
                    reply.addVocab32(m_current_status);
                }
                else
                {
                    //m_current_status is obtained by getLocalizationStatus()
                    iLoc->getLocalizationStatus(m_current_status);
                    reply.addVocab32(VOCAB_OK);
                    reply.addVocab32(m_current_status);
                }
            }
            else if (request == VOCAB_NAV_GET_LOCALIZER_POSES)
            {
                std::vector<Map2DLocation> poses;
                iLoc->getEstimatedPoses(poses);
                reply.addVocab32(VOCAB_OK);
                reply.addInt32(poses.size());
                for (size_t i=0; i<poses.size(); i++)
                {
                    Bottle& b = reply.addList();
                    b.addString(poses[i].map_id);
                    b.addFloat64(poses[i].x);
                    b.addFloat64(poses[i].y);
                    b.addFloat64(poses[i].theta);
                }
            }
            else
            {
                reply.addVocab32(VOCAB_ERR);
            }
        }
        else
        {
            yCError(LOCALIZATION2DSERVER) << "Invalid vocab received";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (command.get(0).isString() && command.get(0).asString() == "help")
    {
        reply.addVocab32("many");
        reply.addString("Available commands are:");
        reply.addString("getLoc");
        reply.addString("initLoc <map_name> <x> <y> <angle in degrees>");
    }
    else if (command.get(0).isString() && command.get(0).asString() == "getLoc")
    {
        Map2DLocation curr_loc;
        iLoc->getCurrentPosition(curr_loc);
        std::string s = std::string("Current Location is: ") + curr_loc.toString();
        reply.addString(s);
    }
    else if (command.get(0).isString() && command.get(0).asString() == "initLoc")
    {
        Map2DLocation init_loc;
        init_loc.map_id = command.get(1).asString();
        init_loc.x = command.get(2).asFloat64();
        init_loc.y = command.get(3).asFloat64();
        init_loc.theta = command.get(4).asFloat64();
        iLoc->setInitialPose(init_loc);
        std::string s = std::string("Localization initialized to: ") + init_loc.toString();
        reply.addString(s);
    }
    else
    {
        yCError(LOCALIZATION2DSERVER) << "Invalid command type";
        reply.addVocab32(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }

    return true;
}

void Localization2DServer::run()
{
    double m_stats_time_curr = yarp::os::Time::now();
    if (m_stats_time_curr - m_stats_time_last > 5.0)
    {
        yCInfo(LOCALIZATION2DSERVER) << "Running";
        m_stats_time_last = yarp::os::Time::now();
    }

    if (m_getdata_using_periodic_thread)
    {
        bool ret = iLoc->getLocalizationStatus(m_current_status);
        if (ret == false)
        {
            yCError(LOCALIZATION2DSERVER) << "getLocalizationStatus() failed";
        }

        if (m_current_status == LocalizationStatusEnum::localization_status_localized_ok)
        {
            //update the stamp


            bool ret2 = iLoc->getCurrentPosition(m_current_position);
            if (ret2 == false)
            {
                yCError(LOCALIZATION2DSERVER) << "getCurrentPosition() failed";
            }
            else
            {
                m_loc_stamp.update();
            }
            bool ret3 = iLoc->getEstimatedOdometry(m_current_odometry);
            if (ret3 == false)
            {
                //yCError(LOCALIZATION2DSERVER) << "getEstimatedOdometry() failed";
            }
            else
            {
                m_odom_stamp.update();
            }
        }
        else
        {
            yCWarning(LOCALIZATION2DSERVER, "The system is not properly localized!");
        }
    }

    if (1) {
        publish_odometry_on_yarp_port();
    }
    if (1) {
        publish_2DLocation_on_yarp_port();
    }
    if (m_ros_publish_odometry_on_topic) {
        publish_odometry_on_ROS_topic();
    }
    if (m_ros_publish_odometry_on_tf) {
        publish_odometry_on_TF_topic();
    }
}

void Localization2DServer::publish_odometry_on_yarp_port()
{
    if (m_odometryPort.getOutputCount() > 0)
    {
        yarp::dev::OdometryData& odom = m_odometryPort.prepare();
        odom = m_current_odometry;

        //send data to port
        m_odometryPort.setEnvelope(m_odom_stamp);
        m_odometryPort.write();
    }
}

void Localization2DServer::publish_2DLocation_on_yarp_port()
{
    if (m_2DLocationPort.getOutputCount() > 0)
    {
        Nav2D::Map2DLocation& loc = m_2DLocationPort.prepare();
        if (m_current_status == LocalizationStatusEnum::localization_status_localized_ok)
        {
            loc = m_current_position;
        }
        else
        {
            Map2DLocation temp_loc;
            temp_loc.x = std::nan("");
            temp_loc.y = std::nan("");
            temp_loc.theta = std::nan("");
            loc = temp_loc;
        }

        //send data to port
        m_2DLocationPort.setEnvelope(m_loc_stamp);
        m_2DLocationPort.write();
    }
}

void Localization2DServer::publish_odometry_on_TF_topic()
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

void Localization2DServer::publish_odometry_on_ROS_topic()
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
