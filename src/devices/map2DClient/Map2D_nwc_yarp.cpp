/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

/*! \file Map2D_nwc_yarp.cpp */

#include "Map2D_nwc_yarp.h"
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
YARP_LOG_COMPONENT(MAP2D_NWC_YARP, "yarp.device.map2D_nwc_yarp")
}

//------------------------------------------------------------------------------------------------------------------------------

bool Map2D_nwc_yarp::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_map_server.clear();

    m_local_name       = config.find("local").asString();
    m_map_server       = config.find("remote").asString();

    if (m_local_name.empty())
    {
        yCError(MAP2D_NWC_YARP, "open() error you have to provide valid local name");
        return false;
    }
    if (m_map_server.empty())
    {
        yCError(MAP2D_NWC_YARP, "open() error you have to provide valid remote name");
        return false;
    }

    std::string local_rpc1 = m_local_name;
    local_rpc1 += "/mapClient_rpc";

    std::string remote_rpc1 = m_map_server;
    remote_rpc1 += "/rpc";

    if (!m_rpcPort_to_Map2D_nws.open(local_rpc1))
    {
        yCError(MAP2D_NWC_YARP, "open() error could not open rpc port %s, check network", local_rpc1.c_str());
        return false;
    }

    bool ok=false;
    ok=Network::connect(local_rpc1, remote_rpc1);
    if (!ok)
    {
        yCError(MAP2D_NWC_YARP, "open() error could not connect to %s", remote_rpc1.c_str());
        return false;
    }

    if (!m_RPC.yarp().attachAsClient(m_rpcPort_to_Map2D_nws))
    {
        yCError(MAP2D_NWC_YARP, "Error! Cannot attach the port as a client");
        return false;
    }

    return true;
}

bool Map2D_nwc_yarp::store_map(const MapGrid2D& map)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.store_mapRPC(map);
}

bool Map2D_nwc_yarp::get_map(std::string map_name, MapGrid2D& map)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_RPC.getMapRPC(map_name);
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to get_map");
        return false;
    }
    map = ret.themap;
    return true;
}

bool Map2D_nwc_yarp::clearAllMaps()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.clearAllMapsRPC();
}

bool Map2D_nwc_yarp::get_map_names(std::vector<std::string>& map_names)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_RPC.getMapNamesRPC();
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to get_map_names");
        return false;
    }
    map_names = ret.map_names;
    return true;
}

bool Map2D_nwc_yarp::remove_map(std::string map_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.remove_mapRPC(map_name);
}

bool Map2D_nwc_yarp::storeLocation(std::string location_name, Map2DLocation loc)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.storeLocationRPC(location_name,loc);
}

bool Map2D_nwc_yarp::storeArea(std::string area_name, Map2DArea area)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.storeAreaRPC(area_name, area);
}

bool Map2D_nwc_yarp::storePath(std::string path_name, Map2DPath path)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.storePathRPC(path_name, path);
}

bool   Map2D_nwc_yarp::getLocationsList(std::vector<std::string>& locations)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_RPC.getLocationsListRPC();
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getLocationsList");
        return false;
    }
    locations = ret.locations;
    return true;
}

bool   Map2D_nwc_yarp::getAreasList(std::vector<std::string>& areas)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_RPC.getAreasListRPC();
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getAreasList");
        return false;
    }
    areas = ret.areas;
    return true;
}

bool   Map2D_nwc_yarp::getPathsList(std::vector<std::string>& paths)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_RPC.getPathsListRPC();
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getPathsList");
        return false;
    }
    paths = ret.paths;
    return true;
}

bool   Map2D_nwc_yarp::getLocation(std::string location_name, Map2DLocation& loc)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_RPC.getLocationRPC(location_name);
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getLocation");
        return false;
    }
    loc = ret.loc;
    return true;
}

bool   Map2D_nwc_yarp::getArea(std::string area_name, Map2DArea& area)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_RPC.getAreaRPC(area_name);
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getArea");
        return false;
    }
    area = ret.area;
    return true;
}

bool   Map2D_nwc_yarp::getPath(std::string path_name, Map2DPath& path)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_RPC.getPathRPC(path_name);
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getPath");
        return false;
    }
    path = ret.path;
    return true;
}

bool   Map2D_nwc_yarp::deleteLocation(std::string location_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.deleteLocationRPC(location_name);
}

bool   Map2D_nwc_yarp::renameLocation(std::string original_name, std::string new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.renameLocationRPC(original_name,new_name);
}

bool   Map2D_nwc_yarp::deleteArea(std::string location_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.deleteAreaRPC(location_name);
}

bool   Map2D_nwc_yarp::deletePath(std::string path_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.deletePathRPC(path_name);
}

bool   Map2D_nwc_yarp::renameArea(std::string original_name, std::string new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.renameAreaRPC(original_name, new_name);
}

bool   Map2D_nwc_yarp::renamePath(std::string original_name, std::string new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.renamePathRPC(original_name, new_name);
}

bool   Map2D_nwc_yarp::clearAllLocations()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.clearAllLocationsRPC();
}

bool   Map2D_nwc_yarp::clearAllAreas()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.clearAllAreasRPC();
}

bool   Map2D_nwc_yarp::clearAllMapsTemporaryFlags()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.clearAllMapsTemporaryFlagsRPC();
}

bool   Map2D_nwc_yarp::clearMapTemporaryFlags(std::string map_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.clearMapTemporaryFlagsRPC(map_name);
}

bool   Map2D_nwc_yarp::clearAllPaths()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.clearAllPathsRPC();
}

bool Map2D_nwc_yarp::close()
{
    return true;
}

bool Map2D_nwc_yarp::saveMapsCollection(std::string maps_collection)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.saveMapsCollectionRPC(maps_collection);
}

bool Map2D_nwc_yarp::loadMapsCollection(std::string maps_collection)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.loadMapsCollectionRPC(maps_collection);
}

bool Map2D_nwc_yarp::saveLocationsAndExtras(std::string locations_collection)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.saveLocationsAndExtrasRPC(locations_collection);
}

bool Map2D_nwc_yarp::loadLocationsAndExtras(std::string locations_collection)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.loadLocationsAndExtrasRPC(locations_collection);
}

bool Map2D_nwc_yarp::saveMapToDisk(std::string map_name, std::string file_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.saveMapToDiskRPC(map_name,file_name);
}

bool Map2D_nwc_yarp::loadMapFromDisk(std::string file_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.loadMapFromDiskRPC(file_name);
}

bool Map2D_nwc_yarp::enableMapsCompression(bool enable)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.enableMapsCompressionRPC(enable);
}
