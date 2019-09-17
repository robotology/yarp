/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#define _USE_MATH_DEFINES

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
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

#ifndef RAD2DEG
#define RAD2DEG 180.0/M_PI
#endif

navigation2DServer::navigation2DServer() : PeriodicThread(DEFAULT_THREAD_PERIOD)
{
    iNav_target = nullptr;
    iNav_ctrl = nullptr;
    m_navigation_status=yarp::dev::navigation_status_idle;
}

bool navigation2DServer::attachAll(const PolyDriverList &device2attach)
{
    if (device2attach.size() != 1)
    {
        yError("Navigation2DServer: cannot attach more than one device");
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
        yError("Navigation2DServer: subdevice passed to attach method is invalid");
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
    yDebug() << "navigation2DServer configuration: \n" << config.toString().c_str();

    if (!config.check("period"))
    {
        yInfo() << "navigation2DServer: missing 'period' parameter. Using default value: 0.010";
        m_period = 0.010;
    }
    else
    {
        m_period = config.find("period").asFloat64();
    }

    string local_name = "/navigationServer";
    if (!config.check("name"))
    {
        yInfo() << "navigation2DServer: missing 'name' parameter. Using default value: /navigationServer";
    }
    else
    {
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
            yError() << "navigation2DServer: failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&pNav, "1");
        if (!attachAll(driverlist))
        {
            yError() << "navigation2DServer: failed to open subdevice.. check params";
            return false;
        }
    }
    m_stats_time_last = yarp::os::Time::now();

    if (!initialize_YARP(config))
    {
        yError() << "navigation2DServer: Error initializing YARP ports";
        return false;
    }

    return true;
}

bool navigation2DServer::initialize_YARP(yarp::os::Searchable &params)
{
    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yError("Navigation2DServer: failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);
    return true;
}

bool navigation2DServer::close()
{
    yTrace("navigation2DServer::Close");
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
        yError() << "General error in navigation2DServer::parse_respond_string()";
        return false;
    }

    if (command.get(0).asString() == "help")
    {
        reply.addVocab(Vocab::encode("many"));
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
    if (command.get(0).isVocab() == false)
    {
        yError() << "General error in navigation2DServer::parse_respond_vocab()";
        return false;
    }

    if (command.get(0).asVocab() != VOCAB_INAVIGATION ||
        command.get(1).isVocab() == false)
    {
        yError() << "Invalid vocab received";
        reply.addVocab(VOCAB_ERR);
        return true;
    }

    int request = command.get(1).asVocab();
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
            reply.addVocab(VOCAB_OK);
        }
        else
        {
            yError() << "gotoTargetByAbsoluteLocation() failed";
            reply.addVocab(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_RECOMPUTE_PATH)
    {
        bool ret = iNav_ctrl->recomputeCurrentNavigationPath();
        if (ret)
        {
            reply.addVocab(VOCAB_OK);
        }
        else
        {
            yError() << "recomputeCurrentNavigationPath() failed";
            reply.addVocab(VOCAB_ERR);
        }
        reply.addVocab(VOCAB_OK);
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
                reply.addVocab(VOCAB_OK);
            }
            else
            {
                yError() << "gotoTargetByRelativeLocation() failed";
                reply.addVocab(VOCAB_ERR);
            }
        }
        else if (command.size() == 4)
        {
            double x = command.get(2).asFloat64();
            double y = command.get(3).asFloat64();
            bool ret = iNav_target->gotoTargetByRelativeLocation(x, y);
            if (ret)
            {
                reply.addVocab(VOCAB_OK);
            }
            else
            {
                yError() << "gotoTargetByRelativeLocation() failed";
                reply.addVocab(VOCAB_ERR);
            }
        }
        else
        {
            yError() << "Invalid number of params";
            reply.addVocab(VOCAB_ERR);
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
            reply.addVocab(VOCAB_OK);
            reply.addInt32(VOCAB_OK);
        }
        else
        {
            yError() << "applyVelocityCommand() failed";
            reply.addVocab(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_GET_NAVIGATION_STATUS)
    {
        yarp::dev::NavigationStatusEnum nav_status = yarp::dev::navigation_status_error;
        bool ret = iNav_ctrl->getNavigationStatus(nav_status);
        if (ret)
        {
            reply.addVocab(VOCAB_OK);
            reply.addInt32(nav_status);
        }
        else
        {
            yError() << "getNavigationStatus() failed";
            reply.addVocab(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_STOP)
    {
        bool ret = iNav_ctrl->stopNavigation();
        if (ret)
        {
            reply.addVocab(VOCAB_OK);
        }
        else
        {
            yError() << "stopNavigation() failed";
            reply.addVocab(VOCAB_ERR);
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
                reply.addVocab(VOCAB_OK);
            }
            else
            {
                yError() << "suspendNavigation() failed";
                reply.addVocab(VOCAB_ERR);
            }
        }
        else
        {
            bool ret = iNav_ctrl->suspendNavigation();
            if (ret)
            {
                reply.addVocab(VOCAB_OK);
            }
            else
            {
                yError() << "suspendNavigation() failed";
                reply.addVocab(VOCAB_ERR);
            }
        }
    }
    else if (request == VOCAB_NAV_RESUME)
    {
        bool ret = iNav_ctrl->resumeNavigation();
        if (ret)
        {
            reply.addVocab(VOCAB_OK);
        }
        else
        {
            yError() << "resumeNavigation failed()";
            reply.addVocab(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_GET_NAVIGATION_WAYPOINTS)
    {
        std::vector<Map2DLocation> locs;
        bool ret = iNav_ctrl->getAllNavigationWaypoints(locs);
        if (ret)
        {
            reply.addVocab(VOCAB_OK);
            Bottle& waypoints = reply.addList();
            for (size_t i = 0; i < locs.size(); i++)
            {
                Bottle& the_waypoint = waypoints.addList();
                the_waypoint.addString(locs[i].map_id);
                the_waypoint.addFloat64(locs[i].x);
                the_waypoint.addFloat64(locs[i].y);
                the_waypoint.addFloat64(locs[i].theta);
            }
        }
        else
        {
            yError() << "getAllNavigationWaypoints() failed";
            reply.addVocab(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_GET_CURRENT_WAYPOINT)
    {
        Map2DLocation loc;
        bool ret = iNav_ctrl->getCurrentNavigationWaypoint(loc);
        if (ret)
        {
            reply.addVocab(VOCAB_OK);
            reply.addString(loc.map_id);
            reply.addFloat64(loc.x);
            reply.addFloat64(loc.y);
            reply.addFloat64(loc.theta);
        }
        else
        {
            yError() << "getCurrentNavigationWaypoint() failed";
            reply.addVocab(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_GET_NAV_MAP)
    {
        MapGrid2D map;
        if (iNav_ctrl->getCurrentNavigationMap((yarp::dev::NavigationMapTypeEnum)(command.get(2).asInt32()), map))
        {
            reply.addVocab(VOCAB_OK);
            yarp::os::Bottle& mapbot = reply.addList();
            Property::copyPortable(map, mapbot);
        }
        else
        {
            yError() << "getCurrentNavigationMap() failed";
            reply.addVocab(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_GET_ABS_TARGET)
    {
        Map2DLocation loc;
        bool ret;
        ret = iNav_target->getAbsoluteLocationOfCurrentTarget(loc);
        if (ret)
        {
            reply.addVocab(VOCAB_OK);
            reply.addString(loc.map_id);
            reply.addFloat64(loc.x);
            reply.addFloat64(loc.y);
            reply.addFloat64(loc.theta);
        }
        else
        {
            yError() << "getAbsoluteLocationOfCurrentTarget() failed";
            reply.addVocab(VOCAB_ERR);
        }
    }
    else if (request == VOCAB_NAV_GET_REL_TARGET)
    {
        Map2DLocation loc;
        bool ret;
        ret = iNav_target->getRelativeLocationOfCurrentTarget(loc.x, loc.y, loc.theta);
        if (ret)
        {
            reply.addVocab(VOCAB_OK);
            reply.addFloat64(loc.x);
            reply.addFloat64(loc.y);
            reply.addFloat64(loc.theta);
        }
        else
        {
            yError() << "getRelativeLocationOfCurrentTarget() failed";
            reply.addVocab(VOCAB_ERR);
        }
    }
    else
    {
        yError() << "Invalid vocab received";
        reply.addVocab(VOCAB_ERR);
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
    else if (command.get(0).isVocab())
    {
        parse_respond_vocab(command, reply);
    }
    else
    {
        yError() << "Invalid command type";
        reply.addVocab(VOCAB_ERR);
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
            yError("navigation2DServer, unable to get Navigation Status!\n");
        }
        else
        {
            yInfo() << "navigation2DServer running, ALL ok. Navigation status:" << getStatusAsString(m_navigation_status);
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
