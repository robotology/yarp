/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_RANGEFINDER_RANGEFINDER2DWRAPPER_H
#define YARP_DEV_RANGEFINDER_RANGEFINDER2DWRAPPER_H

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

#include <yarp/os/RateThread.h>
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
#include <yarpRosHelper.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <sensor_msgs_LaserScan.h>

#define DEFAULT_DUMMY_MIN_DISTANCE      0.0             // [m]
#define DEFAULT_DUMMY_MAX_DISTANCE      100.0           // [m]

#define DEFAULT_DUMMY_MIN_ANGLE_DEG     0.0             // angular min scan range in degrees
#define DEFAULT_DUMMY_MAX_ANGLE_DEG     360.0           // angular max scan range in degrees

namespace yarp{
    namespace dev{
        class Rangefinder2DWrapper;
        }
}

#define DEFAULT_THREAD_PERIOD 20 //ms

class yarp::dev::Rangefinder2DWrapper: public yarp::os::RateThread,
                                public yarp::dev::DeviceDriver,
                                public yarp::dev::IMultipleWrapper,
                                public yarp::os::PortReader
{
public:
    Rangefinder2DWrapper();
    ~Rangefinder2DWrapper();

    bool open(yarp::os::Searchable &params);
    bool close();
    yarp::os::Bottle getOptions();

    void setId(const std::string &id);
    std::string getId();

    /**
      * Specify which sensor this thread has to read from.
      */
    bool attachAll(const PolyDriverList &p);
    bool detachAll();

    void attach(yarp::dev::IRangefinder2D *s);
    void detach();

    bool threadInit();
    void threadRelease();
    void run();

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    yarp::os::ConstString partName;
    yarp::os::ConstString streamingPortName;
    yarp::os::ConstString rpcPortName;
    yarp::os::Port rpcPort;
    yarp::os::BufferedPort<yarp::os::Bottle> streamingPort;
    yarp::dev::IRangefinder2D *sens_p;
    yarp::dev::IPreciselyTimed *iTimed;
    yarp::os::Stamp lastStateStamp;
    int _rate;
    std::string sensorId;
    double minAngle, maxAngle;
    double minDistance, maxDistance;

    bool checkROSParams(yarp::os::Searchable &config);
    bool initialize_ROS();
    bool initialize_YARP(yarp::os::Searchable &config);
    virtual bool read(yarp::os::ConnectionReader& connection);

    // ROS data
    ROSTopicUsageType                                   useROS;                     // decide if open ROS topic or not
    std::string                                         frame_id;                   // name of the frame measures are referred to
    std::string                                         rosNodeName;                // name of the rosNode
    std::string                                         rosTopicName;               // name of the rosTopic
    yarp::os::Node                                      *rosNode;                   // add a ROS node
    yarp::os::NetUint32                                 rosMsgCounter;              // incremental counter in the ROS message
    yarp::os::Publisher<sensor_msgs_LaserScan>          rosPublisherPort;           // Dedicated ROS topic publisher

#endif //DOXYGEN_SHOULD_SKIP_THIS
};

#endif
