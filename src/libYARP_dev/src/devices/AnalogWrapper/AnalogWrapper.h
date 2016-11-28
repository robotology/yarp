/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef ANALOG_SERVER_H_
#define ANALOG_SERVER_H_

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

#include <yarp/dev/IAnalogSensor.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/api.h>


// ROS state publisher
#include <yarpRosHelper.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <geometry_msgs_WrenchStamped.h>  // Defines ROS jointState msg; it already includes TickTime and Header
#include <sensor_msgs_JointState.h>


/* Using yarp::dev::impl namespace for all helper class inside yarp::dev to reduce
 * name conflicts
 */

namespace yarp{
    namespace dev{
        class AnalogWrapper;
        namespace impl{
            class AnalogServerHandler;
            class AnalogPortEntry;
        }
    }
}

#define DEFAULT_THREAD_PERIOD 20 //ms

/**
 *  @ingroup dev_impl_wrapper
 *
 * \brief Device that expose an AnalogSensor (using the IAnalogSensor interface) on the YARP or ROS network.
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



class yarp::dev::AnalogWrapper: public yarp::os::RateThread,
                                public yarp::dev::DeviceDriver,
                                public yarp::dev::IMultipleWrapper
{
public:
    // Constructor used by yarp factory
    AnalogWrapper();

    // Constructor used when there is only one output port  -- obsolete, here for backward compatibility with skinwrapper
    AnalogWrapper(const char* name, int rate=20);

    // Contructor used when one or more output ports are specified  -- obsolete, here for backward compatibility with skinwrapper
    AnalogWrapper(const std::vector<yarp::dev::impl::AnalogPortEntry>& _analogPorts, int rate=20);

    ~AnalogWrapper();

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

    void attach(yarp::dev::IAnalogSensor *s);
    void detach();

    bool threadInit();
    void threadRelease();
    void run();

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    yarp::os::ConstString streamingPortName;
    yarp::os::ConstString rpcPortName;
    yarp::dev::IAnalogSensor *analogSensor_p;   // the analog sensor to read from
    std::vector<yarp::dev::impl::AnalogPortEntry> analogPorts;   // the list of output ports
    std::vector<yarp::dev::impl::AnalogServerHandler*> handlers; // the list of rpc port handlers
    yarp::os::Stamp lastStateStamp;             // the last reading time stamp
    yarp::sig::Vector lastDataRead;             // the last vector of data read from the attached IAnalogSensor
    int _rate;
    std::string sensorId;

    // ROS state publisher
    ROSTopicUsageType                                        useROS;                     // decide if open ROS topic or not
    std::string                                              frame_id;                   // name of the reference frame the measures are referred to
    std::vector<std::string>                                 ros_joint_names;
    std::string                                              rosMsgType;                 // ros message type
    std::string                                              rosNodeName;                // name of the rosNode
    std::string                                              rosTopicName;               // name of the rosTopic
    yarp::os::Node                                           *rosNode;                   // add a ROS node
    yarp::os::NetUint32                                      rosMsgCounter;              // incremental counter in the ROS message

    // TODO: in the future, in order to support multiple ROS msgs this should be a pointer allocated dynamically depending on the msg maybe (??)
    //  yarp::os::PortWriterBuffer<geometry_msgs_WrenchStamped>  rosOutputWrench_buffer;      // Buffer associated to the ROS topic
    yarp::os::Publisher<geometry_msgs_WrenchStamped>         rosPublisherWrenchPort;      // Dedicated ROS topic publisher

    //yarp::os::PortWriterBuffer<sensor_msgs_JointState>       rosOutputJoint_buffer;       // Buffer associated to the ROS topic
    yarp::os::Publisher<sensor_msgs_JointState>              rosPublisherJointPort;       // Dedicated ROS topic publisher


    bool ownDevices;
    // Open the wrapper only, the attach method needs to be called before using it
    bool openDeferredAttach(yarp::os::Searchable &prop);

    // For the simulator, if a subdevice parameter is given to the wrapper, it will
    // open it and and attach to it immediatly.
    yarp::dev::PolyDriver *subDeviceOwned;
    bool openAndAttachSubDevice(yarp::os::Searchable &prop);

    bool checkROSParams(yarp::os::Searchable &config);
    bool initialize_ROS();
    bool initialize_YARP(yarp::os::Searchable &config);

    void setHandlers();
    void removeHandlers();

    // Function used when there is only one output port
    bool createPort(const char* name, int rate=20);
    // Function used when one or more output ports are specified
    bool createPorts(const std::vector<yarp::dev::impl::AnalogPortEntry>& _analogPorts, int rate=20);
    bool checkForDeprecatedParams(yarp::os::Searchable &params);
#endif //DOXYGEN_SHOULD_SKIP_THIS
};


#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
  * Handler of the rpc port related to an analog sensor.
  * Manage the calibration command received on the rpc port.
 **/
class yarp::dev::impl::AnalogServerHandler: public yarp::os::PortReader
{
    yarp::dev::IAnalogSensor* is;   // analog sensor to calibrate, when required
    yarp::os::Port rpcPort;         // rpc port related to the analog sensor

public:
    AnalogServerHandler(const char* n);
    ~AnalogServerHandler();

    void setInterface(yarp::dev::IAnalogSensor *is);

    bool _handleIAnalog(yarp::os::Bottle &cmd, yarp::os::Bottle &reply);

    virtual bool read(yarp::os::ConnectionReader& connection);
};


/**
  * A yarp port that output data read from an analog sensor.
  * It contains information about which data of the analog sensor are sent
  * on the port, i.e. an offset and a length.
  */
class yarp::dev::impl::AnalogPortEntry
{
public:
    yarp::os::BufferedPort<yarp::sig::Vector> port;
    std::string port_name;      // the complete name of the port
    int offset;                 // an offset, the port is mapped starting from this taxel
    int length;                 // length of the output vector of the port (-1 for max length)
    AnalogPortEntry();
    AnalogPortEntry(const AnalogPortEntry &alt);
    AnalogPortEntry &operator =(const AnalogPortEntry &alt);
};
#endif


#endif
