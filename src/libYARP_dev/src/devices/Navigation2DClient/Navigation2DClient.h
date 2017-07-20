/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_NAVIGATION2DCLIENT_H
#define YARP_DEV_NAVIGATION2DCLIENT_H

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
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
* @ingroup dev_impl_wrapper
*
*/

class yarp::dev::Navigation2DClient: public DeviceDriver,
                                       public INavigation2D
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:

    yarp::os::Mutex               m_mutex;
    yarp::os::Port                m_rpc_port_navigation_server;
    yarp::os::Port                m_rpc_port_locations_server;
    yarp::os::Port                m_rpc_port_localization_server;
    yarp::os::ConstString         m_local_name;
    yarp::os::ConstString         m_navigation_server_name;
    yarp::os::ConstString         m_locations_server_name;
    yarp::os::ConstString         m_localization_server_name;
    int                           m_period;

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

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
