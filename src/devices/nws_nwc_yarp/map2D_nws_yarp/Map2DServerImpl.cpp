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

bool IMap2DRPCd::clear_all_maps_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->clearAllMaps())
    {
        yCError(MAP2D_RPC, "Unable to clearAllMaps");
        return false;
    }
    return true;
}

bool IMap2DRPCd::store_map_RPC(const yarp::dev::Nav2D::MapGrid2D& themap)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->store_map(themap))
    {
        yCError(MAP2D_RPC, "Unable to store_map");
        return false;
    }
    return true;
}

return_get_map IMap2DRPCd::get_map_RPC(const std::string& map_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_map ret;
    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return ret; }}
    yarp::dev::Nav2D::MapGrid2D map;
    if (!m_iMap->get_map(map_name,map))
    {
        yCError(MAP2D_RPC, "Unable to getEstimatedPoses");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.themap = map;
    }
    return ret;
}

return_get_map_names IMap2DRPCd::get_map_names_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_map_names ret;
    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return ret; }}
    std::vector<string> map_names;
    if (!m_iMap->get_map_names(map_names))
    {
        yCError(MAP2D_RPC, "Unable to getEstimatedPoses");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.map_names = map_names;
    }
    return ret;
}

bool IMap2DRPCd::remove_map_RPC(const std::string& map_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->remove_map(map_name))
    {
        yCError(MAP2D_RPC, "Unable to remove_map");
        return false;
    }
    return true;
}

bool IMap2DRPCd::store_location_RPC(const std::string& location_name, const yarp::dev::Nav2D::Map2DLocation& loc)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->storeLocation(location_name,loc))
    {
        yCError(MAP2D_RPC, "Unable to storeLocation");
        return false;
    }
    return true;
}

bool IMap2DRPCd::store_area_RPC(const std::string& area_name, const yarp::dev::Nav2D::Map2DArea& area)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->storeArea(area_name,area))
    {
        yCError(MAP2D_RPC, "Unable to storeArea");
        return false;
    }
    return true;
}

bool IMap2DRPCd::store_path_RPC(const std::string& path_name, const yarp::dev::Nav2D::Map2DPath& path)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->storePath(path_name,path))
    {
        yCError(MAP2D_RPC, "Unable to storePath");
        return false;
    }
    return true;
}

bool IMap2DRPCd::rename_location_RPC(const std::string& original_name, const std::string& new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->renameLocation(original_name, new_name))
    {
        yCError(MAP2D_RPC, "Unable to renameLocation");
        return false;
    }
    return true;
}

bool IMap2DRPCd::delete_location_RPC(const std::string& location_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->deleteLocation(location_name))
    {
        yCError(MAP2D_RPC, "Unable to deleteLocation");
        return false;
    }
    return true;
}

bool IMap2DRPCd::delete_path_RPC(const std::string& path_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->deletePath(path_name))
    {
        yCError(MAP2D_RPC, "Unable to deletePath");
        return false;
    }
    return true;
}

bool IMap2DRPCd::rename_area_RPC(const std::string& original_name, const std::string& new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->renameArea(original_name, new_name))
    {
        yCError(MAP2D_RPC, "Unable to renameArea");
        return false;
    }
    return true;
}

bool IMap2DRPCd::rename_path_RPC(const std::string& original_name, const std::string& new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->renamePath(original_name, new_name))
    {
        yCError(MAP2D_RPC, "Unable to renamePath");
        return false;
    }
    return true;
}

bool IMap2DRPCd::delete_area_RPC(const std::string& area_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->deleteArea(area_name))
    {
        yCError(MAP2D_RPC, "Unable to deleteArea");
        return false;
    }
    return true;
}

bool IMap2DRPCd::clear_all_locations_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->clearAllLocations())
    {
        yCError(MAP2D_RPC, "Unable to clearAllLocations");
        return false;
    }
    return true;
}

bool IMap2DRPCd::clear_all_areas_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->clearAllAreas())
    {
        yCError(MAP2D_RPC, "Unable to storePath");
        return false;
    }
    return true;
}

bool IMap2DRPCd::clear_all_paths_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->clearAllPaths())
    {
        yCError(MAP2D_RPC, "Unable to clearAllPaths");
        return false;
    }
    return true;
}

bool IMap2DRPCd::clear_all_maps_temporary_flags_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->clearAllMapsTemporaryFlags())
    {
        yCError(MAP2D_RPC, "Unable to clearAllMapsTemporaryFlags");
        return false;
    }
    return true;
}

bool IMap2DRPCd::clear_map_temporary_flags_RPC(const std::string& map_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->clearMapTemporaryFlags(map_name))
    {
        yCError(MAP2D_RPC, "Unable to clearMapTemporaryFlags");
        return false;
    }
    return true;
}

bool IMap2DRPCd::save_maps_collection_RPC(const std::string& maps_collection_file)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->saveMapsCollection(maps_collection_file))
    {
        yCError(MAP2D_RPC, "Unable to saveMapsCollection");
        return false;
    }
    return true;
}

bool IMap2DRPCd::load_maps_collection_RPC(const std::string& maps_collection_file)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->loadMapsCollection(maps_collection_file))
    {
        yCError(MAP2D_RPC, "Unable to loadMapsCollection");
        return false;
    }
    return true;
}

bool IMap2DRPCd::save_locations_and_extras_RPC(const std::string& locations_collection_file)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->saveLocationsAndExtras(locations_collection_file))
    {
        yCError(MAP2D_RPC, "Unable to saveLocationsAndExtras");
        return false;
    }
    return true;
}

bool IMap2DRPCd::load_locations_and_extras_RPC(const std::string& locations_collection_file)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return false; }}

    if (!m_iMap->loadLocationsAndExtras(locations_collection_file))
    {
        yCError(MAP2D_RPC, "Unable to loadLocationsAndExtras");
        return false;
    }
    return true;
}

return_get_location IMap2DRPCd::get_location_RPC(const std::string& location_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_location ret;
    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return ret; }}
    yarp::dev::Nav2D::Map2DLocation loc;
    if (!m_iMap->getLocation(location_name, loc))
    {
        yCError(MAP2D_RPC, "Unable to getLocation");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
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
    if (!m_iMap->getArea(area_name,area))
    {
        yCError(MAP2D_RPC, "Unable to getArea");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.area = area;
    }
    return ret;
}

return_get_path IMap2DRPCd::get_path_RPC(const std::string& path_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_path ret;
    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return ret; }}
    yarp::dev::Nav2D::Map2DPath path;
    if (!m_iMap->getPath(path_name, path))
    {
        yCError(MAP2D_RPC, "Unable to getPath");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.path = path;
    }
    return ret;
}

return_get_locations_list IMap2DRPCd::get_locations_list_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_locations_list ret;
    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return ret; }}
    std::vector<string> loc_names;
    if (!m_iMap->getLocationsList(loc_names))
    {
        yCError(MAP2D_RPC, "Unable to getLocationsList");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.locations = loc_names;
    }
    return ret;
}

return_get_areas_list IMap2DRPCd::get_areas_list_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_areas_list ret;
    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return ret; }}
    std::vector<string> area_names;
    if (!m_iMap->getAreasList(area_names))
    {
        yCError(MAP2D_RPC, "Unable to getAreasList");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.areas = area_names;
    }
    return ret;
}

return_get_paths_list IMap2DRPCd::get_paths_list_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_get_paths_list ret;
    {if (m_iMap == nullptr) { yCError(MAP2D_RPC, "Invalid interface"); return ret; }}
    std::vector<string> path_names;
    if (!m_iMap->getPathsList(path_names))
    {
        yCError(MAP2D_RPC, "Unable to getPathsList");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.paths = path_names;
    }
    return ret;
}
