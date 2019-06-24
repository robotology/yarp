/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IMAP2D_H
#define YARP_DEV_IMAP2D_H

#include <yarp/os/Vocab.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/api.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DArea.h>
#include <vector>
#include <string>

namespace yarp
{
    namespace dev
    {
        class IMap2D;
    }
}

/**
 * @ingroup dev_iface_navigation
 *
 * IMap2D Interface. Provides methods to store/retrieve map grids and locations to/from a map server.
 */
class YARP_dev_API yarp::dev::IMap2D
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
    virtual bool     store_map(const yarp::dev::MapGrid2D& map) = 0;

    /**
    Gets a map from the map server.
    * @return true/false
    */
    virtual bool     get_map(std::string map_name, yarp::dev::MapGrid2D& map) = 0;

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
    virtual bool storeLocation(std::string location_name, Map2DLocation loc) = 0;

    /**
    * Store an area
    * @param area_name the name of the area
    * @param area the area
    * @return true/false
    */
    virtual bool storeArea(std::string area_name, Map2DArea area) = 0;

    /**
    * Retrieves a location specified by the user in the world reference frame
    * @param location_name the name of the location
    * @param loc the location
    * @return true/false
    */
    virtual bool getLocation(std::string location_name, Map2DLocation& loc) = 0;

    /**
    * Retrieves an area
    * @param area_name the name of the area
    * @param area the area
    * @return true/false
    */
    virtual bool getArea(std::string area_name, Map2DArea& area) = 0;

    /**
    * Get a list of all stored locations
    * @param the returned list of locations
    * @return true/false
    */
    virtual bool getLocationsList(std::vector<std::string>& locations) = 0;

    /**
    * Get a list of all stored areas
    * @param the returned list of areas
    * @return true/false
    */
    virtual bool getAreasList(std::vector<std::string>& areas) = 0;

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
    * Searches for an area and renames it
    * @param original_name the name of the area
    * @param new_name the new name of the area
    * @return true/false
    */
    virtual bool renameArea(std::string original_name, std::string new_name) = 0;

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
};

constexpr yarp::conf::vocab32_t VOCAB_IMAP                    = yarp::os::createVocab('i','m','a','p');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_SET_MAP            = yarp::os::createVocab('s','e','t');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_GET_MAP            = yarp::os::createVocab('g','e','t');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_GET_NAMES          = yarp::os::createVocab('n','a','m','s');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_CLEAR              = yarp::os::createVocab('c','l','r');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_REMOVE             = yarp::os::createVocab('r','e','m','v');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_LOAD_COLLECTION    = yarp::os::createVocab('l','d','c','l');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_SAVE_COLLECTION    = yarp::os::createVocab('s','v','c','l');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_OK                 = yarp::os::createVocab('o','k','k');
constexpr yarp::conf::vocab32_t VOCAB_IMAP_ERROR              = yarp::os::createVocab('e','r','r');

#endif // YARP_DEV_IMAP2D_H
