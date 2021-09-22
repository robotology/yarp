/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMAP2D_H
#define YARP_DEV_IMAP2D_H

#include <yarp/os/Vocab.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/api.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DArea.h>
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
    * @return true/false
    */
    virtual bool     clearAllMaps () = 0;

    /**
    Stores a map into the map server.
    * @return true/false
    */
    virtual bool     store_map(const yarp::dev::Nav2D::MapGrid2D& map) = 0;

    /**
    Gets a map from the map server.
    * @return true/false
    */
    virtual bool     get_map(std::string map_name, yarp::dev::Nav2D::MapGrid2D& map) = 0;

    /**
    Gets a list containing the names of all registered maps.
    * @return true/false
    */
    virtual bool     get_map_names(std::vector<std::string>& map_names) = 0;

    /**
    Removes a map from the map server.
    * @return true/false
    */
    virtual bool     remove_map(std::string map_name) = 0;

    /**
    * Store a location specified by the user in the world reference frame
    * @param location_name the name of the location
    * @param loc the location of the robot
    * @return true/false
    */
    virtual bool storeLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation loc) = 0;

    /**
    * Store an area
    * @param area_name the name of the area
    * @param area the area
    * @return true/false
    */
    virtual bool storeArea(std::string area_name, yarp::dev::Nav2D::Map2DArea area) = 0;

    /**
    * Store a path
    * @param path_name the name of the path
    * @param path the path
    * @return true/false
    */
    virtual bool storePath(std::string path_name, yarp::dev::Nav2D::Map2DPath path) = 0;

    /**
    * Retrieves a location specified by the user in the world reference frame
    * @param location_name the name of the location
    * @param loc the location
    * @return true/false
    */
    virtual bool getLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation& loc) = 0;

    /**
    * Retrieves an area
    * @param area_name the name of the area
    * @param area the area
    * @return true/false
    */
    virtual bool getArea(std::string area_name, yarp::dev::Nav2D::Map2DArea& area) = 0;

    /**
    * Retrieves a path
    * @param path_name the name of the path
    * @param path the path
    * @return true/false
    */
    virtual bool getPath(std::string path_name, yarp::dev::Nav2D::Map2DPath& path) = 0;

    /**
    * Get a list of the names of all stored locations
    * @param the returned list of locations names
    * @return true/false
    */
    virtual bool getLocationsList(std::vector<std::string>& locations) = 0;

    /**
    * Get a list of the names of all stored areas
    * @param the returned list of areas names
    * @return true/false
    */
    virtual bool getAreasList(std::vector<std::string>& areas) = 0;

    /**
    * Get a list of the names of all stored paths
    * @param the returned list of paths names
    * @return true/false
    */
    virtual bool getPathsList(std::vector<std::string>& paths) = 0;

    /**
    * Get a list of all stored locations
    * @param the returned list of locations
    * @return true/false
    */
    virtual bool getAllLocations(std::vector<yarp::dev::Nav2D::Map2DLocation>& locations) = 0;

    /**
    * Get a list of all stored areas
    * @param the returned list of areas
    * @return true/false
    */
    virtual bool getAllAreas(std::vector<yarp::dev::Nav2D::Map2DArea>& areas) = 0;

    /**
    * Get a list of all stored paths
    * @param the returned list of paths
    * @return true/false
    */
    virtual bool getAllPaths(std::vector<yarp::dev::Nav2D::Map2DPath>& paths) = 0;

    /**
    * Searches for a location and renames it
    * @param original_name the name of the area
    * @param new_name the new name of the area
    * @return true/false
    */
    virtual bool renameLocation(std::string original_name, std::string new_name) = 0;

    /**
    * Delete a location
    * @param location_name the name of the location
    * @return true/false
    */
    virtual bool deleteLocation(std::string location_name) = 0;

    /**
    * Delete a path
    * @param path_name the name of the path
    * @return true/false
    */
    virtual bool deletePath(std::string path_name) = 0;

    /**
    * Searches for an area and renames it
    * @param original_name the name of the area
    * @param new_name the new name of the area
    * @return true/false
    */
    virtual bool renameArea(std::string original_name, std::string new_name) = 0;

    /**
    * Searches for a path and renames it
    * @param original_name the name of the path
    * @param new_name the new name of the path
    * @return true/false
    */
    virtual bool renamePath(std::string original_name, std::string new_name) = 0;

    /**
    * Delete an area
    * @param area_name the name of the area
    * @return true/false
    */
    virtual bool deleteArea(std::string area_name) = 0;

    /**
    * Delete all stored locations
    * @return true/false
    */
    virtual bool clearAllLocations() = 0;

    /**
    * Delete all stored areas
    * @return true/false
    */
    virtual bool clearAllAreas() = 0;

    /**
    * Delete all stored paths
    * @return true/false
    */
    virtual bool clearAllPaths() = 0;

    /**
    * Clear all temporary flags from all stored maps
    * @return true/false
    */
    virtual bool clearAllMapsTemporaryFlags() = 0;

    /**
    * Clear all temporary flags from a specific map
    * @return true/false
    */
    virtual bool clearMapTemporaryFlags(std::string map_name) = 0;

    /**
    * Save a map to disk
    * @param map_name the name of the area
    * @param file_name file name with full path
    * @return true/false
    */
    virtual bool saveMapToDisk(std::string map_name, std::string file_name) = 0;

    /**
    * Load a map from disk
    * @param file_name file name with full path
    * @return true/false
    */
    virtual bool loadMapFromDisk(std::string file_name) = 0;

    /**
    * Save a collection of maps to disk
    * @param file_name file name with full path
    * @return true/false
    */
    virtual bool saveMapsCollection(std::string file_name) = 0;

    /**
    * Load a collection of maps from disk
    * @param file_name file name with full path
    * @return true/false
    */
    virtual bool loadMapsCollection(std::string file_name) = 0;

    /**
    * Save a collection of locations/area/paths etc to disk
    * @param file_name file name with full path
    * @return true/false
    */
    virtual bool saveLocationsAndExtras(std::string file_name) = 0;

    /**
    * Load a collection of locations/areas/paths etc from disk
    * @param file_name file name with full path
    * @return true/false
    */
    virtual bool loadLocationsAndExtras(std::string file_name) = 0;

    /**
    * 99999999999
    * @param enable
    * @return true/false
    */
    virtual bool enableMapsCompression(bool enable) = 0;
};

//This section of vocabs is used just by deprecated device Map2DServer and Map2DClient
//It can be thus safely eliminated as soon as these devices are removed from yarp
constexpr yarp::conf::vocab32_t VOCAB_IMAP                      = yarp::os::createVocab32('i','m','a','p');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_SET_MAP              = yarp::os::createVocab32('s','e','t');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_GET_MAP              = yarp::os::createVocab32('g','e','t');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_GET_NAMES            = yarp::os::createVocab32('n','a','m','s');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_CLEAR_ALL_MAPS       = yarp::os::createVocab32('c','l','r');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_REMOVE               = yarp::os::createVocab32('r','e','m','v');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_LOAD_X               = yarp::os::createVocab32('l','o','a','d');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_SAVE_X               = yarp::os::createVocab32('s','a','v','e');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_MAPS_COLLECTION      = yarp::os::createVocab32('m','a','p','s');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_LOCATIONS_COLLECTION = yarp::os::createVocab32('l','o','c','s');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_OK                   = yarp::os::createVocab32('o','k','k');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_ERROR                = yarp::os::createVocab32('e','r','r');

#endif // YARP_DEV_IMAP2D_H
