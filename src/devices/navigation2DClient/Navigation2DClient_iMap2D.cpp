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

bool Navigation2DClient::storeLocation(std::string location_name, Map2DLocation loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_STORE_X);
    b.addVocab32(VOCAB_NAV_LOCATION);
    b.addString(location_name);
    b.addString(loc.map_id);
    b.addFloat64(loc.x);
    b.addFloat64(loc.y);
    b.addFloat64(loc.theta);

    bool ret = m_rpc_port_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "storeLocation() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "storeLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::storeArea(std::string location_name, yarp::dev::Nav2D::Map2DArea area)
{
    yCError(NAVIGATION2DCLIENT) << "storeArea() not implemented";
    return false;
}

bool Navigation2DClient::storePath(std::string path_name, yarp::dev::Nav2D::Map2DPath path)
{
    yCError(NAVIGATION2DCLIENT) << "storePath() not implemented";
    return false;
}

bool Navigation2DClient::getLocationsList(std::vector<std::string>& locations)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_LIST_X);
    b.addVocab32(VOCAB_NAV_LOCATION);

    bool ret = m_rpc_port_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "getLocationsList() received error from locations server";
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
                yCError(NAVIGATION2DCLIENT) << "getLocationsList() error while reading from locations server";
                return false;
            }
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "getLocationsList() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::getLocation(std::string location_name, Map2DLocation& loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_X);
    b.addVocab32(VOCAB_NAV_LOCATION);
    b.addString(location_name);

    bool ret = m_rpc_port_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "getLocation() received error from locations server";
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
        yCError(NAVIGATION2DCLIENT) << "getLocation() error on writing on rpc port";
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
        b_loc.addVocab32(VOCAB_INAVIGATION);
        b_loc.addVocab32(VOCAB_NAV_GET_X);
        b_loc.addVocab32(VOCAB_NAV_AREA);
        b_loc.addString(area_name);

        bool ret = m_rpc_port_to_Map2DServer.write(b_loc, resp_loc);
        if (ret)
        {
            if (resp_loc.get(0).asVocab32() != VOCAB_OK)
            {
                yCError(NAVIGATION2DCLIENT) << "getArea() received error from locations server";
                return false;
            }
            else
            {
                Value& b = resp_loc.get(1);
                if (Property::copyPortable(b, area) == false)
                {
                    yCError(NAVIGATION2DCLIENT) << "getArea() received error from locations server";
                    return false;
                }
            }
        }
        else
        {
            yCError(NAVIGATION2DCLIENT) << "getArea() error on writing on rpc port";
            return false;
        }
    }
    return true;
}

bool Navigation2DClient::deleteLocation(std::string location_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_DELETE_X);
    b.addVocab32(VOCAB_NAV_LOCATION);
    b.addString(location_name);

    bool ret = m_rpc_port_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "deleteLocation() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "deleteLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Navigation2DClient::clearAllLocations()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_CLEARALL_X);
    b.addVocab32(VOCAB_NAV_LOCATION);

    bool ret = m_rpc_port_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2DCLIENT) << "clearAllLocations() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2DCLIENT) << "clearAllLocations() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Navigation2DClient::renameLocation(std::string original_name, std::string new_name)
{
    yCError(NAVIGATION2DCLIENT) << "renameLocation() not implemented";
    return false;
}

bool  Navigation2DClient::renameArea(std::string original_name, std::string new_name)
{
    yCError(NAVIGATION2DCLIENT) << "renameArea() not implemented";
    return false;
}

bool  Navigation2DClient::renamePath(std::string original_name, std::string new_name)
{
    yCError(NAVIGATION2DCLIENT) << "renamePath() not implemented";
    return false;
}

bool  Navigation2DClient::getAllLocations(std::vector<yarp::dev::Nav2D::Map2DLocation>& locations)
{
    yCError(NAVIGATION2DCLIENT) << "getAllLocations() not implemented";
    return false;
}

bool  Navigation2DClient::getAllAreas(std::vector<yarp::dev::Nav2D::Map2DArea>& areas)
{
    yCError(NAVIGATION2DCLIENT) << "getAllAreas() not implemented";
    return false;
}

bool  Navigation2DClient::getAllPaths(std::vector<yarp::dev::Nav2D::Map2DPath>& paths)
{
    yCError(NAVIGATION2DCLIENT) << "getAllPaths() not implemented";
    return false;
}

bool Navigation2DClient::clearAllAreas()
{
    yCError(NAVIGATION2DCLIENT) << "clearAllAreas() not implemented";
    return false;
}

bool Navigation2DClient::clearAllPaths()
{
    yCError(NAVIGATION2DCLIENT) << "clearAllPaths() not implemented";
    return false;
}

bool Navigation2DClient::clearAllMapsTemporaryFlags()
{
    yCError(NAVIGATION2DCLIENT) << "clearAllMapsTemporaryFlags() not implemented";
    return false;
}

bool Navigation2DClient::clearMapTemporaryFlags(std::string map_name)
{
    yCError(NAVIGATION2DCLIENT) << "clearMapTemporaryFlags() not implemented";
    return false;
}

bool Navigation2DClient::saveMapsCollection(std::string maps_collection_file)
{
    yCError(NAVIGATION2DCLIENT) << "saveMapsCollection() not implemented";
    return false;
}

bool Navigation2DClient::loadMapsCollection(std::string maps_collection_file)
{
    yCError(NAVIGATION2DCLIENT) << "loadMapsCollection() not implemented";
    return false;
}

bool Navigation2DClient::saveLocationsAndExtras(std::string locations_collection_file)
{
    yCError(NAVIGATION2DCLIENT) << "saveLocationsAndExtras() not implemented";
    return false;
}

bool Navigation2DClient::loadLocationsAndExtras(std::string locations_collection_file)
{
    yCError(NAVIGATION2DCLIENT) << "loadLocationsAndExtras() not implemented";
    return false;
}

bool Navigation2DClient::saveMapToDisk(std::string map_name, std::string file_name)
{
    yCError(NAVIGATION2DCLIENT) << "saveMapToDisk() not implemented";
    return false;
}

bool Navigation2DClient::loadMapFromDisk(std::string file_name)
{
    yCError(NAVIGATION2DCLIENT) << "loadMapFromDisk() not implemented";
    return false;
}

bool Navigation2DClient::enableMapsCompression(bool enable)
{
    yCError(NAVIGATION2DCLIENT) << "enableMapsCompression() not implemented";
    return false;
}

bool Navigation2DClient::deleteArea(std::string location_name)
{
    yCError(NAVIGATION2DCLIENT) << "deleteArea() not implemented";
    return false;
}

bool Navigation2DClient::deletePath(std::string path_name)
{
    yCError(NAVIGATION2DCLIENT) << "deletePath() not implemented";
    return false;
}

bool Navigation2DClient::getAreasList(std::vector<std::string>& locations)
{
    yCError(NAVIGATION2DCLIENT) << "getAreasList() not implemented";
    return false;
}

bool Navigation2DClient::getPathsList(std::vector<std::string>& paths)
{
    yCError(NAVIGATION2DCLIENT) << "getPathsList() not implemented";
    return false;
}

bool Navigation2DClient::getPath(std::string path_name, yarp::dev::Nav2D::Map2DPath& path)
{
    yCError(NAVIGATION2DCLIENT) << "getPath() not implemented";
    return false;
}

bool Navigation2DClient::remove_map(std::string map_name)
{
    yCError(NAVIGATION2DCLIENT) << "remove_map() not implemented";
    return false;
}

bool Navigation2DClient::store_map(const yarp::dev::Nav2D::MapGrid2D& map)
{
    yCError(NAVIGATION2DCLIENT) << "store_map() not implemented";
    return false;
}

bool Navigation2DClient::get_map(std::string map_name, yarp::dev::Nav2D::MapGrid2D& map)
{
    yCError(NAVIGATION2DCLIENT) << "get_map() not implemented";
    return false;
}

bool Navigation2DClient::get_map_names(std::vector<std::string>& map_names)
{
    yCError(NAVIGATION2DCLIENT) << "get_map_names() not implemented";
    return false;
}

bool Navigation2DClient::clearAllMaps()
{
    yCError(NAVIGATION2DCLIENT) << "clearAllMaps() not implemented";
    return false;
}
