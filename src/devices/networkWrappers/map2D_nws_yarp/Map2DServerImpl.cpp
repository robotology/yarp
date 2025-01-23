/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/ILocalization2D.h>
#include "Map2DServerImpl.h"

/*! \file Map2DServerImpl.cpp */

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace std;

#define DEFAULT_THREAD_PERIOD 0.01

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {
YARP_LOG_COMPONENT(MAP2D_RPC, "yarp.device.map2D_nws_yarp.IMap2DRPCd")
}

#define CHECK_POINTER(xxx) {if (xxx==nullptr) {yCError(MAP2D_RPC, "Invalid interface"); return false;}}

ReturnValue IMap2DRPCd::clear_all_maps_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->clearAllMaps();
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to clearAllMaps");
    }
    return ret;
}

ReturnValue IMap2DRPCd::store_map_RPC(const yarp::dev::Nav2D::MapGrid2D& themap)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->store_map(themap);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to store_map");
    }
    return ret;
}

return_get_map IMap2DRPCd::get_map_RPC(const std::string& map_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_map ret;
    yarp::dev::Nav2D::MapGrid2D map;
    ret.retval = m_iMap->get_map(map_name, map);
    if (!ret.retval)
    {
        yCError(MAP2D_RPC, "Unable to getEstimatedPoses");
    }
    else
    {
        ret.themap = map;
    }
    return ret;
}

return_get_map_names IMap2DRPCd::get_map_names_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_map_names ret;
    std::vector<string> map_names;
    ret.retval = m_iMap->get_map_names(map_names);
    if (!ret.retval)
    {
        yCError(MAP2D_RPC, "Unable to getEstimatedPoses");
    }
    else
    {
        ret.map_names = map_names;
    }
    return ret;
}

ReturnValue IMap2DRPCd::remove_map_RPC(const std::string& map_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->remove_map(map_name);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to remove_map");
    }
    return ret;
}

ReturnValue IMap2DRPCd::store_location_RPC(const std::string& location_name, const yarp::dev::Nav2D::Map2DLocation& loc)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->storeLocation(location_name, loc);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to storeLocation");
    }
    return ret;
}

ReturnValue IMap2DRPCd::store_area_RPC(const std::string& area_name, const yarp::dev::Nav2D::Map2DArea& area)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->storeArea(area_name, area);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to storeArea");
    }
    return ret;
}

ReturnValue IMap2DRPCd::store_path_RPC(const std::string& path_name, const yarp::dev::Nav2D::Map2DPath& path)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->storePath(path_name, path);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to storePath");
    }
    return ret;
}

ReturnValue IMap2DRPCd::rename_location_RPC(const std::string& original_name, const std::string& new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->renameLocation(original_name, new_name);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to renameLocation");
    }
    return ret;
}

ReturnValue IMap2DRPCd::delete_location_RPC(const std::string& location_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->deleteLocation(location_name);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to deleteLocation");
    }
    return ret;
}

ReturnValue IMap2DRPCd::delete_path_RPC(const std::string& path_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->deletePath(path_name);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to deletePath");
    }
    return ret;
}

ReturnValue IMap2DRPCd::rename_area_RPC(const std::string& original_name, const std::string& new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->renameArea(original_name, new_name);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to renameArea");
    }
    return ret;
}

ReturnValue IMap2DRPCd::rename_path_RPC(const std::string& original_name, const std::string& new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->renamePath(original_name, new_name);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to renamePath");
    }
    return ret;
}

ReturnValue IMap2DRPCd::delete_area_RPC(const std::string& area_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->deleteArea(area_name);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to deleteArea");
    }
    return ret;
}

ReturnValue IMap2DRPCd::clear_all_locations_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->clearAllLocations();
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to clearAllLocations");
    }
    return ret;
}

ReturnValue IMap2DRPCd::clear_all_areas_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->clearAllAreas();
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to storePath");
    }
    return ret;
}

ReturnValue IMap2DRPCd::clear_all_paths_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->clearAllPaths();
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to clearAllPaths");
    }
    return ret;
}

ReturnValue IMap2DRPCd::clear_all_maps_temporary_flags_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->clearAllMapsTemporaryFlags();
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to clearAllMapsTemporaryFlags");
    }
    return ret;
}

ReturnValue IMap2DRPCd::clear_map_temporary_flags_RPC(const std::string& map_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->clearMapTemporaryFlags(map_name);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to clearMapTemporaryFlags");
    }
    return ret;
}

ReturnValue IMap2DRPCd::save_maps_collection_RPC(const std::string& maps_collection_file)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->saveMapsCollection(maps_collection_file);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to saveMapsCollection");
    }
    return ret;
}

ReturnValue IMap2DRPCd::load_maps_collection_RPC(const std::string& maps_collection_file)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->loadMapsCollection(maps_collection_file);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to loadMapsCollection");
    }
    return ret;
}

ReturnValue IMap2DRPCd::save_locations_and_extras_RPC(const std::string& locations_collection_file)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->saveLocationsAndExtras(locations_collection_file);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to saveLocationsAndExtras");
    }
    return ret;
}

ReturnValue IMap2DRPCd::load_locations_and_extras_RPC(const std::string& locations_collection_file)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iMap->loadLocationsAndExtras(locations_collection_file);
    if (!ret)
    {
        yCError(MAP2D_RPC, "Unable to loadLocationsAndExtras");
    }
    return ret;
}

return_get_location IMap2DRPCd::get_location_RPC(const std::string& location_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_location ret;
    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return ret; }}
    yarp::dev::Nav2D::Map2DLocation loc;
    ret.retval = m_iMap->getLocation(location_name, loc);
    if (!ret.retval)
    {
        yCError(MAP2D_RPC, "Unable to getLocation");
    }
    else
    {
        ret.loc = loc;
    }
    return ret;
}

return_get_area IMap2DRPCd::get_area_RPC(const std::string& area_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_area ret;
    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return ret; }}
    yarp::dev::Nav2D::Map2DArea area;
    ret.retval = m_iMap->getArea(area_name, area);
    if (!ret.retval)
    {
        yCError(MAP2D_RPC, "Unable to getArea");
    }
    else
    {
        ret.area = area;
    }
    return ret;
}

return_get_path IMap2DRPCd::get_path_RPC(const std::string& path_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_path ret;
    yarp::dev::Nav2D::Map2DPath path;
    ret.retval = m_iMap->getPath(path_name, path);
    if (!ret.retval)
    {
        yCError(MAP2D_RPC, "Unable to getPath");
    }
    else
    {
        ret.path = path;
    }
    return ret;
}

return_get_locations_list IMap2DRPCd::get_locations_list_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_locations_list ret;
    std::vector<string> loc_names;
    ret.retval = m_iMap->getLocationsList(loc_names);
    if (!ret.retval)
    {
        yCError(MAP2D_RPC, "Unable to getLocationsList");
    }
    else
    {
        ret.locations = loc_names;
    }
    return ret;
}

return_get_areas_list IMap2DRPCd::get_areas_list_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_areas_list ret;
    std::vector<string> area_names;
    ret.retval = m_iMap->getAreasList(area_names);
    if (!ret.retval)
    {
        yCError(MAP2D_RPC, "Unable to getAreasList");
    }
    else
    {
        ret.areas = area_names;
    }
    return ret;
}

return_get_paths_list IMap2DRPCd::get_paths_list_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_paths_list ret;
    std::vector<string> path_names;
    ret.retval = m_iMap->getPathsList(path_names);
    if (!ret.retval)
    {
        yCError(MAP2D_RPC, "Unable to getPathsList");
    }
    else
    {
        ret.paths = path_names;
    }
    return ret;
}
