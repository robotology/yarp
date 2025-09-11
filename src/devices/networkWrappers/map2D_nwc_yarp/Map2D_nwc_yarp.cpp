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
    if (!parseParams(config)) { return false; }

    std::string local_rpc1 = m_local;
    local_rpc1 += "/mapClient_rpc";

    std::string remote_rpc1 = m_remote;
    remote_rpc1 += "/rpc";

    if (!m_rpcPort_to_Map2D_nws.open(local_rpc1))
    {
        yCError(MAP2D_NWC_YARP, "open() error could not open rpc port %s, check network", local_rpc1.c_str());
        return false;
    }

    bool ok=false;
    ok=Network::connect(local_rpc1, remote_rpc1, m_carrier);
    if (!ok)
    {
        yCError(MAP2D_NWC_YARP, "open() error could not connect to %s", remote_rpc1.c_str());
        return false;
    }

    if (!m_map_RPC.yarp().attachAsClient(m_rpcPort_to_Map2D_nws))
    {
        yCError(MAP2D_NWC_YARP, "Error! Cannot attach the port as a client");
        return false;
    }

    return true;
}

ReturnValue Map2D_nwc_yarp::store_map(const MapGrid2D& map)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.store_map_RPC(map);
}

ReturnValue Map2D_nwc_yarp::get_map(std::string map_name, MapGrid2D& map)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_map_RPC.get_map_RPC(map_name);
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to get_map");
        return ret.retval;
    }
    map = ret.themap;
    return ret.retval;
}

ReturnValue Map2D_nwc_yarp::clearAllMaps()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.clear_all_maps_RPC();
}

ReturnValue Map2D_nwc_yarp::get_map_names(std::vector<std::string>& map_names)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_map_RPC.get_map_names_RPC();
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to get_map_names");
        return ret.retval;
    }
    map_names = ret.map_names;
    return ret.retval;
}

ReturnValue Map2D_nwc_yarp::remove_map(std::string map_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.remove_map_RPC(map_name);
}

ReturnValue Map2D_nwc_yarp::storeLocation(std::string location_name, Map2DLocation loc)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.store_location_RPC(location_name,loc);
}

ReturnValue Map2D_nwc_yarp::storeArea(std::string area_name, Map2DArea area)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.store_area_RPC(area_name, area);
}

ReturnValue Map2D_nwc_yarp::storePath(std::string path_name, Map2DPath path)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.store_path_RPC(path_name, path);
}

ReturnValue   Map2D_nwc_yarp::getLocationsList(std::vector<std::string>& locations)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_map_RPC.get_locations_list_RPC();
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getLocationsList");
        return ret.retval;
    }
    locations = ret.locations;
    return ret.retval;
}

ReturnValue   Map2D_nwc_yarp::getAreasList(std::vector<std::string>& areas)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_map_RPC.get_areas_list_RPC();
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getAreasList");
        return ret.retval;
    }
    areas = ret.areas;
    return ret.retval;
}

ReturnValue   Map2D_nwc_yarp::getPathsList(std::vector<std::string>& paths)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_map_RPC.get_paths_list_RPC();
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getPathsList");
        return ret.retval;
    }
    paths = ret.paths;
    return ret.retval;
}

ReturnValue   Map2D_nwc_yarp::getAllLocations(std::vector<yarp::dev::Nav2D::Map2DLocation>& locations)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_map_RPC.get_all_locations_RPC();
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getAllLocations");
        return ret.retval;
    }
    locations = ret.locations;
    return ret.retval;
}

ReturnValue   Map2D_nwc_yarp::getAllAreas(std::vector<yarp::dev::Nav2D::Map2DArea>& areas)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_map_RPC.get_all_areas_RPC();
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getAllAreas");
        return ret.retval;
    }
    areas = ret.areas;
    return ret.retval;
}

ReturnValue   Map2D_nwc_yarp::getAllPaths(std::vector<yarp::dev::Nav2D::Map2DPath>& paths)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_map_RPC.get_all_paths_RPC();
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getAllPaths");
        return ret.retval;
    }
    paths = ret.paths;
    return ret.retval;
}

ReturnValue   Map2D_nwc_yarp::getLocation(std::string location_name, Map2DLocation& loc)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_map_RPC.get_location_RPC(location_name);
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getLocation");
        return ret.retval;
    }
    loc = ret.loc;
    return ret.retval;
}

ReturnValue   Map2D_nwc_yarp::getArea(std::string area_name, Map2DArea& area)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_map_RPC.get_area_RPC(area_name);
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getArea");
        return ret.retval;
    }
    area = ret.area;
    return ret.retval;
}

ReturnValue   Map2D_nwc_yarp::getPath(std::string path_name, Map2DPath& path)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_map_RPC.get_path_RPC(path_name);
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getPath");
        return ret.retval;
    }
    path = ret.path;
    return ret.retval;
}

ReturnValue   Map2D_nwc_yarp::deleteLocation(std::string location_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.delete_location_RPC(location_name);
}

ReturnValue   Map2D_nwc_yarp::renameLocation(std::string original_name, std::string new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.rename_location_RPC(original_name,new_name);
}

ReturnValue   Map2D_nwc_yarp::deleteArea(std::string location_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.delete_area_RPC(location_name);
}

ReturnValue   Map2D_nwc_yarp::deletePath(std::string path_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.delete_path_RPC(path_name);
}

ReturnValue   Map2D_nwc_yarp::renameArea(std::string original_name, std::string new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.rename_area_RPC(original_name, new_name);
}

ReturnValue   Map2D_nwc_yarp::renamePath(std::string original_name, std::string new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.rename_path_RPC(original_name, new_name);
}

ReturnValue   Map2D_nwc_yarp::clearAllLocations()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.clear_all_locations_RPC();
}

ReturnValue   Map2D_nwc_yarp::clearAllAreas()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.clear_all_areas_RPC();
}

ReturnValue   Map2D_nwc_yarp::clearAllMapsTemporaryFlags()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.clear_all_maps_temporary_flags_RPC();
}

ReturnValue   Map2D_nwc_yarp::clearMapTemporaryFlags(std::string map_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.clear_map_temporary_flags_RPC(map_name);
}

ReturnValue   Map2D_nwc_yarp::clearAllPaths()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.clear_all_paths_RPC();
}

bool Map2D_nwc_yarp::close()
{
    return true;
}

ReturnValue Map2D_nwc_yarp::saveMapsCollection(std::string maps_collection)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.save_maps_collection_RPC(maps_collection);
}

ReturnValue Map2D_nwc_yarp::loadMapsCollection(std::string maps_collection)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.load_maps_collection_RPC(maps_collection);
}

ReturnValue Map2D_nwc_yarp::saveLocationsAndExtras(std::string locations_collection)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.save_locations_and_extras_RPC(locations_collection);
}

ReturnValue Map2D_nwc_yarp::loadLocationsAndExtras(std::string locations_collection)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.load_locations_and_extras_RPC(locations_collection);
}

ReturnValue Map2D_nwc_yarp::saveMapToDisk(std::string map_name, std::string file_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.save_map_to_disk_RPC(map_name,file_name);
}

ReturnValue Map2D_nwc_yarp::loadMapFromDisk(std::string file_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.load_map_from_disk_RPC(file_name);
}

ReturnValue Map2D_nwc_yarp::enableMapsCompression(bool enable)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.enable_maps_compression_RPC(enable);
}

ReturnValue Map2D_nwc_yarp::storeObject(std::string object_name, Map2DObject obj)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.store_object_RPC(object_name, obj);
}

ReturnValue Map2D_nwc_yarp::getObject(std::string object_name, Map2DObject& obj)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_map_RPC.get_object_RPC(object_name);
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getObject");
        return ret.retval;
    }
    obj = ret.obj;
    return ret.retval;
}

ReturnValue Map2D_nwc_yarp::getObjectsList(std::vector<std::string>& objects)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_map_RPC.get_objects_list_RPC();
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getObjectsList");
        return ret.retval;
    }
    objects = ret.objects;
    return ret.retval;
}

ReturnValue Map2D_nwc_yarp::getAllObjects(std::vector<yarp::dev::Nav2D::Map2DObject>& objects)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_map_RPC.get_all_objects_RPC();
    if (!ret.retval)
    {
        yCError(MAP2D_NWC_YARP, "Unable to getAllObjects");
        return ret.retval;
    }
    objects = ret.objects;
    return ret.retval;
}

ReturnValue Map2D_nwc_yarp::renameObject(std::string original_name, std::string new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.rename_object_RPC(original_name, new_name);
}

ReturnValue Map2D_nwc_yarp::deleteObject(std::string object_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.delete_object_RPC(object_name);
}

ReturnValue Map2D_nwc_yarp::clearAllObjects()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_map_RPC.clear_all_objects_RPC();
}
