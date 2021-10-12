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

bool Navigation2DClient::open(yarp::os::Searchable &config)
{
    yCWarning(NAVIGATION2DCLIENT) << "The 'navigation2DClient' device is deprecated in favour of 'navigation2D_nwc_yarp'.";
    yCWarning(NAVIGATION2DCLIENT) << "The old device is no longer supported, and it will be deprecated in YARP 3.6 and removed in YARP 4.";
    yCWarning(NAVIGATION2DCLIENT) << "Please update your scripts.";

    m_local_name.clear();
    m_navigation_server_name.clear();
    m_map_locations_server_name.clear();
    m_localization_server_name.clear();

    m_local_name = config.find("local").asString();
    m_navigation_server_name = config.find("navigation_server").asString();
    m_map_locations_server_name = config.find("map_locations_server").asString();
    m_localization_server_name = config.find("localization_server").asString();

    m_carrier = config.check("carrier", yarp::os::Value("tcp"), "the carrier used for the connection with the server").asString();

    if (m_local_name == "")
    {
        yCError(NAVIGATION2DCLIENT, "open() error you have to provide a valid 'local' param");
        return false;
    }

    if (m_navigation_server_name == "")
    {
        yCError(NAVIGATION2DCLIENT, "open() error you have to provide a valid 'navigation_server' param");
        return false;
    }

    if (m_map_locations_server_name == "")
    {
        yCError(NAVIGATION2DCLIENT, "open() error you have to provide valid 'map_locations_server' param");
        return false;
    }

    if (m_localization_server_name == "")
    {
        yCError(NAVIGATION2DCLIENT, "open() error you have to provide valid 'localization_server' param");
        return false;
    }

    if (config.check("period"))
    {
        m_period = config.find("period").asInt32();
    }
    else
    {
        m_period = 10;
        yCWarning(NAVIGATION2DCLIENT, "Using default period of %d ms" , m_period);
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

    if (!m_rpc_port_to_navigation_server.open(local_rpc_1))
    {
        yCError(NAVIGATION2DCLIENT, "open() error could not open rpc port %s, check network", local_rpc_1.c_str());
        return false;
    }

    if (!m_rpc_port_to_Map2DServer.open(local_rpc_2))
    {
        yCError(NAVIGATION2DCLIENT, "open() error could not open rpc port %s, check network", local_rpc_2.c_str());
        return false;
    }

    if (!m_rpc_port_to_localization_server.open(local_rpc_3))
    {
        yCError(NAVIGATION2DCLIENT, "open() error could not open rpc port %s, check network", local_rpc_3.c_str());
        return false;
    }

    bool ok = true;

    ok = Network::connect(local_rpc_1, remote_rpc_1, m_carrier);
    if (!ok)
    {
        yCError(NAVIGATION2DCLIENT, "open() error could not connect to %s", remote_rpc_1.c_str());
        return false;
    }

    ok = Network::connect(local_rpc_2, remote_rpc_2, m_carrier);
    if (!ok)
    {
        yCError(NAVIGATION2DCLIENT, "open() error could not connect to %s", remote_rpc_2.c_str());
        return false;
    }

    ok = Network::connect(local_rpc_3, remote_rpc_3, m_carrier);
    if (!ok)
    {
        yCError(NAVIGATION2DCLIENT, "open() error could not connect to %s", remote_rpc_3.c_str());
        return false;
    }

    if (!m_rpc_port_user_commands.open(local_rpc_4.c_str()))
    {
        yCError(NAVIGATION2DCLIENT, "Failed to open port %s", local_rpc_4.c_str());
        return false;
    }
    m_rpc_port_user_commands.setReader(*this);

    return true;
}

bool Navigation2DClient::close()
{
    m_rpc_port_to_navigation_server.close();
    m_rpc_port_to_Map2DServer.close();
    m_rpc_port_to_localization_server.close();
    m_rpc_port_user_commands.close();
    return true;
}

bool Navigation2DClient::parse_respond_string(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
    if (command.get(0).isString() == false)
    {
        yCError(NAVIGATION2DCLIENT) << "General error in parse_respond_string";
        return false;
    }

    if (command.get(0).asString() == "help")
    {
        reply.addVocab32("many");
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
        reply.addString("stop_loc");
        reply.addString("start_loc");
        reply.addString("stop_nav");
        reply.addString("pause_nav");
        reply.addString("resume_nav");
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
        yarp::dev::Nav2D::NavigationStatusEnum ss;
        bool ret = this->getNavigationStatus(ss);
        if (ret)
        {
            std::string s = yarp::dev::Nav2D::INavigation2DHelpers::statusToString(ss);
            reply.addString(s.c_str());
        }
        else
        {
            reply.addString("getNavigationStatus() failed");
        }
    }
    else if (command.get(0).isString() && command.get(0).asString() == "get_current_loc")
    {
        {
            Map2DLocation curr_loc;
            bool ret1 = this->getCurrentPosition(curr_loc);
            if (ret1)
            {
                std::string s = std::string("Current Location is: ") + curr_loc.toString();
                reply.addString(s);
            }
            else
            {
                reply.addString("getCurrentPosition(curr_loc) failed");
            }
        }
        {
            Map2DLocation curr_loc;
            Matrix cov;
            bool ret2 = this->getCurrentPosition(curr_loc, cov);
            if (ret2)
            {
                std::string s = std::string("Current Location with covariance is: ") + curr_loc.toString() + "\n" + cov.toString();
                reply.addString(s);
            }
            else
            {
                reply.addString("getCurrentPosition(curr_loc, covariance) failed");
            }
        }
    }
    else if (command.get(0).isString() && command.get(0).asString() == "initLoc")
    {
        Map2DLocation init_loc;
        bool ret = false;
        if (command.size() == 5)
        {
            init_loc.map_id = command.get(1).asString();
            init_loc.x = command.get(2).asFloat64();
            init_loc.y = command.get(3).asFloat64();
            init_loc.theta = command.get(4).asFloat64();
            ret = this->setInitialPose(init_loc);
        }
        else if (command.size() == 6)
        {
            init_loc.map_id = command.get(1).asString();
            init_loc.x = command.get(2).asFloat64();
            init_loc.y = command.get(3).asFloat64();
            init_loc.theta = command.get(4).asFloat64();
            Bottle* b= command.get(5).asList();
            if (b && b->size()==9)
            {
                yarp::sig::Matrix cov(3,3);
                for (size_t i = 0; i < 3; i++) { for (size_t j = 0; j < 3; j++) { cov[i][j] = b->get(i * 3 + j).asFloat64(); } }
                ret = this->setInitialPose(init_loc, cov);
            } else {
                ret = false;
            }
        }

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
            yCError(NAVIGATION2DCLIENT) << "get_last_target failed: goto <location_name> target not found.";
            reply.addString("not found");
        }
    }
    else if (command.get(0).asString() == "stop_nav")
    {
        this->stopNavigation();
        reply.addString("Stopping movement.");
    }
    else if (command.get(0).asString() == "stop_loc")
    {
        this->stopLocalizationService();
        reply.addString("Stopping localization service.");
    }
    else if (command.get(0).asString() == "pause_nav")
    {
        double time = -1;
        if (command.size() > 1) {
            time = command.get(1).asFloat64();
        }
        this->suspendNavigation(time);
        reply.addString("Pausing.");
    }
    else if (command.get(0).asString() == "resume_nav")
    {
        this->resumeNavigation();
        reply.addString("Resuming.");
    }
    else if (command.get(0).asString() == "start_loc")
    {
        this->startLocalizationService();
        reply.addString("Starting localization service.");
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "Unknown command";
        reply.addVocab32(VOCAB_ERR);
    }
    return true;
}

bool Navigation2DClient::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) {
        return false;
    }
    reply.clear();

    if (command.get(0).isString())
    {
        parse_respond_string(command, reply);
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "Invalid command type";
        reply.addVocab32(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }
    return true;
}

bool Navigation2DClient::checkNearToLocation(Map2DLocation loc, double linear_tolerance, double angular_tolerance)
{
    Map2DLocation curr_loc;
    if (getCurrentPosition(curr_loc) == false)
    {
        yCError(NAVIGATION2DCLIENT) << "checkInsideArea() unable to get robot position";
        return false;
    }

    return curr_loc.is_near_to(loc, linear_tolerance, angular_tolerance);
}

bool Navigation2DClient::checkNearToLocation(std::string location_name, double linear_tolerance, double angular_tolerance)
{
    Map2DLocation loc;
    Map2DLocation curr_loc;
    if (this->getLocation(location_name, loc) == false)
    {
        yCError(NAVIGATION2DCLIENT) << "Location" << location_name << "not found";
        return false;
    }

    if (getCurrentPosition(curr_loc) == false)
    {
        yCError(NAVIGATION2DCLIENT) << "checkInsideArea() unable to get robot position";
        return false;
    }

    return curr_loc.is_near_to(loc, linear_tolerance, angular_tolerance);
}

bool  Navigation2DClient::checkInsideArea(Map2DArea area)
{
    Map2DLocation curr_loc;
    if (getCurrentPosition(curr_loc) == false)
    {
        yCError(NAVIGATION2DCLIENT) << "checkInsideArea() unable to get robot position";
        return false;
    }

    if (area.checkLocationInsideArea(curr_loc) == false)
    {
        //yCDebug(NAVIGATION2DCLIENT) << "Not inside Area";
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
        yCError(NAVIGATION2DCLIENT) << "Area" << area_name << "not found";
        return false;
    }

    if (getCurrentPosition(curr_loc) == false)
    {
        yCError(NAVIGATION2DCLIENT) << "checkInsideArea() unable to get robot position";
        return false;
    }

    if (area.checkLocationInsideArea(curr_loc) == false)
    {
        //yCDebug(NAVIGATION2DCLIENT) << "Not inside Area";
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
        yCError(NAVIGATION2DCLIENT) << "Location not found";
        return false;
    }

    //...otherwise we can go to the found/computed location!
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GOTOABS_AND_NAME);
    b.addString(loc.map_id);
    b.addFloat64(loc.x);
    b.addFloat64(loc.y);
    b.addFloat64(loc.theta);
    b.addString(location_name);
    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "gotoTargetByLocationName() received error from navigation server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "gotoTargetByLocationName() error on writing on rpc port";
        return false;
    }

    return true;
}


bool Navigation2DClient::getNameOfCurrentTarget(std::string& location_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_NAME_TARGET);

    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "getNameOfCurrentTarget() received error from navigation server";
            return false;
        }
        else
        {
            location_name = resp.get(1).asString();
            return true;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "getNameOfCurrentTarget() error on writing on rpc port";
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

    b_nav.addVocab32(VOCAB_INAVIGATION);
    b_nav.addVocab32(VOCAB_NAV_GET_CURRENT_POS);
    bool ret_nav = m_rpc_port_to_localization_server.write(b_nav, resp_nav);
    if (ret_nav)
    {
        if (resp_nav.get(0).asVocab32() != VOCAB_OK || resp_nav.size()!=5)
        {
            yCError(NAVIGATION2DCLIENT) << "storeCurrentPosition() received error from localization server";
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
        yCError(NAVIGATION2DCLIENT) << "storeCurrentPosition() error on writing on rpc port";
        return false;
    }

    b_loc.addVocab32(VOCAB_INAVIGATION);
    b_loc.addVocab32(VOCAB_NAV_STORE_X);
    b_loc.addVocab32(VOCAB_NAV_LOCATION);
    b_loc.addString(location_name);
    b_loc.addString(loc.map_id);
    b_loc.addFloat64(loc.x);
    b_loc.addFloat64(loc.y);
    b_loc.addFloat64(loc.theta);

    bool ret_loc = m_rpc_port_to_Map2DServer.write(b_loc, resp_loc);
    if (ret_loc)
    {
        if (resp_loc.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "storeCurrentPosition() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "storeCurrentPosition() error on writing on rpc port";
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
