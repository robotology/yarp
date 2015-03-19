// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Alexis Maldonado, Radu Bogdan Rusu
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef _YARP2_SERVERINERTIAL_
#define _YARP2_SERVERINERTIAL_

#include <stdio.h>

#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/GenericSensorInterfaces.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/PreciselyTimed.h>

// ROS state publisher
#include <yarpRosHelper.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <sensor_msgs_Imu.h>

namespace yarp
{
    namespace dev
    {
        class ServerInertial;
    }
}


/**
 * @ingroup dev_impl_wrapper
 *
 * Export an inertial sensor.
 * The network interface is a single Port.
 * We will stream bottles with 12 floats:
 * 0  1   2  = Euler orientation data  XYZ global frame representation.
 * 3  4   5  = Calibrated 3-axis (X, Y, Z) acceleration data
 * 6  7   8  = Calibrated 3-axis (X, Y, Z) gyroscope data
 * 9 10 11   = Calibrated 3-axis (X, Y, Z) magnetometer data
 *
 * @author Alexis Maldonado, Radu Bogdan Rusu
 */

/**
 *  ROS message type used is sensor_msgs/Imu.msg
 */

class yarp::dev::ServerInertial : public DeviceDriver,
            private yarp::os::Thread,
            public yarp::os::PortReader,
            public yarp::dev::IGenericSensor
{
private:
    bool spoke;
    yarp::os::ConstString partName;
    yarp::dev::PolyDriver poly;
    IGenericSensor *IMU; //The inertial device
    IPreciselyTimed *iTimed;
    double period;
    yarp::os::Port p;
    yarp::os::PortWriterBuffer<yarp::os::Bottle> writer;
    int prev_timestamp_counter;
    int curr_timestamp_counter;
    int trap;

    // ROS data
    ROSTopicUsageType                                   useROS;                     // decide if open ROS topic or not
    std::string                                         frame_id;                   // name of the frame mesuares are referred to
    std::string                                         rosNodeName;                // name of the rosNode
    std::string                                         rosTopicName;               // name of the rosTopic
    yarp::os::Node                                      *rosNode;                   // add a ROS node
    yarp::os::NetUint32                                 rosMsgCounter;              // incremental counter in the ROS message
    yarp::os::Publisher<sensor_msgs_Imu>                rosPublisherPort;           // Dedicated ROS topic publisher
//    sensor_msgs_Imu                                     rosData;                    // store imu data in ROS format to be published
    std::vector<yarp::os::NetFloat64>                   covariance;                 // empty matrix to store covariance data needed by ROS msg

    bool checkROSParams(yarp::os::Searchable &config);
    bool initialize_ROS();
    bool initialize_YARP(yarp::os::Searchable &prop);

public:
    /**
     * Constructor.
     */
    ServerInertial();

    virtual ~ServerInertial();

    /**
     * Configure with a set of options. These are:
     * <TABLE>
     * <TR><TD> subdevice </TD><TD> Common name of device to wrap (e.g. "test_grabber"). </TD></TR>
     * <TR><TD> name </TD><TD> Port name to assign to this server (default /grabber). </TD></TR>
     * </TABLE>
     *
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    virtual bool open(yarp::os::Searchable& config);

    virtual bool close();

    virtual bool getInertial(yarp::os::Bottle &bot);

    virtual void run();

    virtual bool read(ConnectionReader& connection);

    virtual bool read(yarp::sig::Vector &out);

    virtual bool getChannels(int *nc);

    virtual bool calibrate(int ch, double v);
};

#endif

