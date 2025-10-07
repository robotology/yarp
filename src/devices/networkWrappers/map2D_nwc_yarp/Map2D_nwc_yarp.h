/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_MAP2D_NWC_YARP_H
#define YARP_DEV_MAP2D_NWC_YARP_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DArea.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>
#include "IMap2DMsgs.h"

#include "Map2D_nwc_yarp_ParamsParser.h"

/**
 * @ingroup dev_impl_nwc_yarp dev_impl_navigation
 *
 * \section Map2D_nwc_yarp
 *
 * \brief `Map2D_nwc_yarp`: A device which allows a client application to store/retrieve user maps device in a map server.
 *
 * Parameters required by this device are shown in class: Map2D_nwc_yarp_ParamsParser
 *
 */

class Map2D_nwc_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::Nav2D::IMap2D,
        public Map2D_nwc_yarp_ParamsParser
{
protected:
    yarp::os::Port      m_rpcPort_to_Map2D_nws;
    IMap2DMsgs          m_map_RPC;
    std::mutex          m_mutex;

public:

     /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* The following methods belong to IMap2D interface */
    yarp::dev::ReturnValue clearAllMaps  () override;
    yarp::dev::ReturnValue remove_map (std::string map_name) override;
    yarp::dev::ReturnValue store_map  (const yarp::dev::Nav2D::MapGrid2D& map) override;
    yarp::dev::ReturnValue get_map    (std::string map_name, yarp::dev::Nav2D::MapGrid2D& map) override;
    yarp::dev::ReturnValue get_map_names(std::vector<std::string>& map_names) override;

    yarp::dev::ReturnValue storeObject(std::string object_name, yarp::dev::Nav2D::Map2DObject obj) override;
    yarp::dev::ReturnValue storeLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation loc) override;
    yarp::dev::ReturnValue storeArea(std::string area_name, yarp::dev::Nav2D::Map2DArea area) override;
    yarp::dev::ReturnValue storePath(std::string path_name, yarp::dev::Nav2D::Map2DPath path) override;

    yarp::dev::ReturnValue getObject(std::string object_name, yarp::dev::Nav2D::Map2DObject& obj) override;
    yarp::dev::ReturnValue getLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation& loc) override;
    yarp::dev::ReturnValue getArea(std::string location_name, yarp::dev::Nav2D::Map2DArea& area) override;
    yarp::dev::ReturnValue getPath(std::string path_name, yarp::dev::Nav2D::Map2DPath& path) override;

    yarp::dev::ReturnValue renameObject(std::string original_name, std::string new_name) override;
    yarp::dev::ReturnValue renameLocation(std::string original_name, std::string new_name) override;
    yarp::dev::ReturnValue renameArea(std::string original_name, std::string new_name) override;
    yarp::dev::ReturnValue renamePath(std::string original_name, std::string new_name) override;

    yarp::dev::ReturnValue deleteObject(std::string object_name) override;
    yarp::dev::ReturnValue deleteLocation(std::string location_name) override;
    yarp::dev::ReturnValue deleteArea(std::string location_name) override;
    yarp::dev::ReturnValue deletePath(std::string path_name) override;

    yarp::dev::ReturnValue getObjectsList(std::vector<std::string>& objects) override;
    yarp::dev::ReturnValue getLocationsList(std::vector<std::string>& locations) override;
    yarp::dev::ReturnValue getAreasList(std::vector<std::string>& locations) override;
    yarp::dev::ReturnValue getPathsList(std::vector<std::string>& paths) override;

    yarp::dev::ReturnValue getAllObjects(std::vector<yarp::dev::Nav2D::Map2DObject>& objects) override;
    yarp::dev::ReturnValue getAllLocations(std::vector<yarp::dev::Nav2D::Map2DLocation>& locations) override;
    yarp::dev::ReturnValue getAllAreas(std::vector<yarp::dev::Nav2D::Map2DArea>& areas) override;
    yarp::dev::ReturnValue getAllPaths(std::vector<yarp::dev::Nav2D::Map2DPath>& paths) override;

    yarp::dev::ReturnValue clearAllObjects() override;
    yarp::dev::ReturnValue clearAllLocations() override;
    yarp::dev::ReturnValue clearAllAreas() override;
    yarp::dev::ReturnValue clearAllPaths() override;

    yarp::dev::ReturnValue clearAllMapsTemporaryFlags() override;
    yarp::dev::ReturnValue clearMapTemporaryFlags(std::string map_name) override;

    yarp::dev::ReturnValue saveMapsCollection(std::string maps_collection_file) override;
    yarp::dev::ReturnValue loadMapsCollection(std::string maps_collection_file) override;
    yarp::dev::ReturnValue saveLocationsAndExtras(std::string locations_collection_file) override;
    yarp::dev::ReturnValue loadLocationsAndExtras(std::string locations_collection_file) override;
    yarp::dev::ReturnValue saveMapToDisk(std::string map_name, std::string file_name) override;
    yarp::dev::ReturnValue loadMapFromDisk(std::string file_name) override;
    yarp::dev::ReturnValue enableMapsCompression(bool enable) override;
};

#endif // YARP_DEV_MAP2D_NWC_YARP_H
