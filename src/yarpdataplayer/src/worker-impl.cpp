/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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

#if defined(_WIN32)
    #pragma warning (disable : 4099)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include "include/utils.h"
#include <yarp/rosmsg/sensor_msgs/LaserScan.h>
#include <yarp/rosmsg/nav_msgs/Odometry.h>
#include <yarp/rosmsg/tf/tfMessage.h>
#include <yarp/rosmsg/tf2_msgs/TFMessage.h>
#include <yarp/rosmsg/geometry_msgs/Pose.h>
#include <yarp/rosmsg/geometry_msgs/Pose2D.h>

template <class T>
int WorkerClass::sendGenericData(int part, int id)
{
    yarp::os::Bottle tmp;
    if (utilities->withExtraColumn) {
        tmp = utilities->partDetails[part].bot.get(id).asList()->tail().tail().tail();
    }
    else {
        tmp = utilities->partDetails[part].bot.get(id).asList()->tail().tail();
    }

    yarp::os::BufferedPort<T>* the_port = dynamic_cast<yarp::os::BufferedPort<T>*> (utilities->partDetails[part].outputPort);
    if (the_port == nullptr) { LOG_ERROR("dynamic_cast failed"); return -1; }

    auto& dat = the_port->prepare();
    yarp::os::Portable::copyPortable(tmp, dat);

    //propagate timestamp
    yarp::os::Stamp ts(id, utilities->partDetails[part].timestamp[id]);
    the_port->setEnvelope(ts);

    if (utilities->sendStrict) {
        the_port->writeStrict();
    }
    else {
        the_port->write();
    }
    return 0;
}


template int WorkerClass::sendGenericData<yarp::os::Bottle>(int,int);
template int WorkerClass::sendGenericData<yarp::rosmsg::sensor_msgs::LaserScan>(int, int);
template int WorkerClass::sendGenericData<yarp::rosmsg::nav_msgs::Odometry>(int, int);
template int WorkerClass::sendGenericData<yarp::rosmsg::tf2_msgs::TFMessage>(int, int);
template int WorkerClass::sendGenericData<yarp::rosmsg::geometry_msgs::Pose>(int, int);
template int WorkerClass::sendGenericData<yarp::rosmsg::geometry_msgs::Pose2D>(int, int);
