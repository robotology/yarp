/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMAP2D_H
#define YARP_DEV_IMAP2D_H

#include <yarp/os/Vocab.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DArea.h>
#include <yarp/dev/Map2DObject.h>
#include <yarp/dev/Map2DPath.h>
#include <vector>
#include <string>

namespace yarp::dev::Nav2D {
class IMap2D;
}

/**
 * @ingroup dev_iface_navigation
 *
 * IMap2D Interface. Provides methods to store/retrieve map grids and locations to/from a map server.
 */
class YARP_dev_API yarp::dev::Nav2D::IMap2D
{
public:
    /**
     * Destructor.
     */
    virtual ~IMap2D();

    /**
     Removes all the registered maps from the server.
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue     clearAllMaps () = 0;

    /**
    Stores a map into the map server.
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue     store_map(const yarp::dev::Nav2D::MapGrid2D& map) = 0;

    /**
    Gets a map from the map server.
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue     get_map(std::string map_name, yarp::dev::Nav2D::MapGrid2D& map) = 0;

    /**
    Gets a list containing the names of all registered maps.
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue     get_map_names(std::vector<std::string>& map_names) = 0;

    /**
    Removes a map from the map server.
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue     remove_map(std::string map_name) = 0;

    /**
    * Store a object specified by the user in the world reference frame
    * @param object_name the name of the location
    * @param obj the location of the object
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue storeObject(std::string object_name, yarp::dev::Nav2D::Map2DObject obj) = 0;

    /**
    * Store a location specified by the user in the world reference frame
    * @param location_name the name of the location
    * @param loc the location of the location
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue storeLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation loc) = 0;

    /**
    * Store an area
    * @param area_name the name of the area
    * @param area the area
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue storeArea(std::string area_name, yarp::dev::Nav2D::Map2DArea area) = 0;

    /**
    * Store a path
    * @param path_name the name of the path
    * @param path the path
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue storePath(std::string path_name, yarp::dev::Nav2D::Map2DPath path) = 0;

    /**
    * Retrieves a object specified by the user in the world reference frame
    * @param object_name the name of the location
    * @param obj the location
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue getObject(std::string object_name, yarp::dev::Nav2D::Map2DObject& obj) = 0;

    /**
    * Retrieves a location specified by the user in the world reference frame
    * @param location_name the name of the location
    * @param loc the location
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue getLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation& loc) = 0;

    /**
    * Retrieves an area
    * @param area_name the name of the area
    * @param area the area
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue getArea(std::string area_name, yarp::dev::Nav2D::Map2DArea& area) = 0;

    /**
    * Retrieves a path
    * @param path_name the name of the path
    * @param path the path
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue getPath(std::string path_name, yarp::dev::Nav2D::Map2DPath& path) = 0;

    /**
    * Get a list of the names of all stored objects
    * @param the returned list of objects names
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue getObjectsList(std::vector<std::string> & objects) = 0;

    /**
    * Get a list of the names of all stored locations
    * @param the returned list of locations names
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue getLocationsList(std::vector<std::string>& locations) = 0;

    /**
    * Get a list of the names of all stored areas
    * @param the returned list of areas names
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue getAreasList(std::vector<std::string>& areas) = 0;

    /**
    * Get a list of the names of all stored paths
    * @param the returned list of paths names
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue getPathsList(std::vector<std::string>& paths) = 0;

    /**
    * Get a list of all stored objects
    * @param the returned list of objects
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue getAllObjects(std::vector<yarp::dev::Nav2D::Map2DObject>& obj) = 0;

    /**
    * Get a list of all stored locations
    * @param the returned list of locations
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue getAllLocations(std::vector<yarp::dev::Nav2D::Map2DLocation>& locations) = 0;

    /**
    * Get a list of all stored areas
    * @param the returned list of areas
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue getAllAreas(std::vector<yarp::dev::Nav2D::Map2DArea>& areas) = 0;

    /**
    * Get a list of all stored paths
    * @param the returned list of paths
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue getAllPaths(std::vector<yarp::dev::Nav2D::Map2DPath>& paths) = 0;

    /**
    * Searches for a object and renames it
    * @param original_name the name of the area
    * @param new_name the new name of the area
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue renameObject(std::string original_name, std::string new_name) = 0;

    /**
    * Searches for a location and renames it
    * @param original_name the name of the area
    * @param new_name the new name of the area
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue renameLocation(std::string original_name, std::string new_name) = 0;

    /**
    * Delete an object
    * @param object_name the name of the location
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue deleteObject(std::string object_name) = 0;

    /**
    * Delete a location
    * @param location_name the name of the location
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue deleteLocation(std::string location_name) = 0;

    /**
    * Delete a path
    * @param path_name the name of the path
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue deletePath(std::string path_name) = 0;

    /**
    * Searches for an area and renames it
    * @param original_name the name of the area
    * @param new_name the new name of the area
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue renameArea(std::string original_name, std::string new_name) = 0;

    /**
    * Searches for a path and renames it
    * @param original_name the name of the path
    * @param new_name the new name of the path
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue renamePath(std::string original_name, std::string new_name) = 0;

    /**
    * Delete an area
    * @param area_name the name of the area
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue deleteArea(std::string area_name) = 0;

    /**
    * Delete all stored objects
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue clearAllObjects() = 0;

    /**
    * Delete all stored locations
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue clearAllLocations() = 0;

    /**
    * Delete all stored areas
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue clearAllAreas() = 0;

    /**
    * Delete all stored paths
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue clearAllPaths() = 0;

    /**
    * Clear all temporary flags from all stored maps
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue clearAllMapsTemporaryFlags() = 0;

    /**
    * Clear all temporary flags from a specific map
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue clearMapTemporaryFlags(std::string map_name) = 0;

    /**
    * Save a map to disk
    * @param map_name the name of the area
    * @param file_name file name with full path
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue saveMapToDisk(std::string map_name, std::string file_name) = 0;

    /**
    * Load a map from disk
    * @param file_name file name with full path
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue loadMapFromDisk(std::string file_name) = 0;

    /**
    * Save a collection of maps to disk
    * @param file_name file name with full path
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue saveMapsCollection(std::string file_name) = 0;

    /**
    * Load a collection of maps from disk
    * @param file_name file name with full path
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue loadMapsCollection(std::string file_name) = 0;

    /**
    * Save a collection of locations/area/paths etc to disk
    * @param file_name file name with full path
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue saveLocationsAndExtras(std::string file_name) = 0;

    /**
    * Load a collection of locations/areas/paths etc from disk
    * @param file_name file name with full path
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue loadLocationsAndExtras(std::string file_name) = 0;

    /**
    * Reloads the last opened file containing the maps from disk
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue reloadMapsCollection() = 0;

    /**
    * Reloads the last opened file containing the collection of locations/areas/paths etc from disk
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue reloadLocationsAndExtras() = 0;

    /**
    * Enable/disables maps compression over the network
    * @param enable
    * @return a ReturnValue, convertible to true/false
    */
    virtual yarp::dev::ReturnValue enableMapsCompression(bool enable) = 0;
};

#endif // YARP_DEV_IMAP2D_H
