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

Localization2D_nws_yarp::Localization2D_nws_yarp() : PeriodicThread(DEFAULT_THREAD_PERIOD)
{
    m_stats_time_last = yarp::os::Time::now();
}

bool Localization2D_nws_yarp::attach(PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(iLoc);
        m_RPC = new ILocalization2DRPCd(iLoc);
        m_RPC->m_getdata_using_periodic_thread = m_getdata_using_periodic_thread;
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
        m_RPC->m_current_status = status;
        m_RPC->m_current_position = loc;
    }
    else
    {
        yCWarning(LOCALIZATION2D_NWS_YARP) << "Localization data not yet available during server initialization";
    }

    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yCError(LOCALIZATION2D_NWS_YARP, "Failed to open port %s", m_rpcPortName.c_str());
        return false;
    }

    PeriodicThread::setPeriod(m_GENERAL_period);
    return PeriodicThread::start();
}

bool Localization2D_nws_yarp::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    m_rpcPort.interrupt();
    m_rpcPort.close();
    if (m_RPC)
    {
        delete m_RPC;
        m_RPC = nullptr;
    }

    iLoc = nullptr;
    return true;
}

bool Localization2D_nws_yarp::open(Searchable& config)
{
    if (!parseParams(config)) { return false; }

    //check some parameters consistency
    if (!m_GENERAL_retrieve_position_periodically)
    {
        m_getdata_using_periodic_thread = false;

        if (!m_GENERAL_publish_odometry)
           {yCWarning(LOCALIZATION2D_NWS_YARP) << "retrieve_position_periodically is true, but data is not published because publish_odometry is false. This configuration is strange";}
        if (!m_GENERAL_publish_location)
           {yCWarning(LOCALIZATION2D_NWS_YARP) << "retrieve_position_periodically is true, but data is not published because publish_location is false. This configuration is strange";}
    }
    else
    {
        m_getdata_using_periodic_thread = true;
    }

    //Some debug prints
    if (m_getdata_using_periodic_thread)
    {
        yCInfo(LOCALIZATION2D_NWS_YARP) << "retrieve_position_periodically requested, Period:" << m_GENERAL_period;
    }
    else
    {
        yCInfo(LOCALIZATION2D_NWS_YARP) << "retrieve_position_periodically NOT requested. Localization data obtained asynchronously.";
    }


    m_local_name = m_GENERAL_name;
    m_rpcPortName = m_local_name + "/rpc";
    m_2DLocationPortName = m_local_name + "/streaming:o";
    m_odometryPortName = m_local_name + "/odometry:o";

    m_stats_time_last = yarp::os::Time::now();

    m_rpcPort.setReader(*this);

    yCInfo(LOCALIZATION2D_NWS_YARP) << "Waiting for device to attach";

    return true;
}

bool Localization2D_nws_yarp::threadInit()
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
    return true;
}

void Localization2D_nws_yarp::threadRelease()
{
    yarp::os::Time::delay(1.0);
//    m_2DLocationPort.interrupt();
    m_2DLocationPort.close();
//    m_odometryPort.interrupt();
    m_odometryPort.close();
}

bool Localization2D_nws_yarp::close()
{
    yCTrace(LOCALIZATION2D_NWS_YARP, "Close");

    detach();

    yCDebug(LOCALIZATION2D_NWS_YARP) << "Execution terminated";
    return true;
}

bool Localization2D_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    if (!m_RPC) { return false;}

    bool b = m_RPC->read(connection);
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
    if (!m_RPC) { return;}
    m_RPC->getMutex()->lock();
    {
        if (m_RPC->m_getdata_using_periodic_thread)
        {
            bool ret = iLoc->getLocalizationStatus(m_RPC->m_current_status);
            if (ret == false)
            {
                yCError(LOCALIZATION2D_NWS_YARP) << "getLocalizationStatus() failed";
            }

            if (m_RPC->m_current_status == LocalizationStatusEnum::localization_status_localized_ok)
            {
                bool ret2 = iLoc->getCurrentPosition(m_RPC->m_current_position);
                if (ret2 == false)
                {
                    yCError(LOCALIZATION2D_NWS_YARP) << "getCurrentPosition() failed";
                }
                else
                {
                    m_RPC->m_loc_stamp.update();
                }
                bool ret3 = iLoc->getEstimatedOdometry(m_RPC->m_current_odometry);
                if (ret3 == false)
                {
                    //yCError(LOCALIZATION2D_NWS_YARP) << "getEstimatedOdometry() failed";
                }
                else
                {
                    m_RPC->m_odom_stamp.update();
                }
            }
            else
            {
                yCWarning(LOCALIZATION2D_NWS_YARP, "The system is not properly localized!");
            }
        }
    }
    m_RPC->getMutex()->unlock();

    if (m_GENERAL_publish_odometry) {
        publish_odometry_on_yarp_port();
    }
    if (m_GENERAL_publish_location) {
        publish_2DLocation_on_yarp_port();
    }
}

void Localization2D_nws_yarp::publish_odometry_on_yarp_port()
{
    if (!m_RPC) return;

    if (m_odometryPort.getOutputCount() > 0)
    {
        yarp::dev::OdometryData& odom = m_odometryPort.prepare();
        odom = m_RPC->m_current_odometry;

        //send data to port
        m_odometryPort.setEnvelope(m_RPC->m_odom_stamp);
        m_odometryPort.write();
    }
}

void Localization2D_nws_yarp::publish_2DLocation_on_yarp_port()
{
    if (!m_RPC) return;

    if (m_2DLocationPort.getOutputCount() > 0)
    {
        Nav2D::Map2DLocation& loc = m_2DLocationPort.prepare();
        if (m_RPC->m_current_status == LocalizationStatusEnum::localization_status_localized_ok)
        {
            loc = m_RPC->m_current_position;
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
        m_2DLocationPort.setEnvelope(m_RPC->m_loc_stamp);
        m_2DLocationPort.write();
    }
}
