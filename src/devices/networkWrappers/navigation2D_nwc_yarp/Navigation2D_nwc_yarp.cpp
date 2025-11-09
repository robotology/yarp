/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Navigation2D_nwc_yarp.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include "yarp/dev/Map2DPath.h"
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
    if (!parseParams(config)) { return false; }

    std::string local_name = m_local;
    std::string navigation_server_name = m_navigation_server;
    std::string map_locations_server_name = m_map_locations_server;
    std::string localization_server_name = m_localization_server;

    std::string
            local_rpc_1,
            local_rpc_2,
            local_rpc_3,
            remote_rpc_1,
            remote_rpc_2,
            remote_rpc_3,
            remote_streaming_name,
            local_streaming_name,
            rpc_port_user_commandsName;

    local_rpc_1           = local_name           + "/navigation/rpc";
    local_rpc_2           = local_name           + "/locations/rpc";
    local_rpc_3           = local_name           + "/localization/rpc";
    remote_rpc_1          = navigation_server_name + "/rpc";
    remote_rpc_2          = map_locations_server_name + "/rpc";
    remote_rpc_3          = localization_server_name + "/rpc";
    remote_streaming_name = localization_server_name + "/stream:o";
    local_streaming_name  = local_name           + "/stream:i";
    rpc_port_user_commandsName = local_name      + "/user_commands/rpc";

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

    if (!m_rpc_port_user_commands.open(rpc_port_user_commandsName))
    {
        yCError(NAVIGATION2D_NWC_YARP, "Failed to open port %s", rpc_port_user_commandsName.c_str());
        return false;
    }

    m_rpc_port_user_commands.setReader(*this);
    return true;
}

bool Navigation2D_nwc_yarp::close()
{
    m_rpc_port_to_navigation_server.close();
    m_rpc_port_to_Map2DServer.close();
    m_rpc_port_to_localization_server.close();
    m_rpc_port_user_commands.close();
    return true;
}

bool Navigation2D_nwc_yarp::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) {
        return false;
    }
    reply.clear();

    if (command.get(0).asString() == "gotoLocation")
    {
        std::string loc= command.get(1).asString();
        this->gotoTargetByLocationName(loc);
        reply.addVocab32(VOCAB_OK);
    }
    else if (command.get(0).asString() == "listLocations")
    {
        std::vector<std::string> locations;
        this->getLocationsList(locations);
        reply.addVocab32("many");
        for (auto it=locations.begin(); it!=locations.end(); it++)
        {
            std::string loc = *it;
            reply.addString(loc);
        }
    }
    else if (command.get(0).asString() == "help")
    {
        reply.addVocab32("many");
        reply.addString("gotoLocation <locationName>");
        reply.addString("listLocations");
    }
    else
    {
        yCError(NAVIGATION2D_NWC_YARP) << "Invalid command";
        reply.addVocab32(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }
    return true;
}

ReturnValue Navigation2D_nwc_yarp::checkNearToLocation(Map2DLocation loc, bool& is_near, double linear_tolerance, double angular_tolerance)
{
    Map2DLocation curr_loc;
    auto ret = getCurrentPosition(curr_loc);
    if (!ret)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "checkInsideArea() unable to get robot position";
        return ret;
    }

    is_near = curr_loc.is_near_to(loc, linear_tolerance, angular_tolerance);

    return ReturnValue_ok;
}

ReturnValue Navigation2D_nwc_yarp::checkNearToLocation(std::string location_name, bool& is_near, double linear_tolerance, double angular_tolerance)
{
    Map2DLocation loc;
    Map2DLocation curr_loc;
    auto ret1 = getLocation(location_name, loc);
    if (!ret1)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "Location" << location_name << "not found";
        return ret1;
    }

    auto ret2 = getCurrentPosition(curr_loc);
    if (!ret2)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "checkInsideArea() unable to get robot position";
        return ret2;
    }

    is_near = curr_loc.is_near_to(loc, linear_tolerance, angular_tolerance);

    return ReturnValue_ok;
}

ReturnValue  Navigation2D_nwc_yarp::checkInsideArea(Map2DArea area, bool& is_inside)
{
    Map2DLocation curr_loc;
    auto ret1 = getCurrentPosition(curr_loc);
    if (!ret1)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "checkInsideArea() unable to get robot position";
        return ret1;
    }

    is_inside = area.checkLocationInsideArea(curr_loc);

    return ReturnValue_ok;
}

ReturnValue Navigation2D_nwc_yarp::inWhichAreaIAm(std::string& area_name, Nav2D::Map2DArea& area)
{
    area_name = "";
    area = Map2DArea();

    Map2DLocation curr_loc;
    auto ret1 = getCurrentPosition(curr_loc);
    if (!ret1)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "checkInsideArea() unable to get robot position";
        return ret1;
    }

    std::vector<std::string> areaslist;
    this->getAreasList(areaslist);

    bool is_inside=false;
    for (auto it = areaslist.begin(); it != areaslist.end(); it++)
    {
        Map2DArea areatemp;
        auto ret2 = getArea(*it, areatemp);
        if (!ret2)
        {
            yCError(NAVIGATION2D_NWC_YARP) << "Area" << *it << "not found";
            return ret2;
        }

        is_inside = areatemp.checkLocationInsideArea(curr_loc);
        if (is_inside)
        {
            area_name = *it;
            area = areatemp;
            return ReturnValue_ok;
        }
    }

    return ReturnValue_ok;
}

ReturnValue Navigation2D_nwc_yarp::checkInsideArea(std::string area_name, bool& is_inside)
{
    Map2DLocation curr_loc;
    Map2DArea area;
    auto ret1 = getArea(area_name, area);
    if (!ret1)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "Area" << area_name << "not found";
        return ret1;
    }

    auto ret2 = getCurrentPosition(curr_loc);
    if (!ret2)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "checkInsideArea() unable to get robot position";
        return ret2;
    }

    is_inside = area.checkLocationInsideArea(curr_loc);

    return ReturnValue_ok;
}

ReturnValue Navigation2D_nwc_yarp::gotoTargetByLocationName(std::string location_name)
{
    Map2DLocation loc;
    Map2DArea area;
    Map2DPath path;

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

    //...if it is neither a location, nor an area then check if it is a path...
    if (found == false)
    {
        if(this->getPath(location_name, path))
        {
            // We still need to handle path navigation differently
            std::lock_guard <std::mutex> lg(m_mutex);
            return m_nav_RPC.follow_path_RPC(path);
        }
    }

    //...if it is neither a location, nor an area, nor a path then quit...
    if (found == false)
    {
        yCError(NAVIGATION2D_NWC_YARP) << "Location not found, stopping navigation";
        m_nav_RPC.stop_navigation_RPC();
        return ReturnValue::return_code::return_value_error_method_failed;
    }

    //...otherwise we can go to the found/computed location!
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_nav_RPC.goto_target_by_absolute_location_and_set_name_RPC(loc, location_name);
}


ReturnValue Navigation2D_nwc_yarp::getNameOfCurrentTarget(std::string& location_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_nav_RPC.get_name_of_current_target_RPC();
    if (!ret.ret)
    {
        yCError(NAVIGATION2D_NWC_YARP, "Unable to get_name_of_current_target_RPC");
        return ret.ret;
    }
    location_name = ret.name;
    return ret.ret;
}

ReturnValue Navigation2D_nwc_yarp::storeCurrentPosition(std::string location_name)
{
    Map2DLocation loc;
    auto ret = this->getCurrentPosition(loc);
    if (!ret) {return ret;}
    auto ret2 = this->storeLocation(location_name,loc);
    if (!ret2) {return ret2;}
    return ReturnValue_ok;
}
