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

#include "Navigation2DClient.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <cmath>

/*! \file Navigation2DClient.cpp */

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace yarp::sig;


bool Navigation2DClient::set_current_goal_name(const std::string& name)
{
    m_current_goal_name = name;
    return true;
}

bool Navigation2DClient::get_current_goal_name(std::string& name)
{
    if (m_current_goal_name == "")
    {
        return false;
    }
    name = m_current_goal_name;
    return true;
}

bool Navigation2DClient::reset_current_goal_name()
{
    m_current_goal_name = "";
    return true;
}

//------------------------------------------------------------------------------------------------------------------------------

bool Navigation2DClient::open(yarp::os::Searchable &config)
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
        yError("Navigation2DClient::open() error you have to provide a valid 'local' param");
        return false;
    }

    if (m_navigation_server_name == "")
    {
        yError("Navigation2DClient::open() error you have to provide a valid 'navigation_server' param");
        return false;
    }

    if (m_map_locations_server_name == "")
    {
        yError("Navigation2DClient::open() error you have to provide valid 'map_locations_server' param");
        return false;
    }

    if (m_localization_server_name == "")
    {
        yError("Navigation2DClient::open() error you have to provide valid 'localization_server' param");
        return false;
    }

    if (config.check("period"))
    {
        m_period = config.find("period").asInt32();
    }
    else
    {
        m_period = 10;
        yWarning("Navigation2DClient: using default period of %d ms" , m_period);
    }

    std::string
            local_rpc_1,
            local_rpc_2,
            local_rpc_3,
            local_rpc_4,
            remote_rpc_1,
            remote_rpc_2,
            remote_rpc_3,
            remote_streaming_name,
            local_streaming_name;

    local_rpc_1           = m_local_name           + "/navigation/rpc";
    local_rpc_2           = m_local_name           + "/locations/rpc";
    local_rpc_3           = m_local_name           + "/localization/rpc";
    local_rpc_4           = m_local_name           + "/user_commands/rpc";
    remote_rpc_1          = m_navigation_server_name + "/rpc";
    remote_rpc_2          = m_map_locations_server_name + "/rpc";
    remote_rpc_3          = m_localization_server_name + "/rpc";
    remote_streaming_name = m_localization_server_name + "/stream:o";
    local_streaming_name  = m_local_name           + "/stream:i";

    if (!m_rpc_port_navigation_server.open(local_rpc_1))
    {
        yError("Navigation2DClient::open() error could not open rpc port %s, check network", local_rpc_1.c_str());
        return false;
    }

    if (!m_rpc_port_map_locations_server.open(local_rpc_2))
    {
        yError("Navigation2DClient::open() error could not open rpc port %s, check network", local_rpc_2.c_str());
        return false;
    }

    if (!m_rpc_port_localization_server.open(local_rpc_3))
    {
        yError("Navigation2DClient::open() error could not open rpc port %s, check network", local_rpc_3.c_str());
        return false;
    }

    bool ok = true;

    ok = Network::connect(local_rpc_1, remote_rpc_1);
    if (!ok)
    {
        yError("Navigation2DClient::open() error could not connect to %s", remote_rpc_1.c_str());
        return false;
    }

    ok = Network::connect(local_rpc_2, remote_rpc_2);
    if (!ok)
    {
        yError("Navigation2DClient::open() error could not connect to %s", remote_rpc_2.c_str());
        return false;
    }

    ok = Network::connect(local_rpc_3, remote_rpc_3);
    if (!ok)
    {
        yError("Navigation2DClient::open() error could not connect to %s", remote_rpc_3.c_str());
        return false;
    }

    if (!m_rpc_port_user_commands.open(local_rpc_4.c_str()))
    {
        yError("Navigation2DServer: failed to open port %s", local_rpc_4.c_str());
        return false;
    }
    m_rpc_port_user_commands.setReader(*this);

    return true;
}

bool Navigation2DClient::close()
{
    m_rpc_port_navigation_server.close();
    m_rpc_port_map_locations_server.close();
    m_rpc_port_localization_server.close();
    m_rpc_port_user_commands.close();
    return true;
}

bool Navigation2DClient::parse_respond_string(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
    if (command.get(0).isString() == false)
    {
        yError() << "General error in Navigation2DClient::parse_respond_string";
        return false;
    }

    if (command.get(0).asString() == "help")
    {
        reply.addVocab(Vocab::encode("many"));
        reply.addString("Available commands are:");
        reply.addString("goto <locationName>");
        //reply.addString("gotoAbs <x> <y> <angle in degrees>");
        //reply.addString("gotoRel <x> <y> <angle in degrees>");
        reply.addString("store_location <location_name> <map_id> <x> <y> <y>");
        reply.addString("store_current_location <location_name>");
        reply.addString("delete_location <location_name>");
        reply.addString("clear_all_locations");
        reply.addString("get_last_target");
        reply.addString("get_location_list");
        reply.addString("get_navigation_status");
        reply.addString("stop");
        reply.addString("pause");
        reply.addString("resume");
        reply.addString("get_current_loc");
        reply.addString("initLoc <map_name> <x> <y> <angle in degrees>");
    }
    else if (command.get(0).asString() == "store_current_location")
    {
        bool ret = this->storeCurrentPosition(command.get(1).asString());
        if (ret)
        {
            reply.addString("store_current_location done");
        }
        else
        {
            reply.addString("store_current_location failed");
        }
    }
    else if (command.get(0).asString() == "gotoAbs")
    {
        Map2DLocation loc;
        loc.map_id = command.get(1).asString();
        loc.x = command.get(2).asFloat64();
        loc.y = command.get(3).asFloat64();
        if (command.size() == 5)
        {
            loc.theta = command.get(4).asFloat64();
        }
        else
        {
            loc.theta = nan("");
        }

        bool ret = this->gotoTargetByAbsoluteLocation(loc);
        if (ret)
        {
            reply.addString("gotoTargetByAbsoluteLocation() executed successfully");
        }
        else
        {
            reply.addString("gotoTargetByAbsoluteLocation() returned an error");
        }
    }

    else if (command.get(0).asString() == "gotoRel")
    {
        yarp::sig::Vector v;
        double x = command.get(1).asFloat64();
        double y = command.get(2).asFloat64();
        bool ret;
        if (command.size() == 4)
        {
            double t = command.get(3).asFloat64();
            ret = this->gotoTargetByRelativeLocation(x, y, t);
        }
        else
        {
            ret = this->gotoTargetByRelativeLocation(x, y);
        }

        if (ret)
        {
            reply.addString("gotoTargetByRelativeLocation() executed successfully");
        }
        else
        {
            reply.addString("gotoTargetByRelativeLocation() returned an error");
        }
    }
    else if (command.get(0).asString() == "get_location_list")
    {
        std::vector<std::string> locations;
        bool ret = getLocationsList(locations);
        if (ret)
        {
            for (size_t i=0; i < locations.size(); i++)
            {
                reply.addString(locations[i]);
            }
        }
        else
        {
            reply.addString("get_location_list failed");
        }
    }
    else if (command.get(0).asString() == "get_navigation_status")
    {
        yarp::dev::NavigationStatusEnum ss;
        bool ret = this->getNavigationStatus(ss);
        if (ret)
        {
            std::string s = yarp::dev::INavigation2DHelpers::statusToString(ss);
            reply.addString(s.c_str());
        }
        else
        {
            reply.addString("getNavigationStatus() failed");
        }
    }
    else if (command.get(0).isString() && command.get(0).asString() == "get_current_loc")
    {
        Map2DLocation curr_loc;
        bool ret = this->getCurrentPosition(curr_loc);
        if (ret)
        {
            std::string s = std::string("Current Location is: ") + curr_loc.toString();
            reply.addString(s);
        }
        else
        {
            reply.addString("getCurrentPosition() failed");
        }
    }
    else if (command.get(0).isString() && command.get(0).asString() == "initLoc")
    {
        Map2DLocation init_loc;
        init_loc.map_id = command.get(1).asString();
        init_loc.x = command.get(2).asFloat64();
        init_loc.y = command.get(3).asFloat64();
        init_loc.theta = command.get(4).asFloat64();
        bool ret = this->setInitialPose(init_loc);
        if (ret)
        {
            std::string s = std::string("Localization initialized to: ") + init_loc.toString();
            reply.addString(s);
        }
        else
        {
            reply.addString("setInitialPose() failed");
        }
    }
    else if (command.get(0).asString() == "store_location")
    {
        if (command.size() != 6)
        {
            reply.addString("store_location failed (invalid params)");
        }
        else
        {
            Map2DLocation loc;
            loc.map_id = command.get(2).asString();
            loc.x = command.get(3).asFloat64();
            loc.y = command.get(4).asFloat64();
            loc.theta = command.get(5).asFloat64();
            bool ret = this->storeLocation(command.get(1).asString(), loc);
            if (ret)
            {
                reply.addString("store_location done");
            }
            else
            {
                reply.addString("store_location failed");
            }
        }
    }
    else if (command.get(0).asString() == "delete_location")
    {
        bool ret = this->deleteLocation(command.get(1).asString());
        if (ret)
        {
            reply.addString("delete_location done");
        }
        else
        {
            reply.addString("delete_location failed");
        }
    }
    else if (command.get(0).asString() == "clear_all_locations")
    {
        std::vector<std::string> locations;
        bool ret = getLocationsList(locations);
        if (ret)
        {
            for (size_t i = 0; i < locations.size(); i++)
            {
                bool ret = this->deleteLocation(locations[i]);
                if (ret == false)
                {
                    reply.addString("clear_all_locations failed");
                }
            }
            reply.addString("clear_all_locations done");
        }
        else
        {
            reply.addString("clear_all_locations failed");
        }
    }
    else if (command.get(0).asString() == "goto")
    {
        bool ret = this->gotoTargetByLocationName(command.get(1).asString());
        if (ret)
        {
            reply.addString("goto done");
        }
        else
        {
            reply.addString("goto failed");
        }

    }
    else if (command.get(0).asString() == "get_last_target")
    {
        std::string last_target;
        bool b = this->getNameOfCurrentTarget(last_target);
        if (b)
        {
            reply.addString(last_target);
        }
        else
        {
            yError() << "get_last_target failed: goto <location_name> target not found.";
            reply.addString("not found");
        }
    }
    else if (command.get(0).asString() == "stop")
    {
        this->stopNavigation();
        reply.addString("Stopping movement.");
    }
    else if (command.get(0).asString() == "pause")
    {
        double time = -1;
        if (command.size() > 1)
            time = command.get(1).asFloat64();
        this->suspendNavigation(time);
        reply.addString("Pausing.");
    }
    else if (command.get(0).asString() == "resume")
    {
        this->resumeNavigation();
        reply.addString("Resuming.");
    }
    else
    {
        yError() << "Unknown command";
        reply.addVocab(VOCAB_ERR);
    }
    return true;
}

bool Navigation2DClient::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) return false;
    reply.clear();

    if (command.get(0).isString())
    {
        parse_respond_string(command, reply);
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

bool Navigation2DClient::getNavigationStatus(NavigationStatusEnum& status)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_NAVIGATION_STATUS);
    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getNavigationStatus() received error from navigation server";
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
        yError() << "Navigation2DClient::getNavigationStatus() error on writing on rpc port";
        return false;
    }
    return true;
}


bool Navigation2DClient::gotoTargetByAbsoluteLocation(Map2DLocation loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GOTOABS);
    b.addString(loc.map_id);
    b.addFloat64(loc.x);
    b.addFloat64(loc.y);
    b.addFloat64(loc.theta);
    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::gotoTargetByAbsoluteLocation() received error from navigation server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::gotoTargetByAbsoluteLocation() error on writing on rpc port";
        return false;
    }

    reset_current_goal_name();
    return true;
}

bool Navigation2DClient::locations_are_similar(Map2DLocation loc1, Map2DLocation loc2, double linear_tolerance, double angular_tolerance)
{
    if (linear_tolerance < 0) return false;
    if (angular_tolerance < 0) return false;
    yAssert(linear_tolerance >= 0);
    yAssert(angular_tolerance >= 0);

    if (loc1.map_id != loc2.map_id)
    {
        return false;
    }
    if (sqrt(pow((loc1.x - loc2.x),2) + pow((loc1.y - loc2.y),2)) > linear_tolerance)
    {
        return false;
    }

    if (angular_tolerance != std::numeric_limits<double>::infinity() &&
        fabs(normalize_angle(loc1.theta) - normalize_angle(loc2.theta)) > angular_tolerance)
    {
        return false;
    }
    return true;
}

bool Navigation2DClient::checkNearToLocation(Map2DLocation loc, double linear_tolerance, double angular_tolerance)
{
    Map2DLocation curr_loc;
    if (getCurrentPosition(curr_loc) == false)
    {
        yError() << "Navigation2DClient::checkInsideArea() unable to get robot position";
        return false;
    }

    return locations_are_similar(loc, curr_loc, linear_tolerance, angular_tolerance);

    return true;
}

bool Navigation2DClient::checkNearToLocation(std::string location_name, double linear_tolerance, double angular_tolerance)
{
    Map2DLocation loc;
    Map2DLocation curr_loc;
    if (this->getLocation(location_name, loc) == false)
    {
        yError() << "Location" << location_name << "not found";
        return false;
    }

    if (getCurrentPosition(curr_loc) == false)
    {
        yError() << "Navigation2DClient::checkInsideArea() unable to get robot position";
        return false;
    }

    return locations_are_similar(loc, curr_loc, linear_tolerance, angular_tolerance);
}

bool  Navigation2DClient::checkInsideArea(Map2DArea area)
{
    Map2DLocation curr_loc;
    if (getCurrentPosition(curr_loc) == false)
    {
        yError() << "Navigation2DClient::checkInsideArea() unable to get robot position";
        return false;
    }

    if (area.checkLocationInsideArea(curr_loc) == false)
    {
        //yDebug() << "Not inside Area";
        return false;
    }

    return true;
}

bool Navigation2DClient::checkInsideArea(std::string area_name)
{
    Map2DLocation curr_loc;
    Map2DArea area;
    if (this->getArea(area_name, area) == false)
    {
        yError() << "Area" << area_name << "not found";
        return false;
    }

    if (getCurrentPosition(curr_loc) == false)
    {
        yError() << "Navigation2DClient::checkInsideArea() unable to get robot position";
        return false;
    }

    if (area.checkLocationInsideArea(curr_loc) == false)
    {
        //yDebug() << "Not inside Area";
        return false;
    }

    return true;
}

bool Navigation2DClient::gotoTargetByLocationName(std::string location_name)
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
        yError() << "Location not found";
        return false;
    }

    //...otherwise we can go to the found/computed location!
    this->gotoTargetByAbsoluteLocation(loc);
    set_current_goal_name(location_name);

    return true;
}

bool Navigation2DClient::gotoTargetByRelativeLocation(double x, double y)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GOTOREL);
    b.addFloat64(x);
    b.addFloat64(y);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::gotoTargetByRelativeLocation() received error from navigation server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::gotoTargetByRelativeLocation() error on writing on rpc port";
        return false;
    }

    reset_current_goal_name();
    return true;
}

bool Navigation2DClient::gotoTargetByRelativeLocation(double x, double y, double theta)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GOTOREL);
    b.addFloat64(x);
    b.addFloat64(y);
    b.addFloat64(theta);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::gotoTargetByRelativeLocation() received error from navigation server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::gotoTargetByRelativeLocation() error on writing on rpc port";
        return false;
    }

    reset_current_goal_name();
    return true;
}

bool  Navigation2DClient::recomputeCurrentNavigationPath()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_RECOMPUTE_PATH);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::recomputeCurrentNavigationPath() received error from navigation server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::recomputeCurrentNavigationPath() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Navigation2DClient::setInitialPose(const Map2DLocation& loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_SET_INITIAL_POS);
    b.addString(loc.map_id);
    b.addFloat64(loc.x);
    b.addFloat64(loc.y);
    b.addFloat64(loc.theta);

    bool ret = m_rpc_port_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::setInitialPose() received error from localization server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::setInitialPose() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Navigation2DClient::getCurrentPosition(Map2DLocation& loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_CURRENT_POS);

    bool ret = m_rpc_port_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK || resp.size() != 5)
        {
            yError() << "Navigation2DClient::getCurrentPosition() received error from localization server";
            return false;
        }
        else
        {
            loc.map_id = resp.get(1).asString();
            loc.x = resp.get(2).asFloat64();
            loc.y = resp.get(3).asFloat64();
            loc.theta = resp.get(4).asFloat64();
            return true;
        }
    }
    else
    {
        yError() << "Navigation2DClient::getCurrentPosition() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::suspendNavigation(const double time_s)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_SUSPEND);
    b.addFloat64(time_s);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::suspendNavigation() received error from navigation server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::suspendNavigation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::getAbsoluteLocationOfCurrentTarget(Map2DLocation &loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_ABS_TARGET);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK || resp.size() != 5)
        {
            yError() << "Navigation2DClient::getAbsoluteLocationOfCurrentTarget() received error from navigation server";
            return false;
        }
        else
        {
            loc.map_id = resp.get(1).asString();
            loc.x = resp.get(2).asFloat64();
            loc.y = resp.get(3).asFloat64();
            loc.theta = resp.get(4).asFloat64();
            return true;
        }
    }
    else
    {
        yError() << "Navigation2DClient::getAbsoluteLocationOfCurrentTarget() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::getNameOfCurrentTarget(std::string& location_name)
{
    std::string s;
    if (get_current_goal_name(s))
    {
        location_name = s;
        return true;
    }

    location_name = "";
    yError() << "No name for the current target, or no target set";
    return true;
}

bool Navigation2DClient::getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_REL_TARGET);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK || resp.size()!=4)
        {
            yError() << "Navigation2DClient::getRelativeLocationOfCurrentTarget() received error from navigation server";
            return false;
        }
        else
        {
            x = resp.get(1).asFloat64();
            y = resp.get(2).asFloat64();
            theta = resp.get(3).asFloat64();
            return true;
        }
    }
    else
    {
        yError() << "Navigation2DClient::getRelativeLocationOfCurrentTarget() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::storeCurrentPosition(std::string location_name)
{
    yarp::os::Bottle b_nav;
    yarp::os::Bottle resp_nav;
    yarp::os::Bottle b_loc;
    yarp::os::Bottle resp_loc;
    Map2DLocation loc;

    b_nav.addVocab(VOCAB_INAVIGATION);
    b_nav.addVocab(VOCAB_NAV_GET_CURRENT_POS);
    bool ret_nav = m_rpc_port_localization_server.write(b_nav, resp_nav);
    if (ret_nav)
    {
        if (resp_nav.get(0).asVocab() != VOCAB_OK || resp_nav.size()!=5)
        {
            yError() << "Navigation2DClient::storeCurrentPosition() received error from localization server";
            return false;
        }
        else
        {
            loc.map_id = resp_nav.get(1).asString();
            loc.x = resp_nav.get(2).asFloat64();
            loc.y = resp_nav.get(3).asFloat64();
            loc.theta = resp_nav.get(4).asFloat64();
        }
    }
    else
    {
        yError() << "Navigation2DClient::storeCurrentPosition() error on writing on rpc port";
        return false;
    }

    b_loc.addVocab(VOCAB_INAVIGATION);
    b_loc.addVocab(VOCAB_NAV_STORE_X);
    b_loc.addVocab(VOCAB_NAV_LOCATION);
    b_loc.addString(location_name);
    b_loc.addString(loc.map_id);
    b_loc.addFloat64(loc.x);
    b_loc.addFloat64(loc.y);
    b_loc.addFloat64(loc.theta);

    bool ret_loc = m_rpc_port_map_locations_server.write(b_loc, resp_loc);
    if (ret_loc)
    {
        if (resp_loc.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::storeCurrentPosition() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::storeCurrentPosition() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::storeLocation(std::string location_name, Map2DLocation loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_STORE_X);
    b.addVocab(VOCAB_NAV_LOCATION);
    b.addString(location_name);
    b.addString(loc.map_id);
    b.addFloat64(loc.x);
    b.addFloat64(loc.y);
    b.addFloat64(loc.theta);

    bool ret = m_rpc_port_map_locations_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::storeLocation() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::storeLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::getLocationsList(std::vector<std::string>& locations)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_LIST_X);
    b.addVocab(VOCAB_NAV_LOCATION);

    bool ret = m_rpc_port_map_locations_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getLocationsList() received error from locations server";
            return false;
        }
        else
        {
            Bottle* list = resp.get(1).asList();
            if (list)
            {
                locations.clear();
                for (size_t i = 0; i < list->size(); i++)
                {
                    locations.push_back(list->get(i).asString());
                }
                return true;
            }
            else
            {
                yError() << "Navigation2DClient::getLocationsList() error while reading from locations server";
                return false;
            }
        }
    }
    else
    {
        yError() << "Navigation2DClient::getLocationsList() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::getLocation(std::string location_name, Map2DLocation& loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_X);
    b.addVocab(VOCAB_NAV_LOCATION);
    b.addString(location_name);

    bool ret = m_rpc_port_map_locations_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getLocation() received error from locations server";
            return false;
        }
        else
        {
            loc.map_id = resp.get(1).asString();
            loc.x = resp.get(2).asFloat64();
            loc.y = resp.get(3).asFloat64();
            loc.theta = resp.get(4).asFloat64();
        }
    }
    else
    {
        yError() << "Navigation2DClient::getLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::getArea(std::string area_name, Map2DArea& area)
{
    yarp::os::Bottle b_loc;
    yarp::os::Bottle resp_loc;

    {
        b_loc.clear();
        b_loc.addVocab(VOCAB_INAVIGATION);
        b_loc.addVocab(VOCAB_NAV_GET_X);
        b_loc.addVocab(VOCAB_NAV_AREA);
        b_loc.addString(area_name);

        bool ret = m_rpc_port_map_locations_server.write(b_loc, resp_loc);
        if (ret)
        {
            if (resp_loc.get(0).asVocab() != VOCAB_OK)
            {
                yError() << "Navigation2DClient::getArea() received error from locations server";
                return false;
            }
            else
            {
                Value& b = resp_loc.get(1);
                if (Property::copyPortable(b, area) == false)
                {
                    yError() << "Navigation2DClient::getArea() received error from locations server";
                    return false;
                }
            }
        }
        else
        {
            yError() << "Navigation2DClient::getArea() error on writing on rpc port";
            return false;
        }
    }
    return true;
}

bool Navigation2DClient::deleteLocation(std::string location_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_DELETE_X);
    b.addVocab(VOCAB_NAV_LOCATION);
    b.addString(location_name);

    bool ret = m_rpc_port_map_locations_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::deleteLocation() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::deleteLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::clearAllLocations()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_CLEAR_X);
    b.addVocab(VOCAB_NAV_LOCATION);

    bool ret = m_rpc_port_map_locations_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::clearAllLocations() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::clearAllLocations() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::stopNavigation()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_STOP);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::stopNavigation() received error from navigation server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::stopNavigation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::resumeNavigation()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_RESUME);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::resumeNavigation() received error from navigation server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::resumeNavigation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Navigation2DClient::getAllNavigationWaypoints(std::vector<Map2DLocation>& waypoints)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_NAVIGATION_WAYPOINTS);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getNavigationWaypoints() received error from navigation server";
            return false;
        }
        else if (resp.get(1).isList() && resp.get(1).asList()->size()>0)
        {
            waypoints.clear();
            Bottle* waypoints_bottle = resp.get(1).asList();
            if (waypoints_bottle == 0) { yError() << "getNavigationWaypoints parsing error"; return false; }
            for (size_t i = 0; i < waypoints_bottle->size(); i++)
            {
                Bottle* the_waypoint = waypoints_bottle->get(i).asList();
                if (the_waypoint == 0) { yError() << "getNavigationWaypoints parsing error"; return false; }
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
        yError() << "Navigation2DClient::getCurrentPosition() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Navigation2DClient::getCurrentNavigationWaypoint(Map2DLocation& curr_waypoint)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_CURRENT_WAYPOINT);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getCurrentNavigationWaypoint() received error from navigation server";
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
        yError() << "Navigation2DClient::getCurrentPosition() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::getCurrentNavigationMap(yarp::dev::NavigationMapTypeEnum map_type,MapGrid2D& map)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_NAV_MAP);
    b.addVocab(map_type);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getCurrentNavigationMap() received error from server";
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
                yError() << "Navigation2DClient::getCurrentNavigationMap() failed copyPortable()";
                return false;
            }
        }
    }
    else
    {
        yError() << "Navigation2DClient::getCurrentNavigationMap() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Navigation2DClient::getLocalizationStatus(yarp::dev::LocalizationStatusEnum& status)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_LOCALIZER_STATUS);

    bool ret = m_rpc_port_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK || resp.size() != 2)
        {
            yError() << "Navigation2DClient::getLocalizationStatus() received error from localization server";
            return false;
        }
        else
        {
            status = (yarp::dev::LocalizationStatusEnum)(resp.get(1).asVocab());
            return true;
        }
    }
    else
    {
        yError() << "Navigation2DClient::getLocalizationStatus() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Navigation2DClient::applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_VELOCITY_CMD);
    b.addFloat64(x_vel);
    b.addFloat64(y_vel);
    b.addFloat64(theta_vel);
    b.addFloat64(timeout);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::applyVelocityCommand() received error from navigation server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::applyVelocityCommand() error on writing on rpc port";
        return false;
    }

    reset_current_goal_name();
    return true;
}

bool  Navigation2DClient::getEstimatedPoses(std::vector<Map2DLocation>& poses)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_LOCALIZER_POSES);

    bool ret = m_rpc_port_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getEstimatedPoses() received error from localization server";
            return false;
        }
        else
        {
            int nposes = resp.get(1).asInt32();
            poses.clear();
            for (int i = 0; i < nposes; i++)
            {
                Map2DLocation loc;
                Bottle* b = resp.get(2 + i).asList();
                if (b)
                {
                    loc.map_id = b->get(0).asString();
                    loc.x = b->get(1).asFloat64();
                    loc.y = b->get(2).asFloat64();
                    loc.theta = b->get(3).asFloat64();
                }
                else
                {
                    poses.clear();
                    yError() << "Navigation2DClient::getEstimatedPoses() parsing error";
                    return false;
                }
                poses.push_back(loc);
            }
            return true;
        }
    }
    else
    {
        yError() << "Navigation2DClient::getEstimatedPoses() error on writing on rpc port";
        return false;
    }
    return true;
}

//this function receives an angle from (-inf,+inf) and returns an angle in (0,180) or (-180,0)
double Navigation2DClient::normalize_angle(double angle)
{
    angle = std::remainder(angle, 360);

    if (angle > 180 && angle < 360)
    {
        angle = angle - 360;
    }

    if (angle<-180 && angle>-360)
    {
        angle = angle + 360;
    }
    return angle;
}
