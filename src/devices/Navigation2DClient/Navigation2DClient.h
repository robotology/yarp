/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

#ifndef YARP_DEV_NAVIGATION2DCLIENT_H
#define YARP_DEV_NAVIGATION2DCLIENT_H

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/os/ConstString.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/INavigation2D.h>

namespace yarp {
    namespace dev {
        class Navigation2DClient;
    }
}

#define DEFAULT_THREAD_PERIOD 20 //ms


/**
 *  @ingroup dev_impl_wrapper
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
    bool   gotoTargetByLocationName(yarp::os::ConstString location_name) override;
    bool   gotoTargetByRelativeLocation(double x, double y, double theta) override;

    bool   getAbsoluteLocationOfCurrentTarget(Map2DLocation& loc) override;
    bool   getNameOfCurrentTarget(yarp::os::ConstString& location_name);
    bool   getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta) override;

    bool   getCurrentPosition(Map2DLocation &loc) override;
    bool   setInitialPose(yarp::dev::Map2DLocation& loc) override;

    bool   storeCurrentPosition(yarp::os::ConstString location_name) override;
    bool   storeLocation(yarp::os::ConstString location_name, Map2DLocation loc) override;
    bool   getLocation(yarp::os::ConstString location_name, Map2DLocation& loc) override;
    bool   deleteLocation(yarp::os::ConstString location_name) override;
    bool   getLocationsList(std::vector<yarp::os::ConstString>& locations) override;

    bool   getNavigationStatus(NavigationStatusEnum& status) override;
    bool   clearAllLocations() override;
    bool   stopNavigation() override;
    bool   suspendNavigation() override;
    bool   resumeNavigation() override;
};

#endif // YARP_DEV_NAVIGATION2DCLIENT_H
