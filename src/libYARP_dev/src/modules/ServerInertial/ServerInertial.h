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
 * 0  1   2  = Euler orientation data (Kalman filter processed)
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
    yarp::dev::PolyDriver poly;
    IGenericSensor *IMU; //The inertial device
    IPreciselyTimed *iTimed;
    double period;
    yarp::os::Port p;
    yarp::os::PortWriterBuffer<yarp::os::Bottle> writer;
    int prev_timestamp_counter;
    int curr_timestamp_counter;
    int trap;

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

