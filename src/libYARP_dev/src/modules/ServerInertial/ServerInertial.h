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
 *  @ingroup yarp_dev_modules
 * @defgroup serverInertial serverInertial
 * @{
 * Export an inertial sensor.
 * The network interface is a single Port.
 * We will stream bottles with 12 floats:
 * \code{.unparsed}
 * 0  1   2  = Euler orientation data (X, Y, Z)  global frame representation.
 * 3  4   5  = Calibrated 3-axis (X, Y, Z) acceleration data
 * 6  7   8  = Calibrated 3-axis (X, Y, Z) gyroscope data
 * 9 10  11  = Calibrated 3-axis (X, Y, Z) magnetometer data
 * \endcode
 *
 * @author Alexis Maldonado, Radu Bogdan Rusu
 *
 *
 *  It reads the data from an Inertial measurement unit sensor and sends them through yarp port.
 *
 * Parameters accepted in the config argument of the open method:
 * | Parameter name | Type    | Units          | Default Value | Required  | Description   | Notes |
 * |:--------------:|:------: |:--------------:|:-------------:|:--------: |:-------------:|:-----:|
 * | name           | string  |  -             |   -           | Yes       | full name of the port opened by the device, like /robotName/deviceId/sensorType:o | must start with a '/' character |
 * | period         | int     | ms             |   5           | No        | refresh period of the broadcasted values in ms (optional, default 20ms) | - |
 * | subdevice      | string  |  -             |   -           | Yes       | name of the IM device to be instantiated | - |
 * | ROS            | group   |  -             |   -           | No                          | Group containing parameter for ROS topic initialization           | if missing, it is assumed to not use ROS topics |
 * |  useROS        | string  | true/false/only|   -           |  if ROS group is present    | set 'true' to have both yarp ports and ROS topic, set 'only' to have only ROS topic and no yarp port|  - |
 * |  ROS_TopicName | string  |  -             |   -           |  if ROS group is present    | set the name for ROS topic                                        | must start with a leading '/' |
 * |  ROS_nodeName  | string  |  -             |   -           |  if ROS group is present    | set the name for ROS node                                         | must start with a leading '/' |
 * |  ROS_msgType   | string  |  enum          |   -           |  if ROS group is present    | choose the message to be sent through ROS topic                   | supported value now is ONLY geometry_msgs/WrenchedStamped |
 * |  frame_id      | string  |  -             |   -           |  if ROS group is present    | name of reference frame the measures are referred to              | - |
 *
 *  ROS message type used is sensor_msgs/Imu.msg (http://docs.ros.org/api/sensor_msgs/html/msg/Imu.html)
 *
 * Some example of configuration files:
 *
 * Configuration file using .ini format
 *
 * \code{.unparsed}
 * name         /inertial
 * period       20
 * [ROS]
 * useROS        true
 * ROS_topicName /ROSinertial
 * ROS_nodeName  /IMUPublisher
 * frame_id      r_shoulder
 * \endcode
 *
 * Configuration file using .xml format.
 *
 * \code{.unparsed}
 *
 * <param name="name">              /inertial                   </param>
 * <param name="period">            20                          </param>
 * <group name="ROS">
 *     <param name="useROS">         true                       </param>    // use 'only' if you want only ROS topic and NOT yarp ports
 *     <param name="ROS_topicName">  /ROSinertial               </param>
 *     <param name="ROS_nodeName">   /IMUPublisher              </param>
 *     <param name="frame_id">       r_shoulder                 </param>
 * </group>
 * \endcode
 *
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
     * Open the device driver.
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

/// @}

#endif

