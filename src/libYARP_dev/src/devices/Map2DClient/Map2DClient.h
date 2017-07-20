/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_MAP2DCLIENT_H
#define YARP_DEV_MAP2DCLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/RecursiveMutex.h>

namespace yarp {
    namespace dev {
        class Map2DClient;
    }
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class Map2D_type
{
    private:
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

/**
 *  @ingroup dev_impl_wrapper
 *
 * \section Map2DClient
 * A device which allows a client application to store/retrieve user maps device in a map server.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | local          |      -         | string  | -   |   -           | Yes          | Full port name openend by the Map2DClient device.                            |       |
 * | remote         |     -          | string  | -   |   -           | Yes          | Full port name of the port opened by the Map2DServer, to which the Map2DClient connects to.           |  |
 */

class yarp::dev::Map2DClient : public DeviceDriver,
                               public IMap2D
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:

    yarp::os::Port                m_rpcPort_to_Map2DServer;
    yarp::os::Port                m_rpcPort_to_LocationsServer;
    yarp::os::ConstString         m_local_name;
    yarp::os::ConstString         m_locations_server;
    yarp::os::ConstString         m_map_server;

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

     /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    virtual bool     clear      () override;
    virtual bool     remove_map (std::string map_name) override;
    virtual bool     store_map  (const yarp::dev::MapGrid2D& map) override;
    virtual bool     get_map    (std::string map_name, yarp::dev::MapGrid2D& map) override;
    virtual bool     get_map_names(std::vector<std::string>& map_names) override;

    virtual bool     storeLocation(yarp::os::ConstString location_name, Map2DLocation loc) override;
    virtual bool     getLocation(yarp::os::ConstString location_name, Map2DLocation& loc) override;
    virtual bool     deleteLocation(yarp::os::ConstString location_name) override;
    virtual bool     getLocationsList(std::vector<yarp::os::ConstString>& locations) override;
    virtual bool     clearAllLocations() override;
};

#endif // YARP_DEV_MAP2DCLIENT_H
