/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_MAP2DSERVER_H
#define YARP_DEV_MAP2DSERVER_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <mutex>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/RpcServer.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DArea.h>
#include <yarp/dev/Map2DPath.h>
#include <yarp/os/ResourceFinder.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/api.h>

#include "Map2DStorage_ParamsParser.h"

#define DEFAULT_THREAD_PERIOD 20 //ms

/**
 *  @ingroup dev_impl_navigation
 *
 * \section Map2DStorage
 *
 * \brief `Map2DStorage`: A device capable of read/save collections of maps from disk, and make them accessible to any Map2DClient device.
 *
 * Parameters required by this device are shown in class: Map2DStorage_ParamsParser
 */

class Map2DStorage :
        public yarp::dev::DeviceDriver,
        public yarp::os::PortReader,
        public yarp::dev::Nav2D::IMap2D,
        public Map2DStorage_ParamsParser
{
private:
    std::map<std::string, yarp::dev::Nav2D::MapGrid2D>     m_maps_storage;
    std::map<std::string, yarp::dev::Nav2D::Map2DLocation> m_locations_storage;
    std::map<std::string, yarp::dev::Nav2D::Map2DObject>   m_objects_storage;
    std::map<std::string, yarp::dev::Nav2D::Map2DPath>     m_paths_storage;
    std::map<std::string, yarp::dev::Nav2D::Map2DArea>     m_areas_storage;

public:
    Map2DStorage();
    ~Map2DStorage();
    bool open(yarp::os::Searchable& params) override;
    bool close() override;

public:
    //IMap2D methods
    yarp::dev::ReturnValue clearAllMaps() override;
    yarp::dev::ReturnValue store_map(const yarp::dev::Nav2D::MapGrid2D & map) override;
    yarp::dev::ReturnValue get_map(std::string map_name, yarp::dev::Nav2D::MapGrid2D & map) override;
    yarp::dev::ReturnValue get_map_names(std::vector<std::string> & map_names) override;
    yarp::dev::ReturnValue remove_map(std::string map_name) override;
    yarp::dev::ReturnValue storeObject(std::string object_name, yarp::dev::Nav2D::Map2DObject obj) override;
    yarp::dev::ReturnValue storeLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation loc) override;
    yarp::dev::ReturnValue storeArea(std::string area_name, yarp::dev::Nav2D::Map2DArea area) override;
    yarp::dev::ReturnValue storePath(std::string path_name, yarp::dev::Nav2D::Map2DPath path) override;
    yarp::dev::ReturnValue getObject(std::string object_name, yarp::dev::Nav2D::Map2DObject& obj) override;
    yarp::dev::ReturnValue getLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation & loc) override;
    yarp::dev::ReturnValue getArea(std::string area_name, yarp::dev::Nav2D::Map2DArea & area) override;
    yarp::dev::ReturnValue getPath(std::string path_name, yarp::dev::Nav2D::Map2DPath & path) override;
    yarp::dev::ReturnValue getObjectsList(std::vector<std::string> & locations) override;
    yarp::dev::ReturnValue getLocationsList(std::vector<std::string> & locations) override;
    yarp::dev::ReturnValue getAreasList(std::vector<std::string> & areas) override;
    yarp::dev::ReturnValue getPathsList(std::vector<std::string> & paths) override;
    yarp::dev::ReturnValue getAllObjects(std::vector<yarp::dev::Nav2D::Map2DObject>& obj) override;
    yarp::dev::ReturnValue getAllLocations(std::vector<yarp::dev::Nav2D::Map2DLocation>& locations) override;
    yarp::dev::ReturnValue getAllAreas(std::vector<yarp::dev::Nav2D::Map2DArea>& areas) override;
    yarp::dev::ReturnValue getAllPaths(std::vector<yarp::dev::Nav2D::Map2DPath>& paths) override;
    yarp::dev::ReturnValue renameObject(std::string original_name, std::string new_name) override;
    yarp::dev::ReturnValue renameLocation(std::string original_name, std::string new_name) override;
    yarp::dev::ReturnValue deleteObject(std::string object_name) override;
    yarp::dev::ReturnValue deleteLocation(std::string location_name) override;
    yarp::dev::ReturnValue deletePath(std::string path_name) override;
    yarp::dev::ReturnValue renameArea(std::string original_name, std::string new_name) override;
    yarp::dev::ReturnValue renamePath(std::string original_name, std::string new_name) override;
    yarp::dev::ReturnValue deleteArea(std::string area_name) override;
    yarp::dev::ReturnValue clearAllObjects() override;
    yarp::dev::ReturnValue clearAllLocations() override;
    yarp::dev::ReturnValue clearAllAreas() override;
    yarp::dev::ReturnValue clearAllPaths() override;
    yarp::dev::ReturnValue clearAllMapsTemporaryFlags() override;
    yarp::dev::ReturnValue clearMapTemporaryFlags(std::string map_name) override;
    yarp::dev::ReturnValue saveMapToDisk(std::string map_name, std::string file_name) override;
    yarp::dev::ReturnValue loadMapFromDisk(std::string file_name) override;
    yarp::dev::ReturnValue saveMapsCollection(std::string maps_collection_file) override;
    yarp::dev::ReturnValue loadMapsCollection(std::string maps_collection_file) override;
    yarp::dev::ReturnValue reloadMapsCollection() override;
    yarp::dev::ReturnValue loadLocationsAndExtras(std::string locations_file) override;
    yarp::dev::ReturnValue saveLocationsAndExtras(std::string locations_file) override;
    yarp::dev::ReturnValue reloadLocationsAndExtras() override;
    yarp::dev::ReturnValue enableMapsCompression(bool enable) override;


private:
    bool priv_load_locations_and_areas_v1(std::ifstream& file);
    bool priv_load_locations_and_areas_v2(std::ifstream& file);
    bool priv_load_locations_and_areas_v3(std::ifstream& file);
    bool priv_load_locations_and_areas_v4(std::ifstream& file);

private:
    yarp::os::ResourceFinder     m_rf_mapCollection;
    std::mutex                   m_mutex;
    yarp::os::RpcServer          m_rpcPort;
    std::string                  m_collection_startup_file_with_path;
    std::string                  m_locations_startup_file_with_path;

    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_DEV_MAP2DSTORAGE_H
