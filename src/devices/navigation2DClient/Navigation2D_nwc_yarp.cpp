/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Navigation2D_nwc_yarp.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <cmath>

/*! \file Navigation2D_nwc_yarp.cpp */

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(NAVIGATION2D_NWC_YARP, "yarp.device.navigation2D_nwc_yarp")
}

//------------------------------------------------------------------------------------------------------------------------------

bool Navigation2D_nwc_yarp::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_navigation_server_name.clear();
    m_map_locations_server_name.clear();
    m_localization_server_name.clear();

    m_local_name = config.find("local").asString();
    m_navigation_server_name = config.find("navigation_server").asString();
    m_map_locations_server_name = config.find("map_locations_server").asString();
    m_localization_server_name = config.find("localization_server").asString();

    if (m_local_name == "")
    {
        yCError(NAVIGATION2D_NWC_YARP, "open() error you have to provide a valid 'local' param");
        return false;
    }

    if (m_navigation_server_name == "")
    {
        yCError(NAVIGATION2D_NWC_YARP, "open() error you have to provide a valid 'navigation_server' param");
        return false;
    }

    if (m_map_locations_server_name == "")
    {
        yCError(NAVIGATION2D_NWC_YARP, "open() error you have to provide valid 'map_locations_server' param");
        return false;
    }

    if (m_localization_server_name == "")
    {
        yCError(NAVIGATION2D_NWC_YARP, "open() error you have to provide valid 'localization_server' param");
        return false;
    }

    if (config.check("period"))
    {
        m_period = config.find("period").asInt32();
    }
    else
    {
        m_period = 10;
        yCWarning(NAVIGATION2D_NWC_YARP, "Using default period of %d ms" , m_period);
    }

    std::string
            local_rpc_1,
            local_rpc_2,
            local_rpc_3,
            remote_rpc_1,
            remote_rpc_2,
            remote_rpc_3,
            remote_streaming_name,
            local_streaming_name;

    local_rpc_1           = m_local_name           + "/navigation/rpc";
    local_rpc_2           = m_local_name           + "/locations/rpc";
    local_rpc_3           = m_local_name           + "/localization/rpc";
    remote_rpc_1          = m_navigation_server_name + "/rpc";
    remote_rpc_2          = m_map_locations_server_name + "/rpc";
    remote_rpc_3          = m_localization_server_name + "/rpc";
    remote_streaming_name = m_localization_server_name + "/stream:o";
    local_streaming_name  = m_local_name           + "/stream:i";

    if (!m_rpc_port_to_navigation_server.open(local_rpc_1))
    {
        yCError(NAVIGATION2D_NWC_YARP, "open() error could not open rpc port %s, check network", local_rpc_1.c_str());
        return false;
    }

    if (!m_rpc_port_to_Map2DServer.open(local_rpc_2))
    {
        yCError(NAVIGATION2D_NWC_YARP, "open() error could not open rpc port %s, check network", local_rpc_2.c_str());
        return false;
    }

    if (!m_rpc_port_to_localization_server.open(local_rpc_3))
    {
        yCError(NAVIGATION2D_NWC_YARP, "open() error could not open rpc port %s, check network", local_rpc_3.c_str());
        return false;
    }

    bool ok = true;

    ok = Network::connect(local_rpc_1, remote_rpc_1);
    if (!ok)
    {
        yCError(NAVIGATION2D_NWC_YARP, "open() error could not connect to %s", remote_rpc_1.c_str());
        return false;
    }

    ok = Network::connect(local_rpc_2, remote_rpc_2);
    if (!ok)
    {
        yCError(NAVIGATION2D_NWC_YARP, "open() error could not connect to %s", remote_rpc_2.c_str());
        return false;
    }

    ok = Network::connect(local_rpc_3, remote_rpc_3);
    if (!ok)
    {
        yCError(NAVIGATION2D_NWC_YARP, "open() error could not connect to %s", remote_rpc_3.c_str());
        return false;
    }

    if (!m_map_RPC.yarp().attachAsClient(m_rpc_port_to_Map2DServer))
    {
        yCError(NAVIGATION2D_NWC_YARP, "Error! Cannot attach the m_rpc_port_to_Map2DServer port as a client");
        return false;
    }

    if (!m_loc_RPC.yarp().attachAsClient(m_rpc_port_to_localization_server))
    {
        yCError(NAVIGATION2D_NWC_YARP, "Error! Cannot attach the m_rpc_port_localization_server port as a client");
        return false;
    }

    if (!m_nav_RPC.yarp().attachAsClient(m_rpc_port_to_navigation_server))
    {
        yCError(NAVIGATION2D_NWC_YARP, "Error! Cannot attach the m_rpc_port_navigation_server port as a client");
        return false;
    }

    return true;
}

bool Navigation2D_nwc_yarp::close()
{
    m_rpc_port_to_navigation_server.close();
    m_rpc_port_to_Map2DServer.close();
    m_rpc_port_to_localization_server.close();
    return true;
}

bool Navigation2D_nwc_yarp::read(yarp::os::ConnectionReader& connection)
{
    yCError(NAVIGATION2D_NWC_YARP, "Should not enter here");
    return true;
}

bool Navigation2D_nwc_yarp::checkNearToLocation(Map2DLocation loc, double linear_tolerance, double angular_tolerance)
{
    Map2DLocation curr_loc;
    if (getCurrentPosition(curr_loc) == false)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "checkInsideArea() unable to get robot position";
        return false;
    }

    return curr_loc.is_near_to(loc, linear_tolerance, angular_tolerance);
}

bool Navigation2D_nwc_yarp::checkNearToLocation(std::string location_name, double linear_tolerance, double angular_tolerance)
{
    Map2DLocation loc;
    Map2DLocation curr_loc;
    if (this->getLocation(location_name, loc) == false)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "Location" << location_name << "not found";
        return false;
    }

    if (getCurrentPosition(curr_loc) == false)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "checkInsideArea() unable to get robot position";
        return false;
    }

    return curr_loc.is_near_to(loc, linear_tolerance, angular_tolerance);
}

bool  Navigation2D_nwc_yarp::checkInsideArea(Map2DArea area)
{
    Map2DLocation curr_loc;
    if (getCurrentPosition(curr_loc) == false)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "checkInsideArea() unable to get robot position";
        return false;
    }

    if (area.checkLocationInsideArea(curr_loc) == false)
    {
        //yCDebug(NAVIGATION2D_NWC) << "Not inside Area";
        return false;
    }

    return true;
}

bool Navigation2D_nwc_yarp::checkInsideArea(std::string area_name)
{
    Map2DLocation curr_loc;
    Map2DArea area;
    if (this->getArea(area_name, area) == false)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "Area" << area_name << "not found";
        return false;
    }

    if (getCurrentPosition(curr_loc) == false)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "checkInsideArea() unable to get robot position";
        return false;
    }

    if (area.checkLocationInsideArea(curr_loc) == false)
    {
        //yCDebug(NAVIGATION2D_NWC) << "Not inside Area";
        return false;
    }

    return true;
}

bool Navigation2D_nwc_yarp::gotoTargetByLocationName(std::string location_name)
{
    Map2DLocation loc;
    Map2DArea area;

    //first of all, ask to the location server if location_name exists as a location_name...
    bool found = this->getLocation(location_name, loc);

    //...if found, ok...otherwise check if location_name is an area name instead...
    if (found == false)
    {
        found = this->getArea(location_name, area);
        if (found)
        {
            area.getRandomLocation(loc);
        }
    }

    //...if it is neither a location, nor an area then quit...
    if (found == false)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "Location not found";
        return false;
    }

    //...otherwise we can go to the found/computed location!
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_nav_RPC.goto_target_by_absolute_location_and_set_name_RPC(loc, location_name);
}


bool Navigation2D_nwc_yarp::getNameOfCurrentTarget(std::string& location_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_nav_RPC.get_name_of_current_target_RPC();
    if (!ret.ret)
    {
        yCError(NAVIGATION2D_NWC_YARP, "Unable to get_name_of_current_target_RPC");
        return false;
    }
    location_name = ret.name;
    return true;
}

bool Navigation2D_nwc_yarp::storeCurrentPosition(std::string location_name)
{
    Map2DLocation loc;
    bool b = this->getCurrentPosition(loc);
    if (!b) {return false;}
    this->storeLocation(location_name,loc);
    if (!b) {return false;}
    return true;
}
