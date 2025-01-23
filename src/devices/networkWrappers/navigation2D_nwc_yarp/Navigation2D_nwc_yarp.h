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
#include <yarp/dev/Map2DPath.h>
#include "IMap2DMsgs.h"
#include "ILocalization2DMsgs.h"
#include "INavigation2DMsgs.h"

#include <mutex>
#include <string>
#include "Navigation2D_nwc_yarp_ParamsParser.h"

#define DEFAULT_THREAD_PERIOD 20 //ms


/**
 *  @ingroup dev_impl_nwc_yarp dev_impl_navigation
 *
 * \section Navigation2D_nwc_yarp
 *
 * \brief `navigation2D_nwc_yarp`: A device which allows a client application to perform navigation tasks, such as commanding the robot to reach a specific location in a map.
 *
 * Parameters required by this device are shown in class: Navigation2D_nwc_yarp_ParamsParser
 */

class Navigation2D_nwc_yarp:
        public yarp::dev::DeviceDriver,
        public yarp::dev::Nav2D::INavigation2D,
        public yarp::os::PortReader,
        public Navigation2D_nwc_yarp_ParamsParser
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

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* RPC responder */
    bool parse_respond_string(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
    virtual bool read(yarp::os::ConnectionReader& connection) override;

    /* The following methods belong to INavigation2D interface */
           // subcategory: INavigation2DExtraActions
    yarp::dev::ReturnValue   checkInsideArea(yarp::dev::Nav2D::Map2DArea area, bool& is_inside) override;
    yarp::dev::ReturnValue   checkInsideArea(std::string area_name, bool& is_inside)  override;
    yarp::dev::ReturnValue   checkNearToLocation(yarp::dev::Nav2D::Map2DLocation loc, bool& is_near, double linear_tolerance, double angular_tolerance = std::numeric_limits<double>::infinity()) override;
    yarp::dev::ReturnValue   checkNearToLocation(std::string location_name, bool& is_near, double linear_tolerance, double angular_tolerance = std::numeric_limits<double>::infinity()) override;
    yarp::dev::ReturnValue   storeCurrentPosition(std::string location_name) override;
           // subcategory: INavigation2DControlActions
    yarp::dev::ReturnValue   getNavigationStatus(yarp::dev::Nav2D::NavigationStatusEnum& status) override;
    yarp::dev::ReturnValue   recomputeCurrentNavigationPath() override;
    yarp::dev::ReturnValue   stopNavigation() override;
    yarp::dev::ReturnValue   suspendNavigation(const double time_s) override;
    yarp::dev::ReturnValue   resumeNavigation() override;
    yarp::dev::ReturnValue   getAllNavigationWaypoints(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type, yarp::dev::Nav2D::Map2DPath& waypoints) override;
    yarp::dev::ReturnValue   getCurrentNavigationWaypoint(yarp::dev::Nav2D::Map2DLocation& curr_waypoint) override;
    yarp::dev::ReturnValue   getCurrentNavigationMap(yarp::dev::Nav2D::NavigationMapTypeEnum map_type, yarp::dev::Nav2D::MapGrid2D& map) override;
           // subcategory: INavigation2DTargetActions
    yarp::dev::ReturnValue   gotoTargetByAbsoluteLocation(yarp::dev::Nav2D::Map2DLocation loc) override;
    yarp::dev::ReturnValue   gotoTargetByLocationName(std::string location_name) override;
    yarp::dev::ReturnValue   gotoTargetByRelativeLocation(double x, double y, double theta) override;
    yarp::dev::ReturnValue   gotoTargetByRelativeLocation(double x, double y) override;
    yarp::dev::ReturnValue   followPath(const yarp::dev::Nav2D::Map2DPath& locs) override;
    yarp::dev::ReturnValue   getAbsoluteLocationOfCurrentTarget(yarp::dev::Nav2D::Map2DLocation& loc) override;
    yarp::dev::ReturnValue   getNameOfCurrentTarget(std::string& location_name) override;
    yarp::dev::ReturnValue   getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta) override;
           // subcategory: INavigation2DVelocityActions
    yarp::dev::ReturnValue   applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout = 0.1) override;
    yarp::dev::ReturnValue   getLastVelocityCommand(double& x_vel, double& y_vel, double& theta_vel) override;

    /* The following methods belong to INavigation2D, inherited from ILocalization2D interface */
    yarp::dev::ReturnValue   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation &loc) override;
    yarp::dev::ReturnValue   getEstimatedOdometry(yarp::dev::OdometryData& odom) override;
    yarp::dev::ReturnValue   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc) override;
    yarp::dev::ReturnValue   getLocalizationStatus(yarp::dev::Nav2D::LocalizationStatusEnum& status) override;
    yarp::dev::ReturnValue   getEstimatedPoses(std::vector<yarp::dev::Nav2D::Map2DLocation>& poses) override;
    yarp::dev::ReturnValue   setInitialPose(const yarp::dev::Nav2D::Map2DLocation& loc, const yarp::sig::Matrix& cov) override;
    yarp::dev::ReturnValue   getCurrentPosition(yarp::dev::Nav2D::Map2DLocation& loc, yarp::sig::Matrix& cov) override;
    yarp::dev::ReturnValue   startLocalizationService() override;
    yarp::dev::ReturnValue   stopLocalizationService() override;

    /* The following methods belong to INavigation2D, inherited from IMap2D interface */
    yarp::dev::ReturnValue   storeLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation loc) override;
    yarp::dev::ReturnValue   storeArea(std::string location_name, yarp::dev::Nav2D::Map2DArea area) override;
    yarp::dev::ReturnValue   storePath(std::string path_name, yarp::dev::Nav2D::Map2DPath path) override;

    yarp::dev::ReturnValue   getLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation& loc) override;
    yarp::dev::ReturnValue   getArea(std::string location_name, yarp::dev::Nav2D::Map2DArea& area) override;
    yarp::dev::ReturnValue   getPath(std::string path_name, yarp::dev::Nav2D::Map2DPath& path) override;

    yarp::dev::ReturnValue   renameLocation(std::string original_name, std::string new_name) override;
    yarp::dev::ReturnValue   renameArea(std::string original_name, std::string new_name) override;
    yarp::dev::ReturnValue   renamePath(std::string original_name, std::string new_name) override;

    yarp::dev::ReturnValue   deleteLocation(std::string location_name) override;
    yarp::dev::ReturnValue   deleteArea(std::string location_name) override;
    yarp::dev::ReturnValue   deletePath(std::string path_name) override;

    yarp::dev::ReturnValue   getLocationsList(std::vector<std::string>& locations) override;
    yarp::dev::ReturnValue   getAreasList(std::vector<std::string>& locations) override;
    yarp::dev::ReturnValue   getPathsList(std::vector<std::string>& paths) override;

    yarp::dev::ReturnValue   getAllLocations(std::vector<yarp::dev::Nav2D::Map2DLocation>& locations) override;
    yarp::dev::ReturnValue   getAllAreas(std::vector<yarp::dev::Nav2D::Map2DArea>& areas) override;
    yarp::dev::ReturnValue   getAllPaths(std::vector<yarp::dev::Nav2D::Map2DPath>& paths) override;

    yarp::dev::ReturnValue   clearAllLocations() override;
    yarp::dev::ReturnValue   clearAllAreas() override;
    yarp::dev::ReturnValue   clearAllPaths() override;

    yarp::dev::ReturnValue   clearAllMaps() override;
    yarp::dev::ReturnValue   remove_map(std::string map_name) override;
    yarp::dev::ReturnValue   store_map(const yarp::dev::Nav2D::MapGrid2D& map) override;
    yarp::dev::ReturnValue   get_map(std::string map_name, yarp::dev::Nav2D::MapGrid2D& map) override;
    yarp::dev::ReturnValue   get_map_names(std::vector<std::string>& map_names) override;

    yarp::dev::ReturnValue   clearAllMapsTemporaryFlags() override;
    yarp::dev::ReturnValue   clearMapTemporaryFlags(std::string map_name) override;

    yarp::dev::ReturnValue   saveMapsCollection(std::string maps_collection_file) override;
    yarp::dev::ReturnValue   loadMapsCollection(std::string maps_collection_file) override;
    yarp::dev::ReturnValue   saveLocationsAndExtras(std::string locations_collection_file) override;
    yarp::dev::ReturnValue   loadLocationsAndExtras(std::string locations_collection_file) override;

    yarp::dev::ReturnValue   saveMapToDisk(std::string map_name, std::string file_name) override;
    yarp::dev::ReturnValue   loadMapFromDisk(std::string file_name) override;

    yarp::dev::ReturnValue   enableMapsCompression(bool enable) override;
};

#endif // YARP_DEV_NAVIGATION2D_NWC_YARP_H
