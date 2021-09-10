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
    }

    if (nullptr == iNav_target ||
        nullptr == iNav_ctrl ||
        nullptr == iNav_vel)
    {
        yCError(NAVIGATION2D_NWS_YARP, "Subdevice passed to attach method is invalid");
        return false;
    }

#if DEF_m_RPC
    m_RPC.setInterfaces(iNav_target, iNav_ctrl, iNav_vel);
#endif

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

bool navigation2D_nws_yarp::parse_respond_vocab(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
    if (command.get(0).isVocab32() == false)
    {
        yCError(NAVIGATION2D_NWS_YARP) << "General error in navigation2D_nws_yarp::parse_respond_vocab()";
        return false;
    }

    if (command.get(0).asVocab32() != VOCAB_INAVIGATION ||
        command.get(1).isVocab32() == false)
    {
        yCError(NAVIGATION2D_NWS_YARP) << "Invalid vocab received";
        reply.addVocab32(VOCAB_ERR);
        return true;
    }

    int request = command.get(1).asVocab32();
    if (request == VOCAB_NAV_GOTOABS)
    {
        Map2DLocation loc;
        loc.map_id = command.get(2).asString();
        loc.x = command.get(3).asFloat64();
        loc.y = command.get(4).asFloat64();
        loc.theta = command.get(5).asFloat64();
        bool ret = iNav_target->gotoTargetByAbsoluteLocation(loc);
        if (ret)
        {
            clear_current_goal_name();
            reply.addVocab32(VOCAB_OK);
        }
        else
        {
            yCError(NAVIGATION2D_NWS_YARP) << "gotoTargetByAbsoluteLocation() failed";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_GOTOABS_AND_NAME)
    {
        Map2DLocation loc;
        loc.map_id = command.get(2).asString();
        loc.x = command.get(3).asFloat64();
        loc.y = command.get(4).asFloat64();
        loc.theta = command.get(5).asFloat64();
        std::string location_name = command.get(6).asString();
        bool ret = iNav_target->gotoTargetByAbsoluteLocation(loc);
        if (ret)
        {
            set_current_goal_name(location_name);
            reply.addVocab32(VOCAB_OK);
        }
        else
        {
            yCError(NAVIGATION2D_NWS_YARP) << "gotoTargetByAbsoluteLocation() failed";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_RECOMPUTE_PATH)
    {
        bool ret = iNav_ctrl->recomputeCurrentNavigationPath();
        if (ret)
        {
            reply.addVocab32(VOCAB_OK);
        }
        else
        {
            yCError(NAVIGATION2D_NWS_YARP) << "recomputeCurrentNavigationPath() failed";
            reply.addVocab32(VOCAB_ERR);
        }
        reply.addVocab32(VOCAB_OK);
    }
    else if (request == VOCAB_NAV_GOTOREL)
    {
        if (command.size() == 5)
        {
            double x = command.get(2).asFloat64();
            double y = command.get(3).asFloat64();
            double theta = command.get(4).asFloat64();
            bool ret = iNav_target->gotoTargetByRelativeLocation(x, y, theta);
            if (ret)
            {
                clear_current_goal_name();
                reply.addVocab32(VOCAB_OK);
            }
            else
            {
                yCError(NAVIGATION2D_NWS_YARP) << "gotoTargetByRelativeLocation() failed";
                reply.addVocab32(VOCAB_ERR);
            }
        }
        else if (command.size() == 4)
        {
            double x = command.get(2).asFloat64();
            double y = command.get(3).asFloat64();
            bool ret = iNav_target->gotoTargetByRelativeLocation(x, y);
            if (ret)
            {
                clear_current_goal_name();
                reply.addVocab32(VOCAB_OK);
            }
            else
            {
                yCError(NAVIGATION2D_NWS_YARP) << "gotoTargetByRelativeLocation() failed";
                reply.addVocab32(VOCAB_ERR);
            }
        }
        else
        {
            yCError(NAVIGATION2D_NWS_YARP) << "Invalid number of params";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_GET_NAME_TARGET)
    {
        reply.addVocab32(VOCAB_OK);
        reply.addString(m_current_goal_name);
    }
    else if (request == VOCAB_NAV_GET_NAVIGATION_STATUS)
    {
        yarp::dev::Nav2D::NavigationStatusEnum nav_status = yarp::dev::Nav2D::navigation_status_error;
        bool ret = iNav_ctrl->getNavigationStatus(nav_status);
        if (ret)
        {
            reply.addVocab32(VOCAB_OK);
            reply.addInt32(nav_status);
        }
        else
        {
            yCError(NAVIGATION2D_NWS_YARP) << "getNavigationStatus() failed";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_STOP)
    {
        bool ret = iNav_ctrl->stopNavigation();
        if (ret)
        {
            clear_current_goal_name();
            reply.addVocab32(VOCAB_OK);
        }
        else
        {
            yCError(NAVIGATION2D_NWS_YARP) << "stopNavigation() failed";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_SUSPEND)
    {
        double time = -1;
        if (command.size() > 1)
        {
            time = command.get(1).asFloat64();
            bool ret = iNav_ctrl->suspendNavigation(time);
            if (ret)
            {
                reply.addVocab32(VOCAB_OK);
            }
            else
            {
                yCError(NAVIGATION2D_NWS_YARP) << "suspendNavigation() failed";
                reply.addVocab32(VOCAB_ERR);
            }
        }
        else
        {
            bool ret = iNav_ctrl->suspendNavigation();
            if (ret)
            {
                reply.addVocab32(VOCAB_OK);
            }
            else
            {
                yCError(NAVIGATION2D_NWS_YARP) << "suspendNavigation() failed";
                reply.addVocab32(VOCAB_ERR);
            }
        }
    }
    else if (request == VOCAB_NAV_RESUME)
    {
        bool ret = iNav_ctrl->resumeNavigation();
        if (ret)
        {
            reply.addVocab32(VOCAB_OK);
        }
        else
        {
            yCError(NAVIGATION2D_NWS_YARP) << "resumeNavigation failed()";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_GET_NAVIGATION_WAYPOINTS)
    {
        Map2DPath locs;
        bool ret = iNav_ctrl->getAllNavigationWaypoints((yarp::dev::Nav2D::TrajectoryTypeEnum)(command.get(2).asInt32()), locs);
        if (ret)
        {
            reply.addVocab32(VOCAB_OK);
            Bottle& waypoints = reply.addList();
            for (auto it = locs.begin(); it!=locs.end(); it++)
            {
                Bottle& the_waypoint = waypoints.addList();
                the_waypoint.addString(it->map_id);
                the_waypoint.addFloat64(it->x);
                the_waypoint.addFloat64(it->y);
                the_waypoint.addFloat64(it->theta);
            }
        }
        else
        {
            yCError(NAVIGATION2D_NWS_YARP) << "getAllNavigationWaypoints() failed";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_GET_CURRENT_WAYPOINT)
    {
        Map2DLocation loc;
        bool ret = iNav_ctrl->getCurrentNavigationWaypoint(loc);
        if (ret)
        {
            reply.addVocab32(VOCAB_OK);
            reply.addString(loc.map_id);
            reply.addFloat64(loc.x);
            reply.addFloat64(loc.y);
            reply.addFloat64(loc.theta);
        }
        else
        {
            yCError(NAVIGATION2D_NWS_YARP) << "getCurrentNavigationWaypoint() failed";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_GET_NAV_MAP)
    {
        MapGrid2D map;
        if (iNav_ctrl->getCurrentNavigationMap((yarp::dev::Nav2D::NavigationMapTypeEnum)(command.get(2).asInt32()), map))
        {
            reply.addVocab32(VOCAB_OK);
            yarp::os::Bottle& mapbot = reply.addList();
            Property::copyPortable(map, mapbot);
        }
        else
        {
            yCError(NAVIGATION2D_NWS_YARP) << "getCurrentNavigationMap() failed";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_GET_ABS_TARGET)
    {
        Map2DLocation loc;
        bool ret;
        ret = iNav_target->getAbsoluteLocationOfCurrentTarget(loc);
        if (ret)
        {
            reply.addVocab32(VOCAB_OK);
            reply.addString(loc.map_id);
            reply.addFloat64(loc.x);
            reply.addFloat64(loc.y);
            reply.addFloat64(loc.theta);
        }
        else
        {
            yCError(NAVIGATION2D_NWS_YARP) << "getAbsoluteLocationOfCurrentTarget() failed";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_GET_REL_TARGET)
    {
        Map2DLocation loc;
        bool ret;
        ret = iNav_target->getRelativeLocationOfCurrentTarget(loc.x, loc.y, loc.theta);
        if (ret)
        {
            reply.addVocab32(VOCAB_OK);
            reply.addFloat64(loc.x);
            reply.addFloat64(loc.y);
            reply.addFloat64(loc.theta);
        }
        else
        {
            yCError(NAVIGATION2D_NWS_YARP) << "getRelativeLocationOfCurrentTarget() failed";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else
    {
        yCError(NAVIGATION2D_NWS_YARP) << "Invalid vocab received:" << yarp::os::Vocab32::decode(request);
        reply.addVocab32(VOCAB_ERR);
    }

    return true;
}

bool navigation2D_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
#if DEF_m_RPC
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
#else
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) {
        return false;
    }
    reply.clear();

    //^^^^^^^^^^^^^^^^^ VOCAB SECTION
    if (command.get(0).isVocab32())
    {
        parse_respond_vocab(command, reply);
    }
    else
    {
        yCError(NAVIGATION2D_NWS_YARP) << "Invalid command type";
        reply.addVocab32(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }

    return true;
#endif
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

bool navigation2D_nws_yarp::set_current_goal_name(const std::string& name)
{
    m_current_goal_name = name;
    return true;
}

bool navigation2D_nws_yarp::get_current_goal_name(std::string& name)
{
    if (m_current_goal_name == "")
    {
        return false;
    }
    name = m_current_goal_name;
    return true;
}

bool navigation2D_nws_yarp::clear_current_goal_name()
{
    m_current_goal_name = "";
    return true;
}
