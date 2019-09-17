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

/*! \file Map2DClient.cpp */

#include "Map2DClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/GenericVocabs.h>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace yarp::sig;

//------------------------------------------------------------------------------------------------------------------------------

bool Map2DClient::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_map_server.clear();

    m_local_name       = config.find("local").asString();
    m_map_server       = config.find("remote").asString();

    if (m_local_name == "")
    {
        yError("Map2DClient::open() error you have to provide valid local name");
        return false;
    }
    if (m_map_server == "")
    {
        yError("Map2DClient::open() error you have to provide valid remote name");
        return false;
    }

    std::string local_rpc1 = m_local_name;
    local_rpc1 += "/mapClient_rpc";

    std::string remote_rpc1 = m_map_server;
    remote_rpc1 += "/rpc";

    if (!m_rpcPort_to_Map2DServer.open(local_rpc1))
    {
        yError("Map2DClient::open() error could not open rpc port %s, check network", local_rpc1.c_str());
        return false;
    }

    bool ok=false;
    ok=Network::connect(local_rpc1, remote_rpc1);
    if (!ok)
    {
        yError("Map2DClient::open() error could not connect to %s", remote_rpc1.c_str());
        return false;
    }

    return true;
}

bool Map2DClient::store_map(const MapGrid2D& map)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_IMAP);
    b.addVocab(VOCAB_IMAP_SET_MAP);
    yarp::os::Bottle& mapbot = b.addList();
    MapGrid2D maptemp = map;
    if (Property::copyPortable(maptemp, mapbot) == false)
    {
        yError() << "Map2DClient::store_map() failed copyPortable()";
        return false;
    }
    //yDebug() << b.toString();
    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_IMAP_OK)
        {
            yError() << "Map2DClient::store_map() received error from server";
            return false;
        }
    }
    else
    {
        yError() << "Map2DClient::store_map() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::get_map(std::string map_name, MapGrid2D& map)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_IMAP);
    b.addVocab(VOCAB_IMAP_GET_MAP);
    b.addString(map_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_IMAP_OK)
        {
            yError() << "Map2DClient::get_map() received error from server";
            return false;
        }
        else
        {
            Value& bt = resp.get(1);
            if (Property::copyPortable(bt, map))
            {
                return true;
            }
            else
            {
                yError() << "Map2DClient::get_map() failed copyPortable()";
                return false;
            }
        }
    }
    else
    {
        yError() << "Map2DClient::get_map() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::clearAllMaps()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_IMAP);
    b.addVocab(VOCAB_IMAP_CLEAR);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_IMAP_OK)
        {
            yError() << "Map2DClient::clear() received error from server";
            return false;
        }
    }
    else
    {
        yError() << "Map2DClient::clear() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::get_map_names(std::vector<std::string>& map_names)
{
    map_names.clear();
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_IMAP);
    b.addVocab(VOCAB_IMAP_GET_NAMES);
    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_IMAP_OK)
        {
            yError() << "Map2DClient::get_map_names() received error from server";
            return false;
        }
        else
        {
            for (size_t i = 1; i < resp.size(); i++)
            {
                map_names.push_back(resp.get(i).asString());
            }
            return true;
        }
    }
    else
    {
        yError() << "Map2DClient::get_map_names() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::remove_map(std::string map_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_IMAP);
    b.addVocab(VOCAB_IMAP_REMOVE);
    b.addString(map_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_IMAP_OK)
        {
            yError() << "Map2DClient::remove_map() received error from server";
            return false;
        }
    }
    else
    {
        yError() << "Map2DClient::remove_map() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::storeLocation(std::string location_name, Map2DLocation loc)
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

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
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

bool Map2DClient::storeArea(std::string area_name, Map2DArea area)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_STORE_X);
    b.addVocab(VOCAB_NAV_AREA);
    b.addString(area_name);
    yarp::os::Bottle& areabot = b.addList();
    Map2DArea areatemp = area;
    if (Property::copyPortable(areatemp, areabot) == false)
    {
        yError() << "Map2DClient::storeArea() failed copyPortable()";
        return false;
    }

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Map2DClient::storeArea() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Map2DClient::storeArea() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::storePath(std::string path_name, Map2DPath path)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_STORE_X);
    b.addVocab(VOCAB_NAV_PATH);
    b.addString(path_name);
    yarp::os::Bottle& areabot = b.addList();
    Map2DPath pathtemp = path;
    if (Property::copyPortable(pathtemp, areabot) == false)
    {
        yError() << "Map2DClient::storePath() failed copyPortable()";
        return false;
    }

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Map2DClient::storePath() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Map2DClient::storePath() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::getLocationsList(std::vector<std::string>& locations)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_LIST_X);
    b.addVocab(VOCAB_NAV_LOCATION);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
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

bool   Map2DClient::getAreasList(std::vector<std::string>& areas)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_LIST_X);
    b.addVocab(VOCAB_NAV_AREA);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getAreasList() received error from locations server";
            return false;
        }
        else
        {
            Bottle* list = resp.get(1).asList();
            if (list)
            {
                areas.clear();
                for (size_t i = 0; i < list->size(); i++)
                {
                    areas.push_back(list->get(i).asString());
                }
                return true;
            }
            else
            {
                yError() << "Navigation2DClient::getAreasList() error while reading from locations server";
                return false;
            }
        }
    }
    else
    {
        yError() << "Navigation2DClient::getAreasList() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::getPathsList(std::vector<std::string>& paths)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_LIST_X);
    b.addVocab(VOCAB_NAV_PATH);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getPathsList() received error from locations server";
            return false;
        }
        else
        {
            Bottle* list = resp.get(1).asList();
            if (list)
            {
                paths.clear();
                for (size_t i = 0; i < list->size(); i++)
                {
                    paths.push_back(list->get(i).asString());
                }
                return true;
            }
            else
            {
                yError() << "Navigation2DClient::getPathsList() error while reading from locations server";
                return false;
            }
        }
    }
    else
    {
        yError() << "Navigation2DClient::getPathsList() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::getLocation(std::string location_name, Map2DLocation& loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_X);
    b.addVocab(VOCAB_NAV_LOCATION);
    b.addString(location_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
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

bool   Map2DClient::getArea(std::string location_name, Map2DArea& area)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_X);
    b.addVocab(VOCAB_NAV_AREA);
    b.addString(location_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getArea() received error from locations server";
            return false;
        }
        else
        {
            Value& bt = resp.get(1);
            if (Property::copyPortable(bt, area))
            {
                return true;
            }
            else
            {
                yError() << "Navigation2DClient::getArea() failed copyPortable()";
                return false;
            }
        }
    }
    else
    {
        yError() << "Navigation2DClient::getArea() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::getPath(std::string path_name, Map2DPath& path)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_X);
    b.addVocab(VOCAB_NAV_PATH);
    b.addString(path_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getPath() received error from locations server";
            return false;
        }
        else
        {
            Value& bt = resp.get(1);
            if (Property::copyPortable(bt, path))
            {
                return true;
            }
            else
            {
                yError() << "Navigation2DClient::getPath() failed copyPortable()";
                return false;
            }
        }
    }
    else
    {
        yError() << "Navigation2DClient::getPath() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::deleteLocation(std::string location_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_DELETE_X);
    b.addVocab(VOCAB_NAV_LOCATION);
    b.addString(location_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
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

bool   Map2DClient::renameLocation(std::string original_name, std::string new_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_RENAME_X);
    b.addVocab(VOCAB_NAV_LOCATION);
    b.addString(original_name);
    b.addString(new_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::renameLocation() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::renameLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::deleteArea(std::string location_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_DELETE_X);
    b.addVocab(VOCAB_NAV_AREA);
    b.addString(location_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::deleteArea() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::deleteArea() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::deletePath(std::string path_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_DELETE_X);
    b.addVocab(VOCAB_NAV_PATH);
    b.addString(path_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::deletePath() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::deletePath() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::renameArea(std::string original_name, std::string new_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_RENAME_X);
    b.addVocab(VOCAB_NAV_AREA);
    b.addString(original_name);
    b.addString(new_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::renameArea() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::renameArea() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::renamePath(std::string original_name, std::string new_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_RENAME_X);
    b.addVocab(VOCAB_NAV_PATH);
    b.addString(original_name);
    b.addString(new_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::renamePath() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::renamePath() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::clearAllLocations()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_CLEAR_X);
    b.addVocab(VOCAB_NAV_LOCATION);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
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

bool   Map2DClient::clearAllAreas()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_CLEAR_X);
    b.addVocab(VOCAB_NAV_AREA);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::clearAllAreas() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::clearAllAreas() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::clearAllPaths()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_CLEAR_X);
    b.addVocab(VOCAB_NAV_PATH);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::clearAllPaths() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::clearAllPaths() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::close()
{
    return true;
}
