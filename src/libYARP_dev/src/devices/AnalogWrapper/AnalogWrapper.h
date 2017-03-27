/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_ANALOGWRAPPER_ANALOGWRAPPER_H
#define YARP_DEV_ANALOGWRAPPER_ANALOGWRAPPER_H

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
 * It reads the data from an analog sensor and sends them on one or more ports.
 * It creates one rpc port and its related handler for every output port..
 *
 *
 * Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
 * | name           |      -         | string  | -              |   -           | Yes                         | full name of the port opened by the device, like /robotName/part/ | MUST start with a '/' character |
 * | period         |      -         | int     | ms             |   20          | No                          | refresh period of the broadcasted values in ms                    | optional, default 20ms |
 * | subdevice      |      -         | string  | -              |   -           | alternative to netwok group | name of the subdevice to instantiate                              | when used, parameters for the subdevice must be provided as well |
 * | ports          |      -         | group   | -              |   -           | alternative to subdevice    | this is expected to be a group parameter in xml format, a list in .ini file format. SubParameter are mandatory if this is used| - |
 * | -              | portName_1     | 4 * int | channel number |   -           |   if ports is used          | describe how to match subdevice_1 channels with the wrapper channels. First 2 numbers indicate first/last wrapper channel, last 2 numbers are subdevice first/last channel | The channels are intended to be consequent |
 * | -              |      ...       | 4 * int | channel number |   -           |   if ports is used          | same as above                                                     | The channels are intended to be consequent |
 * | -              | portName_n     | 4 * int | channel number |   -           |   if ports is used          | same as above                                                     | The channels are intended to be consequent |
 * | -              | channels       |  int    |  -             |   -           |   if ports is used          | total number of channels handled by the wrapper                   | MUST match the sum of channels from all the ports |
 * | ROS            |      -         | group   |  -             |   -           | No                          | Group containing parameter for ROS topic initialization           | if missing, it is assumed to not use ROS topics |
 * |   -            |  useROS        | string  | true/false/only|   -           |  if ROS group is present    | set 'true' to have both yarp ports and ROS topic, set 'only' to have only ROS topic and no yarp port|  - |
 * |   -            |  ROS_topicName | string  |  -             |   -           |  if ROS group is present    | set the name for ROS topic                                        | must start with a leading '/' |
 * |   -            |  ROS_nodeName  | string  |  -             |   -           |  if ROS group is present    | set the name for ROS node                                         | must start with a leading '/' |
 * |   -            |  channelNames  | string  |  -             |   -           |  deprecated                 | channels names are now got from attached motionControl device     | names order must match with the channel order, from 0 to N |
 *
 * ROS message type used for force/torque is geometry_msgs/WrenchedStamped.msg (http://docs.ros.org/indigo/api/geometry_msgs/html/msg/WrenchStamped.html)
 *
 * Some example of configuration files:
 *
 * Configuration file using .ini format, using subdevice keyword.
 *
 * \code{.unparsed}
 *  device analogServer
 *  subdevice fakeAnalogSensor
 *  name /myAnalogSensor
 *
 * ** parameter for 'fakeAnalogSensor' subdevice follows here **
 * ...
 * \endcode
 *
 * Configuration file using .ini format, using ports keyword
 *
 * \code{.unparsed}
 *  device analogServer
 *  name  /myAnalogServer
 *  period 20
 *  ports (FirstSetOfChannels SecondSetOfChannels ThirdSetOfChannels)
 *  channels 1344
 *  FirstSetOfChannels 	0   191  0 191
 *  SecondSetOfChannels 192 575  0 383
 *  ThirdSetOfChannels 	576 1343 0 767
 *
 * \endcode
 *
 * Configuration file using .xml format.
 *
 * \code{.xml}
 *	<device name="/myAnalogServer" type="analogServer">
 *		<param name="period">   20  			    </param>
 *		<param name="channels"> 1344				</param>
 *
 *		<paramlist name="ports">
 *		  <elem name="FirstSetOfChannels"> 	0   191  0 191</elem>
 *		  <elem name="SecondSetOfChannels"> 192 575  0 383</elem>
 *		  <elem name="ThirdSetOfChannels"> 	576 1343 0 767</elem>
 *		</paramlist>
 *
 *		<action phase="startup" level="5" type="attach">
 *		    <paramlist name="networks">
 *		<!-- The param value must match the device name in the corresponding analogSensor config file.
 *      AnalogWrapper is able to attach to only one subdevice. -->
 *		        <elem name="myAnalogSensor">  my_analog_sensor </elem>
 *		    </paramlist>
 *		</action>
 *
 *		<action phase="shutdown" level="5" type="detach" />
 *	</device>
 * \endcode
 *
 * Configuration for ROS topic using .ini format.
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
 * Configuration for ROS topic using .xml format.
 *
 * \code{.unparsed}
 *
 * <group name="ROS">
 *     <param name="useROS">         true                           </param>    // use 'only' if you want only ROS topic and NOT yarp ports
 *     <param name="ROS_topicName">  /left_arm/forceTorque          </param>
 *     <param name="ROS_nodeName">   /torquePublisher               </param>
 *     <param name="ROS_msgType">    geometry_msgs/WrenchedStamped  </param>
 *     <param name="frame_id">       r_shoulder                     </param>
 * </group>
 * \endcode
 * */



class yarp::dev::AnalogWrapper: public yarp::os::RateThread,
                                public yarp::dev::DeviceDriver,
                                public yarp::dev::IMultipleWrapper
{
public:
    // Constructor used by yarp factory
    AnalogWrapper();

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
    // Constructor used when there is only one output port  -- obsolete, here for backward compatibility with skinwrapper
    /** @deprecated since YARP 2.3.70 */
#if !defined(_MSC_VER) || _MSC_VER != 1900
    // For some unknown reason, Visual studio 2015 fails with this error:
    // "C2416 attribute 'deprecated' cannot be applied in this context"
    YARP_DEPRECATED
#endif
    AnalogWrapper(const char* name, int rate=20);

    // Contructor used when one or more output ports are specified  -- obsolete, here for backward compatibility with skinwrapper
    /** @deprecated since YARP 2.3.70 */
#if !defined(_MSC_VER) || _MSC_VER != 1900
    // For some unknown reason, Visual studio 2015 fails with this error:
    // "C2416 attribute 'deprecated' cannot be applied in this context"
    YARP_DEPRECATED
#endif
    AnalogWrapper(const std::vector<yarp::dev::impl::AnalogPortEntry>& _analogPorts, int rate=20);
#endif // YARP_NO_DEPRECATED

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
#endif // DOXYGEN_SHOULD_SKIP_THIS
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
#endif // DOXYGEN_SHOULD_SKIP_THIS


#endif // YARP_DEV_ANALOGWRAPPER_ANALOGWRAPPER_H
