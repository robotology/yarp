/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_NAVIGATION2DCLIENT_H
#define YARP_DEV_NAVIGATION2DCLIENT_H

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/INavigation2D.h>

#include <mutex>
#include <string>

#define DEFAULT_THREAD_PERIOD 20 //ms


/**
 *  @ingroup dev_impl_network_clients dev_impl_navigation
 *
 * \section Navigation2DClient
 *
 * \brief `navigation2DClient`: A device which allows a client application to perform navigation tasks, such as commanding the robot to reach a specific location in a map.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | local          |      -         | string  | -              |   -           | Yes          | Full port name opened by the Navigation2DClient device.                             |       |
 * | navigation_server    |     -    | string  | -              |   -           | Yes          | Full port name of the port remotely opened by the Navigation server, to which the Navigation2DClient connects to.           |  |
 * | map_locations_server |     -    | string  | -              |   -           | Yes          | Full port name of the port remotely opened by the Map2DServer, to which the Navigation2DClient connects to.           |  |
 * | localization_server  |     -    | string  | -              |   -           | Yes          | Full port name of the port remotely opened by the Localization server, to which the Navigation2DClient connects to.           |  |
 */

class Navigation2DClient:
        public yarp::dev::DeviceDriver,
        public yarp::dev::Nav2D::INavigation2D,
        public yarp::os::PortReader
{
protected:
    std::mutex                    m_mutex;
    yarp::os::Port                m_rpc_port_navigation_server;
    yarp::os::Port                m_rpc_port_map_locations_server;
    yarp::os::Port                m_rpc_port_localization_server;
    yarp::os::Port                m_rpc_port_user_commands;
    std::string                   m_local_name;
    std::string                   m_navigation_server_name;
    std::string                   m_map_locations_server_name;
    std::string                   m_localization_server_name;
    int                           m_period;

private: //math stuff
    double                        normalize_angle(double angle);

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* RPC responder */
    bool parse_respond_string(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
    virtual bool read(yarp::os::ConnectionReader& connection) override;

    /* The following methods belong to INavigation2D interface */
    virtual bool checkInsideArea(yarp::dev::Nav2D::Map2DArea area) override;
    virtual bool checkInsideArea(std::string area_name)  override;
    virtual bool checkNearToLocation(yarp::dev::Nav2D::Map2DLocation loc, double linear_tolerance, double angular_tolerance = std::numeric_limits<double>::infinity()) override;
    virtual bool checkNearToLocation(std::string location_name, double linear_tolerance, double angular_tolerance = std::numeric_limits<double>::infinity()) override;

    bool   gotoTargetByAbsoluteLocation(yarp::dev::Nav2D::Map2DLocation loc) override;
    bool   gotoTargetByLocationName(std::string location_name) override;
    bool   gotoTargetByRelativeLocation(double x, double y, double theta) override;
    bool   gotoTargetByRelativeLocation(double x, double y) override;
    bool   applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout = 0.1) override;
    bool   recomputeCurrentNavigationPath() override;

    bool   getAbsoluteLocationOfCurrentTarget(yarp::dev::Nav2D::Map2DLocation& loc) override;
    bool   getNameOfCurrentTarget(std::string& location_name) override;
    bool   getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta) override;

    bool   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation &loc) override;
    bool   getEstimatedOdometry(yarp::dev::OdometryData& odom) override;
    bool   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc) override;
    bool   getLocalizationStatus(yarp::dev::Nav2D::LocalizationStatusEnum& status) override;
    bool   getEstimatedPoses(std::vector<yarp::dev::Nav2D::Map2DLocation>& poses) override;
    bool   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc, const yarp::sig::Matrix& cov) override;
    bool   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation& loc, yarp::sig::Matrix& cov) override;
    bool   startLocalizationService() override;
    bool   stopLocalizationService() override;

    bool   storeCurrentPosition(std::string location_name) override;
    bool   storeLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation loc) override;
    bool   getLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation& loc) override;
    bool   getArea(std::string area_name, yarp::dev::Nav2D::Map2DArea& area) override;
    bool   deleteLocation(std::string location_name) override;
    bool   getLocationsList(std::vector<std::string>& locations) override;

    bool   getNavigationStatus(yarp::dev::Nav2D::NavigationStatusEnum& status) override;
    bool   clearAllLocations() override;
    bool   stopNavigation() override;
    bool   suspendNavigation(const double time_s) override;
    bool   resumeNavigation() override;
    bool   getAllNavigationWaypoints(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type, yarp::dev::Nav2D::Map2DPath& waypoints) override;
    bool   getCurrentNavigationWaypoint(yarp::dev::Nav2D::Map2DLocation& curr_waypoint) override;
    bool   getCurrentNavigationMap(yarp::dev::Nav2D::NavigationMapTypeEnum map_type, yarp::dev::Nav2D::MapGrid2D& map) override;
};

#endif // YARP_DEV_NAVIGATION2DCLIENT_H
