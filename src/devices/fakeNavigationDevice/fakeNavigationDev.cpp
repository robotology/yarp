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

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include "fakeNavigationDev.h"
#include <math.h>
#include <cmath>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;

bool fakeNavigation :: open(yarp::os::Searchable& config)
{
#if 1

    yDebug() << "config configuration: \n" << config.toString().c_str();

    std::string context_name = "robotGoto";
    std::string file_name = "robotGoto_cer.ini";

    if (config.check("context"))   context_name = config.find("context").asString();
    if (config.check("from")) file_name    = config.find("from").asString();

    yarp::os::ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultContext(context_name.c_str());
    rf.setDefaultConfigFile(file_name.c_str());

    yarp::os::Property p;
    std::string configFile = rf.findFile("from");
    if (configFile != "") p.fromConfigFile(configFile.c_str());
    yDebug() << "robotGotoDev configuration: \n" << p.toString().c_str();

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
    yInfo() << "gotoTargetByAbsoluteLocation not yet implemented";
    return true;
}

bool fakeNavigation::gotoTargetByRelativeLocation(double x, double y, double theta)
{
    yInfo() << "gotoTargetByRelativeLocation not yet implemented";
    return true;
}

bool fakeNavigation::gotoTargetByRelativeLocation(double x, double y)
{
    yInfo() << "gotoTargetByRelativeLocation not yet implemented";
    return true;
}

bool fakeNavigation::applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout)
{
    yInfo() << "applyVelocityCommand not yet implemented";
    return true;
}

bool fakeNavigation::stopNavigation()
{
    yInfo() << "stopNavigation not yet implemented";
    return true;
}

bool fakeNavigation::suspendNavigation(double time)
{
    yInfo() << "suspendNavigation not yet implemented";
    return true;
}

bool fakeNavigation::resumeNavigation()
{
    yInfo() << "resumeNavigation not yet implemented";
    return true;
}

bool fakeNavigation::getAllNavigationWaypoints(std::vector<Map2DLocation>& waypoints)
{
    yInfo() << "getAllNavigationWaypoints not yet implemented";
    return true;
}

bool fakeNavigation::getCurrentNavigationWaypoint(Map2DLocation& curr_waypoint)
{
    yInfo() << "getCurrentNavigationWaypoint not yet implemented";
    return true;
}

bool fakeNavigation::getCurrentNavigationMap(yarp::dev::NavigationMapTypeEnum map_type, MapGrid2D& map)
{
    yInfo() << "getCurrentNavigationMap not yet implemented";
    return true;
}

bool fakeNavigation::getNavigationStatus(yarp::dev::NavigationStatusEnum& status)
{
    yInfo() << "getNavigationStatus not yet implemented";
    return true;
}

bool fakeNavigation::getAbsoluteLocationOfCurrentTarget(Map2DLocation& target)
{
    yInfo() << "getAbsoluteLocationOfCurrentTarget not yet implemented";
    return true;
}

bool fakeNavigation::recomputeCurrentNavigationPath()
{
    yInfo() << "recomputeCurrentNavigationPath not yet implemented";
    return true;
}

bool fakeNavigation::getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta)
{
    yInfo() << "getRelativeLocationOfCurrentTarget not yet implemented";
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

