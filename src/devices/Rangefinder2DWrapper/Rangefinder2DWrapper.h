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

#ifndef YARP_DEV_RANGEFINDER2DWRAPPER_RANGEFINDER2DWRAPPER_H
#define YARP_DEV_RANGEFINDER2DWRAPPER_RANGEFINDER2DWRAPPER_H

 //#include <list>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/api.h>
#include <yarp/dev/PreciselyTimed.h>

// ROS state publisher
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/rosmsg/sensor_msgs/LaserScan.h>
#include <yarp/rosmsg/impl/yarpRosHelper.h>


#define DEFAULT_THREAD_PERIOD 0.02 //s

class Rangefinder2DWrapper:
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::IMultipleWrapper,
        public yarp::os::PortReader
{
public:
    Rangefinder2DWrapper();
    ~Rangefinder2DWrapper();

    bool open(yarp::os::Searchable &params) override;
    bool close() override;
    yarp::os::Bottle getOptions();

    void setId(const std::string &id);
    std::string getId();

    /**
      * Specify which sensor this thread has to read from.
      */
    bool attachAll(const yarp::dev::PolyDriverList &p) override;
    bool detachAll() override;

    void attach(yarp::dev::IRangefinder2D *s);
    void detach();

    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
    yarp::dev::PolyDriver driver;
    std::string partName;
    std::string streamingPortName;
    std::string rpcPortName;
    yarp::os::Port rpcPort;
    yarp::os::BufferedPort<yarp::os::Bottle> streamingPort;
    yarp::dev::IRangefinder2D *sens_p;
    yarp::dev::IPreciselyTimed *iTimed;
    yarp::os::Stamp lastStateStamp;
    double _period;
    std::string sensorId;
    double minAngle, maxAngle;
    double minDistance, maxDistance;
    double resolution;
    bool   isDeviceOwned;

    bool checkROSParams(yarp::os::Searchable &config);
    bool initialize_ROS();
    bool initialize_YARP(yarp::os::Searchable &config);
    bool read(yarp::os::ConnectionReader& connection) override;

    // ROS data
    ROSTopicUsageType                                   useROS;                     // decide if open ROS topic or not
    std::string                                         frame_id;                   // name of the frame measures are referred to
    std::string                                         rosNodeName;                // name of the rosNode
    std::string                                         rosTopicName;               // name of the rosTopic
    yarp::os::Node                                      *rosNode;                   // add a ROS node
    yarp::os::NetUint32                                 rosMsgCounter;              // incremental counter in the ROS message
    yarp::os::Publisher<yarp::rosmsg::sensor_msgs::LaserScan> rosPublisherPort;     // Dedicated ROS topic publisher
};

#endif // YARP_DEV_RANGEFINDER2DWRAPPER_RANGEFINDER2DWRAPPER_H
