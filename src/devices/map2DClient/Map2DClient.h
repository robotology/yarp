/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

/**
 * @ingroup dev_impl_network_clients
 *
 * \section Map2DClient
 * A device which allows a client application to store/retrieve user maps device in a map server.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | local          |      -         | string  | -   |   -           | Yes          | Full port name opened by the Map2DClient device.                             |       |
 * | remote         |     -          | string  | -   |   -           | Yes          | Full port name of the port remotely opened by the Map2DServer, to which the Map2DClient connects to.           |  |
 */

class yarp::dev::Map2DClient : public DeviceDriver,
                               public IMap2D
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:

    yarp::os::Port                m_rpcPort_to_Map2DServer;
    std::string         m_local_name;
    std::string         m_map_server;

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

     /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* The following methods belong to IMap2D interface */
    bool     clearAllMaps  () override;
    bool     remove_map (std::string map_name) override;
    bool     store_map  (const yarp::dev::MapGrid2D& map) override;
    bool     get_map    (std::string map_name, yarp::dev::MapGrid2D& map) override;
    bool     get_map_names(std::vector<std::string>& map_names) override;

    bool     storeLocation(std::string location_name, Map2DLocation loc) override;
    bool     getLocation(std::string location_name, Map2DLocation& loc) override;
    bool     deleteLocation(std::string location_name) override;
    bool     getLocationsList(std::vector<std::string>& locations) override;
    bool     clearAllLocations() override;
};

#endif // YARP_DEV_MAP2DCLIENT_H
