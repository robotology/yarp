/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define _USE_MATH_DEFINES

#include "Localization2D_nws_yarp.h"

#include <yarp/os/Bottle.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IFrameTransform.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/math/Math.h>

#include <cmath>

/*! \file Localization2D_nws_yarp.cpp */

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace std;

#define DEFAULT_THREAD_PERIOD 0.01

namespace
{
    YARP_LOG_COMPONENT(LOCALIZATION2D_NWS_YARP, "yarp.device.localization2D_nws_yarp")
}

//------------------------------------------------------------------------------------------------------------------------------

Localization2D_nws_yarp::Localization2D_nws_yarp() : PeriodicThread(DEFAULT_THREAD_PERIOD),
                                                     m_period(DEFAULT_THREAD_PERIOD)
{
    m_stats_time_last = yarp::os::Time::now();
}

bool Localization2D_nws_yarp::attach(PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(iLoc);
    }

    if (nullptr == iLoc)
    {
        yCError(LOCALIZATION2D_NWS_YARP, "Subdevice passed to attach method is invalid");
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
        yCWarning(LOCALIZATION2D_NWS_YARP) << "Localization data not yet available during server initialization";
    }

    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}

bool Localization2D_nws_yarp::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    iLoc = nullptr;
    return true;
}

bool Localization2D_nws_yarp::open(Searchable& config)
{
    Property params;
    params.fromString(config.toString().c_str());
    yCDebug(LOCALIZATION2D_NWS_YARP) << "Configuration: \n" << config.toString().c_str();

    if (config.check("GENERAL") == false)
    {
        yCWarning(LOCALIZATION2D_NWS_YARP) << "Missing GENERAL group, assuming default options";
    }

    Bottle& general_group = config.findGroup("GENERAL");
    if (!general_group.check("period"))
    {
        yCInfo(LOCALIZATION2D_NWS_YARP) << "Missing 'period' parameter. Using default value: " << DEFAULT_THREAD_PERIOD;
        m_period = DEFAULT_THREAD_PERIOD;
    }
    else
    {
        m_period = general_group.find("period").asFloat64();
        yCInfo(LOCALIZATION2D_NWS_YARP) << "Period requested: " << m_period;
    }

    if (!general_group.check("publish_odometry"))
    {
        m_enable_publish_odometry = general_group.find("publish_odometry").asBool();
        yCInfo(LOCALIZATION2D_NWS_YARP) << "publish_odometry=" << m_enable_publish_odometry;
    }
    if (!general_group.check("publish_location"))
    {
        m_enable_publish_location = general_group.find("publish_location").asBool();
        yCInfo(LOCALIZATION2D_NWS_YARP) << "publish_location=" << m_enable_publish_location;
    }

    if (!general_group.check("retrieve_position_periodically"))
    {
        yCInfo(LOCALIZATION2D_NWS_YARP) << "Missing 'retrieve_position_periodically' parameter. Using default value: true. Period:" << m_period ;
        m_getdata_using_periodic_thread = true;
    }
    else
    {
        m_getdata_using_periodic_thread = general_group.find("retrieve_position_periodically").asBool();
        if (m_getdata_using_periodic_thread)
            { yCInfo(LOCALIZATION2D_NWS_YARP) << "retrieve_position_periodically requested, Period:" << m_period; }
        else
            { yCInfo(LOCALIZATION2D_NWS_YARP) << "retrieve_position_periodically NOT requested. Localization data obtained asynchronously."; }
    }

    if (!general_group.check("name"))
    {
        yCInfo(LOCALIZATION2D_NWS_YARP) << "Missing 'name' parameter. Using default value:" << m_local_name;
    }
    else
    {
        m_local_name = general_group.find("name").asString();
        if (m_local_name.c_str()[0] != '/') { yCError(LOCALIZATION2D_NWS_YARP) << "Missing '/' in name parameter" ;  return false; }
        yCInfo(LOCALIZATION2D_NWS_YARP) << "Using local name:" << m_local_name;
    }

    m_rpcPortName = m_local_name + "/rpc";
    m_2DLocationPortName = m_local_name + "/streaming:o";
    m_odometryPortName = m_local_name + "/odometry:o";

    if (config.check("subdevice"))
    {
        Property       p;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if (!pLoc.open(p) || !pLoc.isValid())
        {
            yCError(LOCALIZATION2D_NWS_YARP) << "Failed to open subdevice.. check params";
            return false;
        }

        if (!attach(&pLoc))
        {
            yCError(LOCALIZATION2D_NWS_YARP) << "Failed to open subdevice.. check params";
            return false;
        }
    }
    else
    {
        yCInfo(LOCALIZATION2D_NWS_YARP) << "Waiting for device to attach";
    }
    m_stats_time_last = yarp::os::Time::now();

    if (!initialize_YARP(config))
    {
        yCError(LOCALIZATION2D_NWS_YARP) << "Error initializing YARP ports";
        return false;
    }

    return true;
}

bool Localization2D_nws_yarp::initialize_YARP(yarp::os::Searchable &params)
{
    if (!m_2DLocationPort.open(m_2DLocationPortName.c_str()))
    {
        yCError(LOCALIZATION2D_NWS_YARP, "Failed to open port %s", m_2DLocationPortName.c_str());
        return false;
    }

    if (!m_odometryPort.open(m_odometryPortName.c_str()))
    {
        yCError(LOCALIZATION2D_NWS_YARP, "Failed to open port %s", m_odometryPortName.c_str());
        return false;
    }

    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yCError(LOCALIZATION2D_NWS_YARP, "Failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    return true;
}

bool Localization2D_nws_yarp::close()
{
    yCTrace(LOCALIZATION2D_NWS_YARP, "Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    detach();

    m_2DLocationPort.interrupt();
    m_2DLocationPort.close();
    m_odometryPort.interrupt();
    m_odometryPort.close();
    m_rpcPort.interrupt();
    m_rpcPort.close();

    yCDebug(LOCALIZATION2D_NWS_YARP) << "Execution terminated";
    return true;
}

bool Localization2D_nws_yarp::read(yarp::os::ConnectionReader& connection)
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
            yCError(LOCALIZATION2D_NWS_YARP) << "Invalid vocab received";
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
        yCError(LOCALIZATION2D_NWS_YARP) << "Invalid command type";
        reply.addVocab32(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }

    return true;
}

void Localization2D_nws_yarp::run()
{
    double m_stats_time_curr = yarp::os::Time::now();
    if (m_stats_time_curr - m_stats_time_last > 5.0)
    {
        yCInfo(LOCALIZATION2D_NWS_YARP) << "Running";
        m_stats_time_last = yarp::os::Time::now();
    }

    if (m_getdata_using_periodic_thread)
    {
        bool ret = iLoc->getLocalizationStatus(m_current_status);
        if (ret == false)
        {
            yCError(LOCALIZATION2D_NWS_YARP) << "getLocalizationStatus() failed";
        }

        if (m_current_status == LocalizationStatusEnum::localization_status_localized_ok)
        {
            bool ret2 = iLoc->getCurrentPosition(m_current_position);
            if (ret2 == false)
            {
                yCError(LOCALIZATION2D_NWS_YARP) << "getCurrentPosition() failed";
            }
            else
            {
                m_loc_stamp.update();
            }
            bool ret3 = iLoc->getEstimatedOdometry(m_current_odometry);
            if (ret3 == false)
            {
                //yCError(LOCALIZATION2D_NWS_YARP) << "getEstimatedOdometry() failed";
            }
            else
            {
                m_odom_stamp.update();
            }
        }
        else
        {
            yCWarning(LOCALIZATION2D_NWS_YARP, "The system is not properly localized!");
        }
    }

    if (m_enable_publish_odometry) {
        publish_odometry_on_yarp_port();
    }
    if (m_enable_publish_location) {
        publish_2DLocation_on_yarp_port();
    }
}

void Localization2D_nws_yarp::publish_odometry_on_yarp_port()
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

void Localization2D_nws_yarp::publish_2DLocation_on_yarp_port()
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
