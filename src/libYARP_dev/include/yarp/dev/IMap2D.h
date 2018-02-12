/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_IMAP2D_H
#define YARP_DEV_IMAP2D_H

#include <yarp/os/Vocab.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/Map2DLocation.h>
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
    virtual ~IMap2D() {}

    /**
     Removes all the registered maps from the server.
    * @return true/false
    */
    virtual bool     clear () = 0;

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
    virtual bool storeLocation(yarp::os::ConstString location_name, Map2DLocation loc) = 0;

    /**
    * Retrieves a location specified by the user in the world reference frame
    * @param location_name the name of the location
    * @param loc the location of the robot
    * @return true/false
    */
    virtual bool getLocation(yarp::os::ConstString location_name, Map2DLocation& loc) = 0;

    /**
    * Get a list of all stored locations
    * @param the returned list of locations
    * @return true/false
    */
    virtual bool getLocationsList(std::vector<yarp::os::ConstString>& locations) = 0;

    /**
    * Delete a location
    * @param location_name the name of the location
    * @return true/false
    */
    virtual bool deleteLocation(yarp::os::ConstString location_name) = 0;

    /**
    * Delete all stored locations
    * @return true/false
    */
    virtual bool clearAllLocations() = 0;
};

#define VOCAB_IMAP                    VOCAB4('i','m','a','p')
#define VOCAB_IMAP_SET_MAP            VOCAB3('s','e','t')
#define VOCAB_IMAP_GET_MAP            VOCAB3('g','e','t')
#define VOCAB_IMAP_GET_NAMES          VOCAB4('n','a','m','s')
#define VOCAB_IMAP_CLEAR              VOCAB3('c','l','r')
#define VOCAB_IMAP_REMOVE             VOCAB4('r','e','m','v')
#define VOCAB_IMAP_LOAD_COLLECTION    VOCAB4('l','d','c','l')
#define VOCAB_IMAP_SAVE_COLLECTION    VOCAB4('s','v','c','l')
#define VOCAB_IMAP_OK                 VOCAB3('o','k','k')
#define VOCAB_IMAP_ERROR              VOCAB3('e','r','r')

#endif // YARP_DEV_IMAP2D_H
