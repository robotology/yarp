/*
 * Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_INAVIGATION_H
#define YARP_INAVIGATION_H

#include <yarp/os/Vocab.h>
#include <yarp/os/Log.h>
#include <vector>

namespace yarp {
    namespace dev {
        class INavigation2D;

        enum NavigationStatusEnum
        {
            navigation_status_idle             = VOCAB4('i', 'd', 'l', 'e'),
            navigation_status_moving           = VOCAB4('m', 'o', 'v', 'g'),
            navigation_status_waiting_obstacle = VOCAB4('w', 'a', 'i', 't'),
            navigation_status_goal_reached     = VOCAB4('r', 'e', 'c', 'h'),
            navigation_status_aborted          = VOCAB4('a', 'b', 'r', 't'),
            navigation_status_paused           = VOCAB4('p', 'a', 'u', 's'),
            navigation_status_thinking         = VOCAB4('t', 'h', 'n', 'k')
        };

        struct Map2DLocation
        {
            yarp::os::ConstString map_id;
            double x;
            double y;
            double theta;
        };

      }
}

/**
 * @ingroup dev_iface_motor
 *
 * Control board, encoder interface.
 */
class yarp::dev::INavigation2D
{
public:
    /**
     * Destructor.
     */
    virtual ~INavigation2D() {}

    /**
     * Ask the robot to reach a position defined in the world reference frame
     * @param loc the location to be reached
     * @return true/false
     */
    virtual bool gotoTargetByAbsoluteLocation(yarp::dev::Map2DLocation loc) = 0;

    /**
     * Ask the robot to reach a previosuly stored location
     * @param location_name the name of a location previously saved
     * @return true/false
     */
    virtual bool gotoTargetByLocationName(yarp::os::ConstString location_name) = 0;

    /**
    * Ask the robot to reach a position defined in the robot reference frame
    * @param x
    * @param y
    * @param theta
    * @return true/false
    */
    virtual bool gotoTargetByRelativeLocation(double x, double y, double theta) = 0;

    /**
    * Gets the current position of the robot w.r.t world reference frame
    * @param loc the location of the robot
    * @return true/false
    */
    virtual bool   getCurrentPosition(yarp::dev::Map2DLocation loc) = 0;

    /**
    * Gets the last navigation target in the world reference frame
    * @param x
    * @param loc the location of the robot
    * @return true/false
    */
    virtual bool getAbsoluteLocationOfCurrentTarget(Map2DLocation loc) = 0;

    /**
    * Gets the last navigation target in the robot reference frame
    * @param x
    * @param y
    * @param theta
    * @return true/false
    */
    virtual bool getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta) = 0;

    /**
    * Store the current location of the robot
    * @param location_name the name of the location
    * @return true/false
    */
    virtual bool storeCurrentPosition(yarp::os::ConstString location_name) = 0;

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

    /**
    * Gets the current status of the navigation task.
    * @return true/false
    */
    virtual bool getNavigationStatus(NavigationStatusEnum& status) = 0;

    /**
    * Terminates the current navigation task. Cannot be resumed.
    * @return true/false
    */
    virtual bool stopNavigation() = 0;

    /**
    * Ask to the robot to suspend the current navigation task. Can be resumed by resume()
    * @return true/false
    */
    virtual bool suspendNavigation() = 0;

    /**
    * Resume a previosuly suspended navigation task.
    * @return true/false
    */
    virtual bool resumeNavigation() = 0;
};

#define VOCAB_INAVIGATION           VOCAB4('i','n','a','v')

#define VOCAB_NAV_GOTOABS           VOCAB4('s','a','b','s')
#define VOCAB_NAV_GOTOREL           VOCAB4('s','r','e','l')

#define VOCAB_NAV_GET_LOCATION      VOCAB4('g','l','o','c')
#define VOCAB_NAV_GET_LOCATION_LIST VOCAB4('l','i','s','t')
#define VOCAB_NAV_GET_ABS_TARGET    VOCAB4('g','a','b','s')
#define VOCAB_NAV_GET_REL_TARGET    VOCAB4('g','r','e','l')
#define VOCAB_NAV_GET_NAME_TARGET   VOCAB4('g','n','a','m')
#define VOCAB_NAV_GET_CURRENT_POS   VOCAB4('g','p','o','s')
#define VOCAB_NAV_GET_STATUS        VOCAB4('g','s','t','s')
#define VOCAB_NAV_CLEAR             VOCAB4('c','l','r','l')
#define VOCAB_NAV_DELETE            VOCAB4('d','e','l','l')
#define VOCAB_NAV_STORE_ABS         VOCAB4('s','t','o','a')

#define VOCAB_NAV_STOP              VOCAB4('s','t','o','p')
#define VOCAB_NAV_SUSPEND           VOCAB4('s','u','s','p')
#define VOCAB_NAV_RESUME            VOCAB4('r','e','s','m')

#endif
