/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Navigation2DClient.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <cmath>

/*! \file Navigation2DClient.cpp */

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(NAVIGATION2DCLIENT, "yarp.device.navigation2DClient")
}

//------------------------------------------------------------------------------------------------------------------------------

bool Navigation2DClient::getNavigationStatus(NavigationStatusEnum& status)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_NAVIGATION_STATUS);
    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "getNavigationStatus() received error from navigation server";
            return false;
        }
        else
        {
            status = (NavigationStatusEnum) resp.get(1).asInt32();
            return true;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "getNavigationStatus() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Navigation2DClient::recomputeCurrentNavigationPath()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_RECOMPUTE_PATH);

    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "recomputeCurrentNavigationPath() received error from navigation server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "recomputeCurrentNavigationPath() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::suspendNavigation(const double time_s)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_SUSPEND);
    b.addFloat64(time_s);

    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "suspendNavigation() received error from navigation server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "suspendNavigation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::stopNavigation()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_STOP);

    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "stopNavigation() received error from navigation server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "stopNavigation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::resumeNavigation()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_RESUME);

    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "resumeNavigation() received error from navigation server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "resumeNavigation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Navigation2DClient::getAllNavigationWaypoints(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type, yarp::dev::Nav2D::Map2DPath& waypoints)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_NAVIGATION_WAYPOINTS);
    b.addVocab32(trajectory_type);

    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "getNavigationWaypoints() received error from navigation server";
            return false;
        }
        else if (resp.get(1).isList() && resp.get(1).asList()->size()>0)
        {
            waypoints.clear();
            Bottle* waypoints_bottle = resp.get(1).asList();
            if (waypoints_bottle == 0) { yCError(NAVIGATION2DCLIENT) << "getNavigationWaypoints parsing error"; return false; }
            for (size_t i = 0; i < waypoints_bottle->size(); i++)
            {
                Bottle* the_waypoint = waypoints_bottle->get(i).asList();
                if (the_waypoint == 0) { yCError(NAVIGATION2DCLIENT) << "getNavigationWaypoints parsing error"; return false; }
                Map2DLocation loc;
                loc.map_id = the_waypoint->get(0).asString();
                loc.x = the_waypoint->get(1).asFloat64();
                loc.y = the_waypoint->get(2).asFloat64();
                loc.theta = the_waypoint->get(3).asFloat64();
                waypoints.push_back(loc);
            }
            return true;
        }
        else
        {
            //not available
            waypoints.clear();
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "getCurrentPosition() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Navigation2DClient::getCurrentNavigationWaypoint(Map2DLocation& curr_waypoint)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_CURRENT_WAYPOINT);

    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "getCurrentNavigationWaypoint() received error from navigation server";
            return false;
        }
        else if (resp.size() == 5)
        {
            curr_waypoint.map_id = resp.get(1).asString();
            curr_waypoint.x      = resp.get(2).asFloat64();
            curr_waypoint.y      = resp.get(3).asFloat64();
            curr_waypoint.theta  = resp.get(4).asFloat64();
            return true;
        }
        else
        {
            //not available
            curr_waypoint.map_id = "invalid";
            curr_waypoint.x = std::nan("");
            curr_waypoint.y = std::nan("");
            curr_waypoint.theta = std::nan("");
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "getCurrentPosition() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::getCurrentNavigationMap(yarp::dev::Nav2D::NavigationMapTypeEnum map_type,MapGrid2D& map)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_NAV_MAP);
    b.addVocab32(map_type);

    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "getCurrentNavigationMap() received error from server";
            return false;
        }
        else
        {
            Value& b = resp.get(1);
            if (Property::copyPortable(b, map))
            {
                return true;
            }
            else
            {
                yCError(NAVIGATION2DCLIENT) << "getCurrentNavigationMap() failed copyPortable()";
                return false;
            }
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "getCurrentNavigationMap() error on writing on rpc port";
        return false;
    }
    return true;
}
