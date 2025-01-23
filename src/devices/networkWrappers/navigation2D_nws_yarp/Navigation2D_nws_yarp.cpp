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
#include "Navigation2D_nws_yarp.h"

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;

namespace {
YARP_LOG_COMPONENT(NAVIGATION2D_NWS_YARP, "yarp.device.navigation2D_nws_yarp")
}

Navigation2D_nws_yarp::Navigation2D_nws_yarp() : PeriodicThread(DEFAULT_THREAD_PERIOD)
{
    m_navigation_status=yarp::dev::Nav2D::navigation_status_idle;
    m_stats_time_last = yarp::os::Time::now();
    m_prev_navigation_status = getStatusAsString(m_navigation_status);
}

bool Navigation2D_nws_yarp::attach(PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(iNav_target);
        driver->view(iNav_ctrl);
        driver->view(iNav_vel);
    }

    if (nullptr == iNav_target ||
        nullptr == iNav_ctrl)
    {
        yCError(NAVIGATION2D_NWS_YARP, "Subdevice passed to attach method is invalid (it does not implement all the required interfaces)");
        return false;
    }

    if (nullptr == iNav_vel)
    {
        yCWarning(NAVIGATION2D_NWS_YARP, "The attached subdevice does not implement INavigation2DVelocityActions interface");
    }

    m_RPC = new INavigation2DRPCd(iNav_target, iNav_ctrl, iNav_vel);

    PeriodicThread::setPeriod(m_GENERAL_period);
    return PeriodicThread::start();
}

bool Navigation2D_nws_yarp::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    if (m_RPC)
    {
        delete m_RPC;
        m_RPC = nullptr;
    }

    m_statusPort.close();
    m_rpcPort.close();

    return true;
}

bool Navigation2D_nws_yarp::open(Searchable& config)
{
    if (!parseParams(config)) { return false; }

    m_rpcPortName = m_GENERAL_name + "/rpc";
    m_statusPortName = m_GENERAL_name + "/status:o";

    if (!initialize_YARP(config))
    {
        yCError(NAVIGATION2D_NWS_YARP) << "Error initializing YARP ports";
        return false;
    }

    yCInfo(NAVIGATION2D_NWS_YARP) << "Waiting for device to attach";

    return true;
}

bool Navigation2D_nws_yarp::initialize_YARP(yarp::os::Searchable &params)
{
    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yCError(NAVIGATION2D_NWS_YARP, "Failed to open port %s", m_rpcPortName.c_str());
        return false;
    }

    if(!m_statusPort.open(m_statusPortName.c_str()))
    {
        yCError(NAVIGATION2D_NWS_YARP, "Failed to open port %s", m_statusPortName.c_str());
        return false;
    }

    m_rpcPort.setReader(*this);
    return true;
}

bool Navigation2D_nws_yarp::close()
{
    yCTrace(NAVIGATION2D_NWS_YARP, "Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    detachAll();
    return true;
}

bool Navigation2D_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    if (!m_RPC) { return false; }

    bool b = m_RPC->read(connection);
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

void Navigation2D_nws_yarp::run()
{
    bool ok = iNav_ctrl->getNavigationStatus(m_navigation_status);

    double m_stats_time_curr = yarp::os::Time::now();

    std::string a_navigation_status = getStatusAsString(m_navigation_status);

    if(a_navigation_status != m_prev_navigation_status
        || m_stats_time_curr - m_stats_time_last > 5.0)
    {
        if (!ok)
        {
            yCError(NAVIGATION2D_NWS_YARP, "Unable to get Navigation Status!\n");
        }
        else
        {
            yCInfo(NAVIGATION2D_NWS_YARP) << "Running, ALL ok. Navigation status:" << a_navigation_status;
            updateStatusPort(a_navigation_status);
            m_prev_navigation_status = a_navigation_status;
        }
        m_stats_time_last = yarp::os::Time::now();
    }
}

std::string Navigation2D_nws_yarp::getStatusAsString(NavigationStatusEnum status)
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

void Navigation2D_nws_yarp::updateStatusPort(std::string& status)
{
    Bottle output_state;
    output_state.addString(status);
    m_statusPort.write(output_state);
}
