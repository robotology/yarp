/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define _USE_MATH_DEFINES

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
#include "navigation2DServer.h"

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace std;

namespace {
YARP_LOG_COMPONENT(NAVIGATION2DSERVER, "yarp.device.navigation2DServer")
}

#ifndef RAD2DEG
#define RAD2DEG 180.0/M_PI
#endif

navigation2DServer::navigation2DServer() : PeriodicThread(DEFAULT_THREAD_PERIOD)
{
    iNav_target = nullptr;
    iNav_ctrl = nullptr;
    m_navigation_status=yarp::dev::Nav2D::navigation_status_idle;
    m_stats_time_last = yarp::os::Time::now();
}

bool navigation2DServer::attachAll(const PolyDriverList &device2attach)
{
    if (device2attach.size() != 1)
    {
        yCError(NAVIGATION2DSERVER, "Cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(iNav_target);
        Idevice2attach->view(iNav_ctrl);
    }

    if (nullptr == iNav_target ||
        nullptr == iNav_ctrl)
    {
        yCError(NAVIGATION2DSERVER, "Subdevice passed to attach method is invalid");
        return false;
    }

    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}

bool navigation2DServer::detachAll()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    iNav_target = nullptr;
    iNav_ctrl = nullptr;
    return true;
}

bool navigation2DServer::open(Searchable& config)
{
    Property params;
    params.fromString(config.toString().c_str());
    yCDebug(NAVIGATION2DSERVER) << "Configuration: \n" << config.toString().c_str();

    if (!config.check("period"))
    {
        yCInfo(NAVIGATION2DSERVER) << "Missing 'period' parameter. Using default value: 0.010";
        m_period = 0.010;
    }
    else
    {
        m_period = config.find("period").asFloat64();
    }

    string local_name = "/navigationServer";
    if (!config.check("name"))
    {
        yCInfo(NAVIGATION2DSERVER) << "Missing 'name' parameter. Using default value: /navigationServer";
    }
    else
    {
        local_name = config.find("name").asString();
    }
    m_rpcPortName = local_name + "/rpc";
    m_streamingPortName = local_name + "/streaming:o";

    if (config.check("subdevice"))
    {
        Property       p;
        PolyDriverList driverlist;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if (!pNav.open(p) || !pNav.isValid())
        {
            yCError(NAVIGATION2DSERVER) << "Failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&pNav, "1");
        if (!attachAll(driverlist))
        {
            yCError(NAVIGATION2DSERVER) << "Failed to open subdevice.. check params";
            return false;
        }
    }

    if (!initialize_YARP(config))
    {
        yCError(NAVIGATION2DSERVER) << "Error initializing YARP ports";
        return false;
    }

    return true;
}

bool navigation2DServer::initialize_YARP(yarp::os::Searchable &params)
{
    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yCError(NAVIGATION2DSERVER, "Failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);
    return true;
}

bool navigation2DServer::close()
{
    yCTrace(NAVIGATION2DSERVER, "Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    detachAll();
    return true;
}

bool navigation2DServer::parse_respond_string(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
    if (command.get(0).isString() == false)
    {
        yCError(NAVIGATION2DSERVER) << "General error in navigation2DServer::parse_respond_string()";
        return false;
    }

    if (command.get(0).asString() == "help")
    {
        reply.addVocab32("many");
        reply.addString("Navigation2DServer does not support rpc commands in plain text format, only vocabs.");
        reply.addString("Please use the rpc port of Navigation2DClient.");
    }
    else
    {
        reply.addString("Unknown command. Type 'help'.");
    }
    return true;
}

bool navigation2DServer::parse_respond_vocab(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
    if (command.get(0).isVocab32() == false)
    {
        yCError(NAVIGATION2DSERVER) << "General error in navigation2DServer::parse_respond_vocab()";
        return false;
    }

    if (command.get(0).asVocab32() != VOCAB_INAVIGATION ||
        command.get(1).isVocab32() == false)
    {
        yCError(NAVIGATION2DSERVER) << "Invalid vocab received";
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
            yCError(NAVIGATION2DSERVER) << "gotoTargetByAbsoluteLocation() failed";
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
        string location_name = command.get(6).asString();
        bool ret = iNav_target->gotoTargetByAbsoluteLocation(loc);
        if (ret)
        {
            set_current_goal_name(location_name);
            reply.addVocab32(VOCAB_OK);
        }
        else
        {
            yCError(NAVIGATION2DSERVER) << "gotoTargetByAbsoluteLocation() failed";
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
            yCError(NAVIGATION2DSERVER) << "recomputeCurrentNavigationPath() failed";
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
                yCError(NAVIGATION2DSERVER) << "gotoTargetByRelativeLocation() failed";
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
                yCError(NAVIGATION2DSERVER) << "gotoTargetByRelativeLocation() failed";
                reply.addVocab32(VOCAB_ERR);
            }
        }
        else
        {
            yCError(NAVIGATION2DSERVER) << "Invalid number of params";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_VELOCITY_CMD)
    {
        double x_vel   = command.get(2).asFloat64();
        double y_vel   = command.get(3).asFloat64();
        double t_vel   = command.get(4).asFloat64();
        double timeout = command.get(5).asFloat64();
        bool ret = iNav_target->applyVelocityCommand(x_vel,y_vel,t_vel,timeout);
        if (ret)
        {
            clear_current_goal_name();
            reply.addVocab32(VOCAB_OK);
        }
        else
        {
            yCError(NAVIGATION2DSERVER) << "applyVelocityCommand() failed";
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
            yCError(NAVIGATION2DSERVER) << "getNavigationStatus() failed";
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
            yCError(NAVIGATION2DSERVER) << "stopNavigation() failed";
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
                yCError(NAVIGATION2DSERVER) << "suspendNavigation() failed";
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
                yCError(NAVIGATION2DSERVER) << "suspendNavigation() failed";
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
            yCError(NAVIGATION2DSERVER) << "resumeNavigation failed()";
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
            yCError(NAVIGATION2DSERVER) << "getAllNavigationWaypoints() failed";
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
            yCError(NAVIGATION2DSERVER) << "getCurrentNavigationWaypoint() failed";
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
            yCError(NAVIGATION2DSERVER) << "getCurrentNavigationMap() failed";
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
            yCError(NAVIGATION2DSERVER) << "getAbsoluteLocationOfCurrentTarget() failed";
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
            yCError(NAVIGATION2DSERVER) << "getRelativeLocationOfCurrentTarget() failed";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else
    {
        yCError(NAVIGATION2DSERVER) << "Invalid vocab received:" << yarp::os::Vocab32::decode(request);
        reply.addVocab32(VOCAB_ERR);
    }

    return true;
}

bool navigation2DServer::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) return false;
    reply.clear();

    //^^^^^^^^^^^^^^^^^ STRING SECTION
    if (command.get(0).isString())
    {
        parse_respond_string(command, reply);
    }
    //^^^^^^^^^^^^^^^^^ VOCAB SECTION
    else if (command.get(0).isVocab32())
    {
        parse_respond_vocab(command, reply);
    }
    else
    {
        yCError(NAVIGATION2DSERVER) << "Invalid command type";
        reply.addVocab32(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }

    return true;
}

void navigation2DServer::run()
{
    bool ok = iNav_ctrl->getNavigationStatus(m_navigation_status);

    double m_stats_time_curr = yarp::os::Time::now();
    if (m_stats_time_curr - m_stats_time_last > 5.0)
    {
        if (!ok)
        {
            yCError(NAVIGATION2DSERVER, "Unable to get Navigation Status!\n");
        }
        else
        {
            yCInfo(NAVIGATION2DSERVER) << "Running, ALL ok. Navigation status:" << getStatusAsString(m_navigation_status);
        }
        m_stats_time_last = yarp::os::Time::now();
    }
}

std::string navigation2DServer::getStatusAsString(NavigationStatusEnum status)
{
    if (status == navigation_status_idle) return std::string("navigation_status_idle");
    else if (status == navigation_status_moving) return std::string("navigation_status_moving");
    else if (status == navigation_status_waiting_obstacle) return std::string("navigation_status_waiting_obstacle");
    else if (status == navigation_status_goal_reached) return std::string("navigation_status_goal_reached");
    else if (status == navigation_status_aborted) return std::string("navigation_status_aborted");
    else if (status == navigation_status_failing) return std::string("navigation_status_failing");
    else if (status == navigation_status_paused) return std::string("navigation_status_paused");
    else if (status == navigation_status_preparing_before_move) return std::string("navigation_status_preparing_before_move");
    else if (status == navigation_status_thinking) return std::string("navigation_status_thinking");
    else if (status == navigation_status_error) return std::string("navigation_status_error");
    return std::string("navigation_status_error");
}

bool navigation2DServer::set_current_goal_name(const std::string& name)
{
    m_current_goal_name = name;
    return true;
}

bool navigation2DServer::get_current_goal_name(std::string& name)
{
    if (m_current_goal_name == "")
    {
        return false;
    }
    name = m_current_goal_name;
    return true;
}

bool navigation2DServer::clear_current_goal_name()
{
    m_current_goal_name = "";
    return true;
}
