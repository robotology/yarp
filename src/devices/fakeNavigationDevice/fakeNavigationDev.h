/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/PeriodicThread.h>
#include <math.h>


class fakeNavigationThread :
        public yarp::os::PeriodicThread
{
public:
    fakeNavigationThread(double _period, yarp::os::Searchable& _cfg);
    virtual bool threadInit() override;
    virtual void threadRelease() override;
    virtual void run() override;
};

/**
 * @ingroup dev_impl_fake dev_impl_navigation
 *
 * \brief `fakeNavigation`: Documentation to be added
 */
class fakeNavigation :
        public yarp::dev::DeviceDriver,
        public yarp::dev::Nav2D::INavigation2DTargetActions,
        public yarp::dev::Nav2D::INavigation2DControlActions,
        public yarp::dev::Nav2D::INavigation2DVelocityActions
{
private:
    yarp::dev::Nav2D::NavigationStatusEnum m_status = yarp::dev::Nav2D::NavigationStatusEnum::navigation_status_idle;
    yarp::dev::Nav2D::Map2DLocation m_absgoal_loc;

public:
    fakeNavigationThread  *navThread=nullptr;

public:
    virtual bool open(yarp::os::Searchable& config) override;

    fakeNavigation();

    //module cleanup
    virtual bool close() override;

public:
    //methods inherited from INavigation2Dxxx interfaces
    bool gotoTargetByAbsoluteLocation(yarp::dev::Nav2D::Map2DLocation loc) override;
    bool gotoTargetByRelativeLocation(double x, double y, double theta) override;
    bool gotoTargetByRelativeLocation(double x, double y) override;
    bool getAbsoluteLocationOfCurrentTarget(yarp::dev::Nav2D::Map2DLocation& target) override;
    bool getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta) override;
    bool getNavigationStatus(yarp::dev::Nav2D::NavigationStatusEnum& status) override;
    bool stopNavigation() override;
    bool suspendNavigation(double time) override;
    bool resumeNavigation() override;
    bool getAllNavigationWaypoints(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type, yarp::dev::Nav2D::Map2DPath& waypoints) override;
    bool getCurrentNavigationWaypoint(yarp::dev::Nav2D::Map2DLocation& curr_waypoint) override;
    bool getCurrentNavigationMap(yarp::dev::Nav2D::NavigationMapTypeEnum map_type, yarp::dev::Nav2D::MapGrid2D& map) override;
    bool recomputeCurrentNavigationPath() override;
    bool applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout = 0.1) override;
    bool getLastVelocityCommand(double& x_vel, double& y_vel, double& theta_vel) override;
};
