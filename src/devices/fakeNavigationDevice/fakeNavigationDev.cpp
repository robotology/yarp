/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include "fakeNavigationDev.h"
#include <math.h>
#include <cmath>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;

namespace {
YARP_LOG_COMPONENT(FAKENAVIGATION, "yarp.device.fakeNavigation")
}


bool fakeNavigation :: open(yarp::os::Searchable& config)
{
#if 1

    yCDebug(FAKENAVIGATION) << "config configuration: \n" << config.toString().c_str();

    std::string context_name = "robotGoto";
    std::string file_name = "robotGoto_cer.ini";

    if (config.check("context")) {
        context_name = config.find("context").asString();
    }
    if (config.check("from")) {
        file_name = config.find("from").asString();
    }

    yarp::os::ResourceFinder rf;
    rf.setDefaultContext(context_name.c_str());
    rf.setDefaultConfigFile(file_name.c_str());

    yarp::os::Property p;
    std::string configFile = rf.findFile("from");
    if (configFile != "") {
        p.fromConfigFile(configFile.c_str());
    }
    yCDebug(FAKENAVIGATION) << "robotGotoDev configuration: \n" << p.toString().c_str();

#else
    Property p;
    p.fromString(config.toString());
#endif
    navThread = new fakeNavigationThread(0.010, p);

    if (!navThread->start())
    {
        delete navThread;
        return false;
    }

    return true;
}

fakeNavigation::fakeNavigation()
{
    navThread =NULL;
}

//module cleanup
bool fakeNavigation:: close()
{
    navThread->stop();
    delete navThread;
    navThread =NULL;

    return true;
}

bool fakeNavigation::gotoTargetByAbsoluteLocation(Map2DLocation loc)
{
    if (m_status == NavigationStatusEnum::navigation_status_idle)
    {
        m_status = NavigationStatusEnum::navigation_status_moving;
        m_absgoal_loc = loc;
    }
    return true;
}

bool fakeNavigation::gotoTargetByRelativeLocation(double x, double y, double theta)
{
    yCInfo(FAKENAVIGATION) << "gotoTargetByRelativeLocation not yet implemented";
    return true;
}

bool fakeNavigation::gotoTargetByRelativeLocation(double x, double y)
{
    yCInfo(FAKENAVIGATION) << "gotoTargetByRelativeLocation not yet implemented";
    return true;
}

bool fakeNavigation::applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout)
{
    yCInfo(FAKENAVIGATION) << "applyVelocityCommand not yet implemented";
    return true;
}

bool fakeNavigation::stopNavigation()
{
    m_status=NavigationStatusEnum::navigation_status_idle;
    m_absgoal_loc=Map2DLocation();
    return true;
}

bool fakeNavigation::suspendNavigation(double time)
{
    if (m_status == NavigationStatusEnum::navigation_status_moving)
    {
        m_status=NavigationStatusEnum::navigation_status_paused;
    }
    return true;
}

bool fakeNavigation::resumeNavigation()
{
    if (m_status == NavigationStatusEnum::navigation_status_paused)
    {
        m_status = NavigationStatusEnum::navigation_status_moving;
    }
    return true;
}

bool fakeNavigation::getAllNavigationWaypoints(yarp::dev::Nav2D::TrajectoryTypeEnum trajectory_type, yarp::dev::Nav2D::Map2DPath& waypoints)
{
    yCInfo(FAKENAVIGATION) << "getAllNavigationWaypoints not yet implemented";
    return true;
}

bool fakeNavigation::getCurrentNavigationWaypoint(Map2DLocation& curr_waypoint)
{
    yCInfo(FAKENAVIGATION) << "getCurrentNavigationWaypoint not yet implemented";
    return true;
}

bool fakeNavigation::getCurrentNavigationMap(yarp::dev::Nav2D::NavigationMapTypeEnum map_type, MapGrid2D& map)
{
    yCInfo(FAKENAVIGATION) << "getCurrentNavigationMap not yet implemented";
    return true;
}

bool fakeNavigation::getNavigationStatus(yarp::dev::Nav2D::NavigationStatusEnum& status)
{
    status = m_status;
    return true;
}

bool fakeNavigation::getAbsoluteLocationOfCurrentTarget(Map2DLocation& target)
{
    target=m_absgoal_loc;
    return true;
}

bool fakeNavigation::recomputeCurrentNavigationPath()
{
    if (m_status == NavigationStatusEnum::navigation_status_moving)
    {
        //do something
    }
    return true;
}

bool fakeNavigation::getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta)
{
    yCInfo(FAKENAVIGATION) << "getRelativeLocationOfCurrentTarget not yet implemented";
    return true;
}

fakeNavigationThread::fakeNavigationThread(double _period, yarp::os::Searchable& _cfg) : PeriodicThread(_period)
{
}

bool fakeNavigationThread::threadInit()
{
    return true;
}

void fakeNavigationThread::threadRelease()
{

}

void fakeNavigationThread::run()
{

}
