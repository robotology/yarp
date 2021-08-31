/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/ILocalization2D.h>
#include "Map2DServerImpl.h"

/*! \file ILocalization2DServerImpl.cpp */

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace std;

#define DEFAULT_THREAD_PERIOD 0.01

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {
YARP_LOG_COMPONENT(MAP2DSERVER, "yarp.device.localization2DServer")
}

#define CHECK_POINTER(xxx) {if (xxx==nullptr) {yCError(MAP2DSERVER, "Invalid interface"); return false;}}

bool IMap2DRPCd::clearAllMapsRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->clearAllMaps())
    {
        yCError(MAP2DSERVER, "Unable to clearAllMaps");
        return false;
    }
    return true;
}

bool IMap2DRPCd::store_mapRPC(const yarp::dev::Nav2D::MapGrid2D& themap)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->store_map(themap))
    {
        yCError(MAP2DSERVER, "Unable to store_map");
        return false;
    }
    return true;
}

return_getMap IMap2DRPCd::getMapRPC(const std::string& map_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getMap ret;
    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return ret; }}
    yarp::dev::Nav2D::MapGrid2D map;
    if (!m_iMap->get_map(map_name,map))
    {
        yCError(MAP2DSERVER, "Unable to getEstimatedPoses");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.themap = map;
    }
    return ret;
}

return_getMapNames IMap2DRPCd::getMapNamesRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getMapNames ret;
    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return ret; }}
    std::vector<string> map_names;
    if (!m_iMap->get_map_names(map_names))
    {
        yCError(MAP2DSERVER, "Unable to getEstimatedPoses");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.map_names = map_names;
    }
    return ret;
}

bool IMap2DRPCd::remove_mapRPC(const std::string& map_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->remove_map(map_name))
    {
        yCError(MAP2DSERVER, "Unable to remove_map");
        return false;
    }
    return true;
}

bool IMap2DRPCd::storeLocationRPC(const std::string& location_name, const yarp::dev::Nav2D::Map2DLocation& loc)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->storeLocation(location_name,loc))
    {
        yCError(MAP2DSERVER, "Unable to storeLocation");
        return false;
    }
    return true;
}

bool IMap2DRPCd::storeAreaRPC(const std::string& area_name, const yarp::dev::Nav2D::Map2DArea& area)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->storeArea(area_name,area))
    {
        yCError(MAP2DSERVER, "Unable to storeArea");
        return false;
    }
    return true;
}

bool IMap2DRPCd::storePathRPC(const std::string& path_name, const yarp::dev::Nav2D::Map2DPath& path)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->storePath(path_name,path))
    {
        yCError(MAP2DSERVER, "Unable to storePath");
        return false;
    }
    return true;
}

bool IMap2DRPCd::renameLocationRPC(const std::string& original_name, const std::string& new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->renameLocation(original_name, new_name))
    {
        yCError(MAP2DSERVER, "Unable to renameLocation");
        return false;
    }
    return true;
}

bool IMap2DRPCd::deleteLocationRPC(const std::string& location_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->deleteLocation(location_name))
    {
        yCError(MAP2DSERVER, "Unable to deleteLocation");
        return false;
    }
    return true;
}

bool IMap2DRPCd::deletePathRPC(const std::string& path_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->deletePath(path_name))
    {
        yCError(MAP2DSERVER, "Unable to deletePath");
        return false;
    }
    return true;
}

bool IMap2DRPCd::renameAreaRPC(const std::string& original_name, const std::string& new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->renameArea(original_name, new_name))
    {
        yCError(MAP2DSERVER, "Unable to renameArea");
        return false;
    }
    return true;
}

bool IMap2DRPCd::renamePathRPC(const std::string& original_name, const std::string& new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->renamePath(original_name, new_name))
    {
        yCError(MAP2DSERVER, "Unable to renamePath");
        return false;
    }
    return true;
}

bool IMap2DRPCd::deleteAreaRPC(const std::string& area_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->deleteArea(area_name))
    {
        yCError(MAP2DSERVER, "Unable to deleteArea");
        return false;
    }
    return true;
}

bool IMap2DRPCd::clearAllLocationsRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->clearAllLocations())
    {
        yCError(MAP2DSERVER, "Unable to clearAllLocations");
        return false;
    }
    return true;
}

bool IMap2DRPCd::clearAllAreasRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->clearAllAreas())
    {
        yCError(MAP2DSERVER, "Unable to storePath");
        return false;
    }
    return true;
}

bool IMap2DRPCd::clearAllPathsRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->clearAllPaths())
    {
        yCError(MAP2DSERVER, "Unable to clearAllPaths");
        return false;
    }
    return true;
}

bool IMap2DRPCd::clearAllMapsTemporaryFlagsRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->clearAllMapsTemporaryFlags())
    {
        yCError(MAP2DSERVER, "Unable to clearAllMapsTemporaryFlags");
        return false;
    }
    return true;
}

bool IMap2DRPCd::clearMapTemporaryFlagsRPC(const std::string& map_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->clearMapTemporaryFlags(map_name))
    {
        yCError(MAP2DSERVER, "Unable to clearMapTemporaryFlags");
        return false;
    }
    return true;
}

bool IMap2DRPCd::saveMapsCollectionRPC(const std::string& maps_collection_file)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->saveMapsCollection(maps_collection_file))
    {
        yCError(MAP2DSERVER, "Unable to saveMapsCollection");
        return false;
    }
    return true;
}

bool IMap2DRPCd::loadMapsCollectionRPC(const std::string& maps_collection_file)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->loadMapsCollection(maps_collection_file))
    {
        yCError(MAP2DSERVER, "Unable to loadMapsCollection");
        return false;
    }
    return true;
}

bool IMap2DRPCd::saveLocationsAndExtrasRPC(const std::string& locations_collection_file)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->saveLocationsAndExtras(locations_collection_file))
    {
        yCError(MAP2DSERVER, "Unable to saveLocationsAndExtras");
        return false;
    }
    return true;
}

bool IMap2DRPCd::loadLocationsAndExtrasRPC(const std::string& locations_collection_file)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return false; }}

    if (!m_iMap->loadLocationsAndExtras(locations_collection_file))
    {
        yCError(MAP2DSERVER, "Unable to loadLocationsAndExtras");
        return false;
    }
    return true;
}

return_getLocation IMap2DRPCd::getLocationRPC(const std::string& location_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getLocation ret;
    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return ret; }}
    yarp::dev::Nav2D::Map2DLocation loc;
    if (!m_iMap->getLocation(location_name, loc))
    {
        yCError(MAP2DSERVER, "Unable to getLocation");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.loc = loc;
    }
    return ret;
}

return_getArea IMap2DRPCd::getAreaRPC(const std::string& area_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getArea ret;
    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return ret; }}
    yarp::dev::Nav2D::Map2DArea area;
    if (!m_iMap->getArea(area_name,area))
    {
        yCError(MAP2DSERVER, "Unable to getArea");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.area = area;
    }
    return ret;
}

return_getPath IMap2DRPCd::getPathRPC(const std::string& path_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getPath ret;
    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return ret; }}
    yarp::dev::Nav2D::Map2DPath path;
    if (!m_iMap->getPath(path_name, path))
    {
        yCError(MAP2DSERVER, "Unable to getPath");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.path = path;
    }
    return ret;
}

return_getLocationsList IMap2DRPCd::getLocationsListRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getLocationsList ret;
    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return ret; }}
    std::vector<string> loc_names;
    if (!m_iMap->getLocationsList(loc_names))
    {
        yCError(MAP2DSERVER, "Unable to getLocationsList");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.locations = loc_names;
    }
    return ret;
}

return_getAreasList IMap2DRPCd::getAreasListRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getAreasList ret;
    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return ret; }}
    std::vector<string> area_names;
    if (!m_iMap->getAreasList(area_names))
    {
        yCError(MAP2DSERVER, "Unable to getAreasList");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.areas = area_names;
    }
    return ret;
}

return_getPathsList IMap2DRPCd::getPathsListRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getPathsList ret;
    {if (m_iMap == nullptr) { yCError(MAP2DSERVER, "Invalid interface"); return ret; }}
    std::vector<string> path_names;
    if (!m_iMap->getPathsList(path_names))
    {
        yCError(MAP2DSERVER, "Unable to getPathsList");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.paths = path_names;
    }
    return ret;
}
