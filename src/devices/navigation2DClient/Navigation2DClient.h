/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
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

#ifndef YARP_DEV_NAVIGATION2DCLIENT_H
#define YARP_DEV_NAVIGATION2DCLIENT_H

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <string>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/INavigation2D.h>

namespace yarp {
    namespace dev {
        class Navigation2DClient;
    }
}

#define DEFAULT_THREAD_PERIOD 20 //ms


/**
 *  @ingroup dev_impl_network_clients
 *
 * \section Navigation2DClient
 * A device which allows a client application to perform navigation tasks, such as commanding the robot to reach a specific location in a map.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | local          |      -         | string  | -              |   -           | Yes          | Full port name opened by the Navigation2DClient device.                             |       |
 * | navigation_server    |     -    | string  | -              |   -           | Yes          | Full port name of the port remotely opened by the Navigation server, to which the Navigation2DClient connects to.           |  |
 * | map_locations_server |     -    | string  | -              |   -           | Yes          | Full port name of the port remotely opened by the Map2DServer, to which the Navigation2DClient connects to.           |  |
 * | localization_server  |     -    | string  | -              |   -           | Yes          | Full port name of the port remotely opened by the Localization server, to which the Navigation2DClient connects to.           |  |
 */

class yarp::dev::Navigation2DClient: public DeviceDriver,
                                       public INavigation2D
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:

    yarp::os::Mutex               m_mutex;
    yarp::os::Port                m_rpc_port_navigation_server;
    yarp::os::Port                m_rpc_port_map_locations_server;
    yarp::os::Port                m_rpc_port_localization_server;
    std::string                   m_local_name;
    std::string                   m_navigation_server_name;
    std::string                   m_map_locations_server_name;
    std::string                   m_localization_server_name;
    int                           m_period;

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* The following methods belong to INavigation2D interface */
    bool   gotoTargetByAbsoluteLocation(Map2DLocation loc) override;
    bool   gotoTargetByLocationName(std::string location_name) override;
    bool   gotoTargetByRelativeLocation(double x, double y, double theta) override;

    bool   getAbsoluteLocationOfCurrentTarget(Map2DLocation& loc) override;
    bool   getNameOfCurrentTarget(std::string& location_name);
    bool   getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta) override;

    bool   getCurrentPosition(Map2DLocation &loc) override;
    bool   setInitialPose(yarp::dev::Map2DLocation& loc) override;

    bool   storeCurrentPosition(std::string location_name) override;
    bool   storeLocation(std::string location_name, Map2DLocation loc) override;
    bool   getLocation(std::string location_name, Map2DLocation& loc) override;
    bool   deleteLocation(std::string location_name) override;
    bool   getLocationsList(std::vector<std::string>& locations) override;

    bool   getNavigationStatus(NavigationStatusEnum& status) override;
    bool   clearAllLocations() override;
    bool   stopNavigation() override;
    bool   suspendNavigation() override;
    bool   resumeNavigation() override;
};

#endif // YARP_DEV_NAVIGATION2DCLIENT_H
