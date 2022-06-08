/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_NAVIGATION2D_NWC_YARP_H
#define YARP_DEV_NAVIGATION2D_NWC_YARP_H

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/INavigation2D.h>
#include "IMap2DMsgs.h"
#include "ILocalization2DMsgs.h"
#include "INavigation2DMsgs.h"

#include <mutex>
#include <string>

#define DEFAULT_THREAD_PERIOD 20 //ms


/**
 *  @ingroup dev_impl_network_clients dev_impl_navigation
 *
 * \section Navigation2D_nwc_yarp
 *
 * \brief `navigation2D_nwc_yarp`: A device which allows a client application to perform navigation tasks, such as commanding the robot to reach a specific location in a map.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | local          |      -         | string  | -              |   -           | Yes          | Full port name opened by the Navigation2D_nwc_yarp device.                             |       |
 * | navigation_server    |     -    | string  | -              |   -           | Yes          | Full port name of the port remotely opened by the Navigation server, to which the Navigation2D_nwc_yarp connects to.           |  |
 * | map_locations_server |     -    | string  | -              |   -           | Yes          | Full port name of the port remotely opened by the Map2DServer, to which the Navigation2D_nwc_yarp connects to.           |  |
 * | localization_server  |     -    | string  | -              |   -           | Yes          | Full port name of the port remotely opened by the Localization server, to which the Navigation2D_nwc_yarp connects to.           |  |
 */

class Navigation2D_nwc_yarp:
        public yarp::dev::DeviceDriver,
        public yarp::dev::Nav2D::INavigation2D,
        public yarp::os::PortReader
{
protected:
    //thrift stuff
    IMap2DMsgs                    m_map_RPC;
    ILocalization2DMsgs           m_loc_RPC;
    INavigation2DMsgs             m_nav_RPC;

protected:
    std::mutex                    m_mutex;
    yarp::os::Port                m_rpc_port_to_navigation_server;
    yarp::os::Port                m_rpc_port_to_Map2DServer;
    yarp::os::Port                m_rpc_port_to_localization_server;
    yarp::os::Port                m_rpc_port_user_commands;
    std::string                   m_local_name;
    std::string                   m_navigation_server_name;
    std::string                   m_map_locations_server_name;
    std::string                   m_localization_server_name;
    int                           m_period;

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* RPC responder */
    bool parse_respond_string(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
    virtual bool read(yarp::os::ConnectionReader& connection) override;

    /* The following methods belong to INavigation2D interface */
           // subcategory: INavigation2DExtraActions
    bool   checkInsideArea(yarp::dev::Nav2D::Map2DArea area) override;
    bool   checkInsideArea(std::string area_name)  override;
    bool   checkNearToLocation(yarp::dev::Nav2D::Map2DLocation loc, double linear_tolerance, double angular_tolerance = std::numeric_limits<double>::infinity()) override;
    bool   checkNearToLocation(std::string location_name, double linear_tolerance, double angular_tolerance = std::numeric_limits<double>::infinity()) override;
    bool   storeCurrentPosition(std::string location_name) override;
           // subcategory: INavigation2DControlActions
    bool   getNavigationStatus(yarp::dev::Nav2D::NavigationStatusEnum& status) override;
    bool   recomputeCurrentNavigationPath() override;
    bool   stopNavigation() override;
    bool   suspendNavigation(const double time_s) override;
    bool   resumeNavigation() override;
    bool   getAllNavigationWaypoints(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type, yarp::dev::Nav2D::Map2DPath& waypoints) override;
    bool   getCurrentNavigationWaypoint(yarp::dev::Nav2D::Map2DLocation& curr_waypoint) override;
    bool   getCurrentNavigationMap(yarp::dev::Nav2D::NavigationMapTypeEnum map_type, yarp::dev::Nav2D::MapGrid2D& map) override;
           // subcategory: INavigation2DTargetActions
    bool   gotoTargetByAbsoluteLocation(yarp::dev::Nav2D::Map2DLocation loc) override;
    bool   gotoTargetByLocationName(std::string location_name) override;
    bool   gotoTargetByRelativeLocation(double x, double y, double theta) override;
    bool   gotoTargetByRelativeLocation(double x, double y) override;
    bool   getAbsoluteLocationOfCurrentTarget(yarp::dev::Nav2D::Map2DLocation& loc) override;
    bool   getNameOfCurrentTarget(std::string& location_name) override;
    bool   getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta) override;
           // subcategory: INavigation2DVelocityActions
    bool   applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout = 0.1) override;
    bool   getLastVelocityCommand(double& x_vel, double& y_vel, double& theta_vel) override;

    /* The following methods belong to INavigation2D, inherited from ILocalization2D interface */
    bool   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation &loc) override;
    bool   getEstimatedOdometry(yarp::dev::OdometryData& odom) override;
    bool   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc) override;
    bool   getLocalizationStatus(yarp::dev::Nav2D::LocalizationStatusEnum& status) override;
    bool   getEstimatedPoses(std::vector<yarp::dev::Nav2D::Map2DLocation>& poses) override;
    bool   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc, const yarp::sig::Matrix& cov) override;
    bool   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation& loc, yarp::sig::Matrix& cov) override;
    bool   startLocalizationService() override;
    bool   stopLocalizationService() override;

    /* The following methods belong to INavigation2D, inherited from IMap2D interface */
    bool   storeLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation loc) override;
    bool   storeArea(std::string location_name, yarp::dev::Nav2D::Map2DArea area) override;
    bool   storePath(std::string path_name, yarp::dev::Nav2D::Map2DPath path) override;

    bool   getLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation& loc) override;
    bool   getArea(std::string location_name, yarp::dev::Nav2D::Map2DArea& area) override;
    bool   getPath(std::string path_name, yarp::dev::Nav2D::Map2DPath& path) override;

    bool   renameLocation(std::string original_name, std::string new_name) override;
    bool   renameArea(std::string original_name, std::string new_name) override;
    bool   renamePath(std::string original_name, std::string new_name) override;

    bool   deleteLocation(std::string location_name) override;
    bool   deleteArea(std::string location_name) override;
    bool   deletePath(std::string path_name) override;

    bool   getLocationsList(std::vector<std::string>& locations) override;
    bool   getAreasList(std::vector<std::string>& locations) override;
    bool   getPathsList(std::vector<std::string>& paths) override;

    bool   getAllLocations(std::vector<yarp::dev::Nav2D::Map2DLocation>& locations) override;
    bool   getAllAreas(std::vector<yarp::dev::Nav2D::Map2DArea>& areas) override;
    bool   getAllPaths(std::vector<yarp::dev::Nav2D::Map2DPath>& paths) override;

    bool   clearAllLocations() override;
    bool   clearAllAreas() override;
    bool   clearAllPaths() override;

    bool   clearAllMaps() override;
    bool   remove_map(std::string map_name) override;
    bool   store_map(const yarp::dev::Nav2D::MapGrid2D& map) override;
    bool   get_map(std::string map_name, yarp::dev::Nav2D::MapGrid2D& map) override;
    bool   get_map_names(std::vector<std::string>& map_names) override;

    bool   clearAllMapsTemporaryFlags() override;
    bool   clearMapTemporaryFlags(std::string map_name) override;

    bool   saveMapsCollection(std::string maps_collection_file) override;
    bool   loadMapsCollection(std::string maps_collection_file) override;
    bool   saveLocationsAndExtras(std::string locations_collection_file) override;
    bool   loadLocationsAndExtras(std::string locations_collection_file) override;

    bool   saveMapToDisk(std::string map_name, std::string file_name) override;
    bool   loadMapFromDisk(std::string file_name) override;

    bool   enableMapsCompression(bool enable) override;
};

#endif // YARP_DEV_NAVIGATION2D_NWC_YARP_H
