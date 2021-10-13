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
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/MapGrid2D.h>
#include <math.h>
#include <cmath>
#include "navigation2D_nws_yarp.h"

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;

namespace {
YARP_LOG_COMPONENT(NAVIGATION2D_NWS_YARP, "yarp.device.navigation2D_nws_yarp")
}

navigation2D_nws_yarp::navigation2D_nws_yarp() : PeriodicThread(DEFAULT_THREAD_PERIOD)
{
    m_navigation_status=yarp::dev::Nav2D::navigation_status_idle;
    m_stats_time_last = yarp::os::Time::now();
}

bool navigation2D_nws_yarp::attach(PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(iNav_target);
        driver->view(iNav_ctrl);
        driver->view(iNav_vel);
        driver->view(iNav_extra);
    }

    if (nullptr == iNav_target ||
        nullptr == iNav_ctrl ||
        nullptr == iNav_vel ||
        nullptr == iNav_extra)
    {
        yCError(NAVIGATION2D_NWS_YARP, "Subdevice passed to attach method is invalid");
        return false;
    }

    m_RPC.setInterfaces(iNav_target, iNav_ctrl, iNav_vel, iNav_extra);

    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}

bool navigation2D_nws_yarp::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    return true;
}

bool navigation2D_nws_yarp::open(Searchable& config)
{
    Property params;
    params.fromString(config.toString().c_str());
    yCDebug(NAVIGATION2D_NWS_YARP) << "Configuration: \n" << config.toString().c_str();

    if (config.check("GENERAL") == false)
    {
        yCWarning(NAVIGATION2D_NWS_YARP) << "Missing GENERAL group, assuming default options";
    }

    Bottle& general_group = config.findGroup("GENERAL");
    if (!general_group.check("period"))
    {
        yCInfo(NAVIGATION2D_NWS_YARP) << "Missing 'period' parameter. Using default value: 0.010";
        m_period = 0.010;
    }
    else
    {
        m_period = config.find("period").asFloat64();
    }

    if (!general_group.check("name"))
    {
        yCInfo(NAVIGATION2D_NWS_YARP) << "Missing 'name' parameter. Using default value: " << m_local_name;
    }
    else
    {
        m_local_name = config.find("name").asString();
        if (m_local_name.c_str()[0] != '/') { yCError(NAVIGATION2D_NWS_YARP) << "Missing '/' in name parameter";  return false; }
        yCInfo(NAVIGATION2D_NWS_YARP) << "Using local name:" << m_local_name;
    }
    m_rpcPortName = m_local_name + "/rpc";

    if (config.check("subdevice"))
    {
        Property       p;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if (!pNav.open(p) || !pNav.isValid())
        {
            yCError(NAVIGATION2D_NWS_YARP) << "Failed to open subdevice.. check params";
            return false;
        }

        if (!attach(&pNav))
        {
            yCError(NAVIGATION2D_NWS_YARP) << "Failed to attach subdevice.. check params";
            return false;
        }
    }
    else
    {
        yCInfo(NAVIGATION2D_NWS_YARP) << "Waiting for device to attach";
    }

    if (!initialize_YARP(config))
    {
        yCError(NAVIGATION2D_NWS_YARP) << "Error initializing YARP ports";
        return false;
    }

    return true;
}

bool navigation2D_nws_yarp::initialize_YARP(yarp::os::Searchable &params)
{
    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yCError(NAVIGATION2D_NWS_YARP, "Failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);
    return true;
}

bool navigation2D_nws_yarp::close()
{
    yCTrace(NAVIGATION2D_NWS_YARP, "Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    detachAll();
    return true;
}

bool navigation2D_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    bool b = m_RPC.read(connection);
    if (b)
    {
        return true;
    }
    else
    {
        yCDebug(NAVIGATION2D_NWS_YARP) << "read() Command failed";
        return false;
    }
}

void navigation2D_nws_yarp::run()
{
    bool ok = iNav_ctrl->getNavigationStatus(m_navigation_status);

    double m_stats_time_curr = yarp::os::Time::now();
    if (m_stats_time_curr - m_stats_time_last > 5.0)
    {
        if (!ok)
        {
            yCError(NAVIGATION2D_NWS_YARP, "Unable to get Navigation Status!\n");
        }
        else
        {
            yCInfo(NAVIGATION2D_NWS_YARP) << "Running, ALL ok. Navigation status:" << getStatusAsString(m_navigation_status);
        }
        m_stats_time_last = yarp::os::Time::now();
    }
}

std::string navigation2D_nws_yarp::getStatusAsString(NavigationStatusEnum status)
{
    if (status == navigation_status_idle) {
        return std::string("navigation_status_idle");
    } else if (status == navigation_status_moving) {
        return std::string("navigation_status_moving");
    } else if (status == navigation_status_waiting_obstacle) {
        return std::string("navigation_status_waiting_obstacle");
    } else if (status == navigation_status_goal_reached) {
        return std::string("navigation_status_goal_reached");
    } else if (status == navigation_status_aborted) {
        return std::string("navigation_status_aborted");
    } else if (status == navigation_status_failing) {
        return std::string("navigation_status_failing");
    } else if (status == navigation_status_paused) {
        return std::string("navigation_status_paused");
    } else if (status == navigation_status_preparing_before_move) {
        return std::string("navigation_status_preparing_before_move");
    } else if (status == navigation_status_thinking) {
        return std::string("navigation_status_thinking");
    } else if (status == navigation_status_error) {
        return std::string("navigation_status_error");
    }
    return std::string("navigation_status_error");
}
