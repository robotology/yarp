// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef BATTERY_SERVER_H_
#define BATTERY_SERVER_H_

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

#include <yarp/dev/IBattery.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/api.h>


// ROS state publisher
#include <yarpRosHelper.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>

/* Using yarp::dev::impl namespace for all helper class inside yarp::dev to reduce
 * name conflicts
 */

namespace yarp{
    namespace dev{
        class BatteryWrapper;
        }
}

#define DEFAULT_THREAD_PERIOD 20 //ms

/**
 *  @ingroup dev_impl_wrapper
 *
 * \section analogWrapper_parameter Description of input parameters
 *
 *  It reads the data from an analog sensor and sends them on one or more ports.
 *  It creates one rpc port and its related handler for every output port..
 *
 * Parameters accepted in the config argument of the open method:
 * | Parameter name | Type    | Units          | Default Value | Required  | Description   | Notes |
 * |:--------------:|:------: |:--------------:|:-------------:|:--------: |:-------------:|:-----:|
 * | name           | string  | -              | -             | Yes       | full name of the port opened by the device, like /robotName/deviceId/sensorType:o | must start with a '/' character |
 * | period         | int     | ms             | 20            | No        | refresh period of the broadcasted values in ms (optional, default 20ms) | - |
 * | ROS            | group   |  -             |   -           | No                          | Group containing parameter for ROS topic initialization           | if missing, it is assumed to not use ROS topics |
 * |  useROS        | string  | true/false/only|   -           |  if ROS group is present    | set 'true' to have both yarp ports and ROS topic, set 'only' to have only ROS topic and no yarp port|  - |
 * |  ROS_TopicName | string  |  -             |   -           |  if ROS group is present    | set the name for ROS topic                                        | must start with a leading '/' |
 * |  ROS_nodeName  | string  |  -             |   -           |  if ROS group is present    | set the name for ROS node                                         | must start with a leading '/' |
 * |  ROS_msgType   | string  |  enum          |   -           |  if ROS group is present    | choose the message to be sent through ROS topic                   | supported value now is ONLY geometry_msgs/WrenchedStamped |
 * |  frame_id      | string  |  -             |   -           |  if ROS group is present    | name of reference frame the measures are referred to              | - |
 *
 * ROS message type used for force/torque is geometry_msgs/WrenchedStamped.msg (http://docs.ros.org/indigo/api/geometry_msgs/html/msg/WrenchStamped.html)
 *
 * Some example of configuration files:
 *
 * Configuration file using .ini format
 *
 * \code{.unparsed}
 * [ROS]
 * useROS        true
 * ROS_topicName /left_arm/forceTorque
 * ROS_nodeName  /torquePublisher
 * ROS_msgType   geometry_msgs/WrenchedStamped
 * frame_id      r_shoulder
 * \endcode
 *
 * Configuration file using .xml format.
 *
 * \code{.unparsed}
 *
 *  * <group name="ROS">
 *     <param name="useROS">         true                           </param>    // use 'only' if you want only ROS topic and NOT yarp ports
 *     <param name="ROS_topicName">  /left_arm/forceTorque          </param>
 *     <param name="ROS_nodeName">   /torquePublisher               </param>
 *     <param name="ROS_msgType">    geometry_msgs/WrenchedStamped  </param>
 *     <param name="frame_id">       r_shoulder                     </param>
 * </group>
 * \endcode
 */



class yarp::dev::BatteryWrapper: public yarp::os::RateThread,
                                public yarp::dev::DeviceDriver,
                                public yarp::dev::IMultipleWrapper
{
public:
    // Constructor used by yarp factory
    BatteryWrapper();

    // Constructor used when there is only one output port  -- obsolete, here for backward compatibility with skinwrapper
    BatteryWrapper(const char* name, int rate = 20);

    ~BatteryWrapper();

    bool open(yarp::os::Searchable &params);
    bool close();
    yarp::os::Bottle getOptions();

    void setId(const std::string &id);
    std::string getId();

    /**
      * Specify which analog sensor this thread has to read from.
      */
    bool attachAll(const PolyDriverList &p);
    bool detachAll();

    void attach(yarp::dev::IBattery *s);
    void detach();

    bool threadInit();
    void threadRelease();
    void run();

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    yarp::os::ConstString streamingPortName;
    yarp::os::ConstString rpcPortName;
    yarp::os::Port rpcPort;
    yarp::os::BufferedPort<yarp::os::Bottle> streamingPort;
    yarp::dev::IBattery *battery_p;             // the battery read from
    yarp::os::Stamp lastStateStamp;             // the last reading time stamp
    int _rate;

    // deprecated params
    std::string robotName;
    std::string sensorId;
    std::string sensorType;
    // END deprecated params

    // ROS state publisher
    ROSTopicUsageType                                        useROS;                     // decide if open ROS topic or not
    std::string                                              frame_id;                   // name of the reference frame the measures are referred to
    std::string                                              rosNodeName;                // name of the rosNode
    std::string                                              rosTopicName;               // name of the rosTopic
    yarp::os::Node                                           *rosNode;                   // add a ROS node
    yarp::os::NetUint32                                      rosMsgCounter;              // incremental counter in the ROS message
    // TODO: in the future, in order to support multiple ROS msgs this should be a pointer allocated dynamically depending on the msg maybe (??)

#if ROS_PART_STILL_TO_BE_DONE
    yarp::os::PortWriterBuffer<???>                          rosOutputState_buffer;      // Buffer associated to the ROS topic
    yarp::os::Publisher<???>                                 rosPublisherPort;           // Dedicated ROS topic publisher
    bool checkROSParams(yarp::os::Searchable &config);
    bool initialize_ROS();
#endif

    bool initialize_YARP(yarp::os::Searchable &config);
    
    // Function used when one or more output ports are specified
    bool checkForDeprecatedParams(yarp::os::Searchable &params);
#endif //DOXYGEN_SHOULD_SKIP_THIS
};

#endif
