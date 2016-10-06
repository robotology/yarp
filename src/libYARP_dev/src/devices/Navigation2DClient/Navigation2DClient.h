/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#ifndef NAVIGATION2D_CLIENT_H
#define NAVIGATION2D_CLIENT_H


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
    yarp::os::ConstString         m_local_name;
    yarp::os::ConstString         m_remote_name;
    yarp::os::ConstString         m_remote_location_name;
    int                           m_period;

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config);
    bool close();

    bool   gotoTargetByAbsoluteLocation(Map2DLocation loc);
    bool   gotoTargetByLocationName(yarp::os::ConstString location_name);
    bool   gotoTargetByRelativeLocation(double x, double y, double theta);

    bool   getAbsoluteLocationOfCurrentTarget(Map2DLocation loc);
    bool   getNameOfCurrentTarget(yarp::os::ConstString& location_name);
    bool   getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta);

    bool   getCurrentPosition(Map2DLocation loc);
    bool   storeCurrentPosition(yarp::os::ConstString location_name);

    bool   storeLocation(yarp::os::ConstString location_name, Map2DLocation loc);
    bool   getLocation(yarp::os::ConstString location_name, Map2DLocation& loc);
    bool   deleteLocation(yarp::os::ConstString location_name);
    bool   getLocationsList(std::vector<yarp::os::ConstString>& locations);
    bool   getNavigationStatus(NavigationStatusEnum& status);
    bool   clearAllLocations();
    bool   stopNavigation();
    bool   suspendNavigation();
    bool   resumeNavigation();
};

#endif // NAVIGATION2D_CLIENT_H
