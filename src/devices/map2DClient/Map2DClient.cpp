/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

/*! \file Map2DClient.cpp */

#include "Map2DClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/GenericVocabs.h>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(MAP2DCLIENT, "yarp.device.map2DClient")
}

//------------------------------------------------------------------------------------------------------------------------------

bool Map2DClient::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_map_server.clear();

    m_local_name       = config.find("local").asString();
    m_map_server       = config.find("remote").asString();

    if (m_local_name == "")
    {
        yCError(MAP2DCLIENT, "open() error you have to provide valid local name");
        return false;
    }
    if (m_map_server == "")
    {
        yCError(MAP2DCLIENT, "open() error you have to provide valid remote name");
        return false;
    }

    std::string local_rpc1 = m_local_name;
    local_rpc1 += "/mapClient_rpc";

    std::string remote_rpc1 = m_map_server;
    remote_rpc1 += "/rpc";

    if (!m_rpcPort_to_Map2DServer.open(local_rpc1))
    {
        yCError(MAP2DCLIENT, "open() error could not open rpc port %s, check network", local_rpc1.c_str());
        return false;
    }

    bool ok=false;
    ok=Network::connect(local_rpc1, remote_rpc1);
    if (!ok)
    {
        yCError(MAP2DCLIENT, "open() error could not connect to %s", remote_rpc1.c_str());
        return false;
    }

    return true;
}

bool Map2DClient::store_map(const MapGrid2D& map)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_IMAP);
    b.addVocab32(VOCAB_IMAP_SET_MAP);
    yarp::os::Bottle& mapbot = b.addList();
    MapGrid2D maptemp = map;
    if (Property::copyPortable(maptemp, mapbot) == false)
    {
        yCError(MAP2DCLIENT) << "store_map() failed copyPortable()";
        return false;
    }
    //yCDebug(MAP2DCLIENT) << b.toString();
    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_IMAP_OK)
        {
            yCError(MAP2DCLIENT) << "store_map() received error from server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "store_map() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::get_map(std::string map_name, MapGrid2D& map)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_IMAP);
    b.addVocab32(VOCAB_IMAP_GET_MAP);
    b.addString(map_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_IMAP_OK)
        {
            yCError(MAP2DCLIENT) << "get_map() received error from server";
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
                yCError(MAP2DCLIENT) << "get_map() failed copyPortable()";
                return false;
            }
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "get_map() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::clearAllMaps()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_IMAP);
    b.addVocab32(VOCAB_IMAP_CLEAR_ALL_MAPS);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_IMAP_OK)
        {
            yCError(MAP2DCLIENT) << "clear() received error from server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "clear() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::get_map_names(std::vector<std::string>& map_names)
{
    map_names.clear();
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_IMAP);
    b.addVocab32(VOCAB_IMAP_GET_NAMES);
    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_IMAP_OK)
        {
            yCError(MAP2DCLIENT) << "get_map_names() received error from server";
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
        yCError(MAP2DCLIENT) << "get_map_names() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::remove_map(std::string map_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_IMAP);
    b.addVocab32(VOCAB_IMAP_REMOVE);
    b.addString(map_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_IMAP_OK)
        {
            yCError(MAP2DCLIENT) << "remove_map() received error from server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "remove_map() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::storeLocation(std::string location_name, Map2DLocation loc)
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

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "storeLocation() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "storeLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::storeArea(std::string area_name, Map2DArea area)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_STORE_X);
    b.addVocab32(VOCAB_NAV_AREA);
    b.addString(area_name);
    yarp::os::Bottle& areabot = b.addList();
    Map2DArea areatemp = area;
    if (Property::copyPortable(areatemp, areabot) == false)
    {
        yCError(MAP2DCLIENT) << "storeArea() failed copyPortable()";
        return false;
    }

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "storeArea() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "storeArea() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::storePath(std::string path_name, Map2DPath path)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_STORE_X);
    b.addVocab32(VOCAB_NAV_PATH);
    b.addString(path_name);
    yarp::os::Bottle& areabot = b.addList();
    Map2DPath pathtemp = path;
    if (Property::copyPortable(pathtemp, areabot) == false)
    {
        yCError(MAP2DCLIENT) << "storePath() failed copyPortable()";
        return false;
    }

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "storePath() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "storePath() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::getLocationsList(std::vector<std::string>& locations)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_LIST_X);
    b.addVocab32(VOCAB_NAV_LOCATION);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "getLocationsList() received error from locations server";
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
                yCError(MAP2DCLIENT) << "getLocationsList() error while reading from locations server";
                return false;
            }
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "getLocationsList() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::getAreasList(std::vector<std::string>& areas)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_LIST_X);
    b.addVocab32(VOCAB_NAV_AREA);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "getAreasList() received error from locations server";
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
                yCError(MAP2DCLIENT) << "getAreasList() error while reading from locations server";
                return false;
            }
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "getAreasList() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::getPathsList(std::vector<std::string>& paths)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_LIST_X);
    b.addVocab32(VOCAB_NAV_PATH);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "getPathsList() received error from locations server";
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
                yCError(MAP2DCLIENT) << "getPathsList() error while reading from locations server";
                return false;
            }
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "getPathsList() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::getLocation(std::string location_name, Map2DLocation& loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_X);
    b.addVocab32(VOCAB_NAV_LOCATION);
    b.addString(location_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "getLocation() received error from locations server";
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
        yCError(MAP2DCLIENT) << "getLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::getArea(std::string location_name, Map2DArea& area)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_X);
    b.addVocab32(VOCAB_NAV_AREA);
    b.addString(location_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "getArea() received error from locations server";
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
                yCError(MAP2DCLIENT) << "getArea() failed copyPortable()";
                return false;
            }
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "getArea() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::getPath(std::string path_name, Map2DPath& path)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_GET_X);
    b.addVocab32(VOCAB_NAV_PATH);
    b.addString(path_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "getPath() received error from locations server";
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
                yCError(MAP2DCLIENT) << "getPath() failed copyPortable()";
                return false;
            }
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "getPath() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::deleteLocation(std::string location_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_DELETE_X);
    b.addVocab32(VOCAB_NAV_LOCATION);
    b.addString(location_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "deleteLocation() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "deleteLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::renameLocation(std::string original_name, std::string new_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_RENAME_X);
    b.addVocab32(VOCAB_NAV_LOCATION);
    b.addString(original_name);
    b.addString(new_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "renameLocation() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "renameLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::deleteArea(std::string location_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_DELETE_X);
    b.addVocab32(VOCAB_NAV_AREA);
    b.addString(location_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "deleteArea() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "deleteArea() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::deletePath(std::string path_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_DELETE_X);
    b.addVocab32(VOCAB_NAV_PATH);
    b.addString(path_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "deletePath() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "deletePath() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::renameArea(std::string original_name, std::string new_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_RENAME_X);
    b.addVocab32(VOCAB_NAV_AREA);
    b.addString(original_name);
    b.addString(new_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "renameArea() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "renameArea() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::renamePath(std::string original_name, std::string new_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_RENAME_X);
    b.addVocab32(VOCAB_NAV_PATH);
    b.addString(original_name);
    b.addString(new_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "renamePath() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "renamePath() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::clearAllLocations()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_CLEARALL_X);
    b.addVocab32(VOCAB_NAV_LOCATION);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "clearAllLocations() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "clearAllLocations() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::clearAllAreas()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_CLEARALL_X);
    b.addVocab32(VOCAB_NAV_AREA);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "clearAllAreas() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "clearAllAreas() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::clearAllMapsTemporaryFlags()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_CLEARALL_X);
    b.addVocab32(VOCAB_NAV_TEMPORARY_FLAGS);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "clearAllMapsTemporaryFlags() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "clearAllAreas() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::clearMapTemporaryFlags(std::string map_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_DELETE_X);
    b.addVocab32(VOCAB_NAV_TEMPORARY_FLAGS);
    b.addString(map_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "clearMapTemporaryFlags() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "clearAllAreas() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   Map2DClient::clearAllPaths()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_CLEARALL_X);
    b.addVocab32(VOCAB_NAV_PATH);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "clearAllPaths() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "clearAllPaths() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::close()
{
    return true;
}

bool Map2DClient::saveMapsCollection(std::string maps_collection)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_IMAP);
    b.addVocab32(VOCAB_IMAP_SAVE_X);
    b.addVocab32(VOCAB_IMAP_MAPS_COLLECTION);
    b.addString(maps_collection);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "saveMapsCollection() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "saveMapsCollection() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::loadMapsCollection(std::string maps_collection)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_IMAP);
    b.addVocab32(VOCAB_IMAP_LOAD_X);
    b.addVocab32(VOCAB_IMAP_MAPS_COLLECTION);
    b.addString(maps_collection);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MAP2DCLIENT) << "loadMapsCollection() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "loadMapsCollection() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::saveLocationsAndExtras(std::string locations_collection)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_IMAP);
    b.addVocab32(VOCAB_IMAP_SAVE_X);
    b.addVocab32(VOCAB_IMAP_LOCATIONS_COLLECTION);
    b.addString(locations_collection);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_IMAP_OK)
        {
            yCError(MAP2DCLIENT) << "saveLocationsAndExtras() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "saveLocationsAndExtras() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Map2DClient::loadLocationsAndExtras(std::string locations_collection)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_IMAP);
    b.addVocab32(VOCAB_IMAP_LOAD_X);
    b.addVocab32(VOCAB_IMAP_LOCATIONS_COLLECTION);
    b.addString(locations_collection);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_IMAP_OK)
        {
            yCError(MAP2DCLIENT) << "loadLocationsAndExtras() received error from locations server";
            return false;
        }
    }
    else
    {
        yCError(MAP2DCLIENT) << "loadLocationsAndExtras() error on writing on rpc port";
        return false;
    }
    return true;
}
