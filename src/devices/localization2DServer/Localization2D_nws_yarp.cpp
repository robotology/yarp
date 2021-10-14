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
        m_RPC.setInterface(iLoc);
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
        m_RPC.m_current_status = status;
        m_RPC.m_current_position = loc;
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
        m_RPC.m_getdata_using_periodic_thread = true;
    }
    else
    {
        m_RPC.m_getdata_using_periodic_thread = general_group.find("retrieve_position_periodically").asBool();
        if (m_RPC.m_getdata_using_periodic_thread)
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
            yCError(LOCALIZATION2D_NWS_YARP) << "Failed to attach subdevice.. check params";
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
    bool b = m_RPC.read(connection);
    if (b)
    {
        return true;
    }
    else
    {
        yCDebug(LOCALIZATION2D_NWS_YARP) << "read() Command failed";
        return false;
    }
}

void Localization2D_nws_yarp::run()
{
    double m_stats_time_curr = yarp::os::Time::now();
    if (m_stats_time_curr - m_stats_time_last > 5.0)
    {
        yCInfo(LOCALIZATION2D_NWS_YARP) << "Running";
        m_stats_time_last = yarp::os::Time::now();
    }

    //enter the critical section
    m_RPC.getMutex()->lock();
    {
        if (m_RPC.m_getdata_using_periodic_thread)
        {
            bool ret = iLoc->getLocalizationStatus(m_RPC.m_current_status);
            if (ret == false)
            {
                yCError(LOCALIZATION2D_NWS_YARP) << "getLocalizationStatus() failed";
            }

            if (m_RPC.m_current_status == LocalizationStatusEnum::localization_status_localized_ok)
            {
                bool ret2 = iLoc->getCurrentPosition(m_RPC.m_current_position);
                if (ret2 == false)
                {
                    yCError(LOCALIZATION2D_NWS_YARP) << "getCurrentPosition() failed";
                }
                else
                {
                    m_RPC.m_loc_stamp.update();
                }
                bool ret3 = iLoc->getEstimatedOdometry(m_RPC.m_current_odometry);
                if (ret3 == false)
                {
                    //yCError(LOCALIZATION2D_NWS_YARP) << "getEstimatedOdometry() failed";
                }
                else
                {
                    m_RPC.m_odom_stamp.update();
                }
            }
            else
            {
                yCWarning(LOCALIZATION2D_NWS_YARP, "The system is not properly localized!");
            }
        }
    }
    m_RPC.getMutex()->unlock();

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
        odom = m_RPC.m_current_odometry;

        //send data to port
        m_odometryPort.setEnvelope(m_RPC.m_odom_stamp);
        m_odometryPort.write();
    }
}

void Localization2D_nws_yarp::publish_2DLocation_on_yarp_port()
{
    if (m_2DLocationPort.getOutputCount() > 0)
    {
        Nav2D::Map2DLocation& loc = m_2DLocationPort.prepare();
        if (m_RPC.m_current_status == LocalizationStatusEnum::localization_status_localized_ok)
        {
            loc = m_RPC.m_current_position;
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
        m_2DLocationPort.setEnvelope(m_RPC.m_loc_stamp);
        m_2DLocationPort.write();
    }
}
