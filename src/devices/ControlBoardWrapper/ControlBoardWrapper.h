/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPER_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPER_H


// ControlBoardWrapper
// A modified version of the remote control board class
// which remaps joints, it can also merge networks into a single part.


#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <yarp/os/BufferedPort.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Stamp.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/ControlBoardHelpers.h>

#include <yarp/dev/impl/jointData.h> // struct for YARP extended port

#include <mutex>
#include <string>
#include <vector>

// ROS state publisher
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/rosmsg/sensor_msgs/JointState.h>
#include <yarp/rosmsg/impl/yarpRosHelper.h>


#include "ControlBoardWrapperPidControl.h"
#include "ControlBoardWrapperPositionControl.h"
#include "ControlBoardWrapperPositionDirect.h"
#include "ControlBoardWrapperVelocityControl.h"
#include "ControlBoardWrapperEncodersTimed.h"
#include "ControlBoardWrapperMotor.h"
#include "ControlBoardWrapperMotorEncoders.h"
#include "ControlBoardWrapperAmplifierControl.h"
#include "ControlBoardWrapperControlLimits.h"
#include "ControlBoardWrapperRemoteCalibrator.h"
#include "ControlBoardWrapperControlCalibration.h"
#include "ControlBoardWrapperTorqueControl.h"
#include "ControlBoardWrapperImpedanceControl.h"
#include "ControlBoardWrapperControlMode.h"
#include "ControlBoardWrapperAxisInfo.h"
#include "ControlBoardWrapperInteractionMode.h"
#include "ControlBoardWrapperRemoteVariables.h"
#include "ControlBoardWrapperPWMControl.h"
#include "ControlBoardWrapperCurrentControl.h"
#include "ControlBoardWrapperPreciselyTimed.h"

#include "SubDevice.h"
#include "StreamingMessagesParser.h"
#include "RPCMessagesParser.h"


#ifdef MSVC
    #pragma warning(disable:4355)
#endif

/*
 * To optimize memory allocation, for group of joints we can have one mem reserver for rpc port
 * and on e for streaming. The size could be numOfSubDevices*maxNumOfjointForSubdevice.
 * (we could also use the actual joint number for each subdevice using a for loop). TODO
 */

class CommandsHelper;
class SubDevice;


/**
 *  @ingroup dev_impl_wrapper dev_impl_deprecated
 *
 * \brief `controlboardwrapper2` *deprecated*: An updated version of the controlBoard network wrapper.
 * It can merge together more than one control board device, or use only a
 * portion of it by remapping functionality.
 * Allows also deferred attach/detach of a subdevice.
 *
 * \section controlboardwrapper2_device_parameters Description of input parameters
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
 * | name           |      -         | string  | -              |   -           | Yes                         | full name of the port opened by the device, like /robotName/part/ | MUST start with a '/' character |
 * | period         |      -         | int     | ms             |   20          | No                          | refresh period of the broadcasted values in ms                    | optional, default 20ms |
 * | subdevice      |      -         | string  | -              |   -           | alternative to netwok group | name of the subdevice to instantiate                              | when used, parameters for the subdevice must be provided as well |
 * | networks       |      -         | group   | -              |   -           | alternative to subdevice    | this is expected to be a group parameter in xml format, a list in .ini file format. SubParameter are mandatory if this is used| - |
 * | -              | networkName_1  | 4 * int | joint number   |   -           |   if networks is used       | describe how to match subdevice_1 joints with the wrapper joints. First 2 numbers indicate first/last wrapper joint, last 2 numbers are subdevice first/last joint | The joints are intended to be consequent |
 * | -              |      ...       | 4 * int | joint number   |   -           |   if networks is used       | same as above                                                     | The joints are intended to be consequent |
 * | -              | networkName_n  | 4 * int | joint number   |   -           |   if networks is used       | same as above                                                     | The joints are intended to be consequent |
 * | -              | joints         |  int    |  -             |   -           |   if networks is used       | total number of joints handled by the wrapper                     | MUST match the sum of joints from all the networks |
 * | ROS            |      -         | group   |  -             |   -           | No                          | Group containing parameter for ROS topic initialization           | if missing, it is assumed to not use ROS topics |
 * |   -            |  useROS        | string  | true/false/only|   -           |  if ROS group is present    | set 'true' to have both yarp ports and ROS topic, set 'only' to have only ROS topic and no yarp port|  - |
 * |   -            |  ROS_topicName | string  |  -             |   -           |  if ROS group is present    | set the name for ROS topic                                        | must start with a leading '/' |
 * |   -            |  ROS_nodeName  | string  |  -             |   -           |  if ROS group is present    | set the name for ROS node                                         | must start with a leading '/' |
 * |   -            |  jointNames    | string  |  -             |   -           |  deprecated                 | joints names are now got from attached motionControl device       | names order must match with the joint order, from 0 to N |
 *
 * ROS message type used is sensor_msgs/JointState.msg (http://docs.ros.org/api/sensor_msgs/html/msg/JointState.html)
 * Some example of configuration files:
 *
 * Configuration file using .ini format, using subdevice keyword.
 *
 * \code{.unparsed}
 *  device controlboardwrapper2
 *  subdevice fakebot
 *  name /icub/head
 *
 * ** parameter for 'fakebot' subdevice follows here **
 * ...
 * \endcode
 *
 * XML format, using 'networks' keyword. This file is meant to be used in junction with yarprobotinterface executable,
 * therefore has an addictional section at the end.
 *
 * \code{.xml}
 * <paramlist name="networks">
 *   <!-- elem name hereafter are custom names that live only in this file, they are used in the attach phase -->
 *   <elem name="FirstSetOfJoints">  (0  3  0  3)  </elem>
 *   <elem name="SecondSetOfJoints"> (4  6  0  2)  </elem>
 * </paramlist>
 *
 * <param name="period"> 20                 </param>
 * <param name="name">   /icub/left_arm     </param>
 * <param name="joints"> 7                  </param>
 *
 *
 *  <!-- Following parameters are meaningful ONLY for yarprobotinterface -->
 *  <action phase="startup" level="5" type="attach">
 *      <paramlist name="networks">
 *          <!-- The param value must match the device name in the corresponding emsX file -->
 *          <elem name="FirstSetOfJoints">  left_upper_arm_mc </elem>
 *          <elem name="SecondSetOfJoints"> left_lower_arm_mc </elem>
 *       </paramlist>
 *  </action>
 *  <action phase="shutdown" level="5" type="detach" />
 * \endcode
 *
 * Configuration file using .ini format, using network keyword
 *
 * \code{.unparsed}
 *  device controlboardwrapper2
 *  name  /robotName/partName
 *  period 10
 *  networks (net_larm net_lhand)
 *  joints 16
 *  net_larm    0 3  0 3
 *  net_lhand   4 6  0 2
 * \endcode
 *
 * Configuration for ROS topic using .ini format
 * \code{.unparsed}
 * [ROS]
 * useROS         true
 * ROS_topicName  /JointState
 * ROS_nodeName   /robotPublisher
 * jointNames     r_shoulder_pitch r_shoulder_roll r_shoulder_yaw r_elbow r_wrist_prosup r_wrist_pitch r_wrist_yaw
 * \endcode
 *
 * Configuration for ROS topic using .xml format
 * \code{.unparsed}
 * <group name="ROS">
 *     <param name="useROS">         true             </param>    // use 'only' if you want only ROS topic and NOT yarp port
 *     <param name="ROS_topicName">  /JointState      </param>
 *     <param name="ROS_nodeName">   /robotPublisher  </param>
 *     <param name="jointNames">     r_shoulder_pitch r_shoulder_roll r_shoulder_yaw r_elbow r_wrist_prosup r_wrist_pitch r_wrist_yaw </param>
 * </group>
 * \endcode
 */

class ControlBoardWrapper :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public yarp::dev::IMultipleWrapper,
        virtual public ControlBoardWrapperCommon,
        public ControlBoardWrapperPidControl,
        public ControlBoardWrapperPositionControl,
        public ControlBoardWrapperPositionDirect,
        public ControlBoardWrapperVelocityControl,
        public ControlBoardWrapperEncodersTimed,
        public ControlBoardWrapperMotor,
        public ControlBoardWrapperMotorEncoders,
        public ControlBoardWrapperAmplifierControl,
        public ControlBoardWrapperControlLimits,
        public ControlBoardWrapperRemoteCalibrator,
        public ControlBoardWrapperControlCalibration,
        public ControlBoardWrapperTorqueControl,
        public ControlBoardWrapperImpedanceControl,
        public ControlBoardWrapperControlMode,
        public ControlBoardWrapperAxisInfo,
        public ControlBoardWrapperPreciselyTimed,
        public ControlBoardWrapperInteractionMode,
        public ControlBoardWrapperRemoteVariables,
        public ControlBoardWrapperPWMControl,
        public ControlBoardWrapperCurrentControl
{
private:
    std::string rootName;

    bool checkPortName(yarp::os::Searchable &params);

    yarp::rosmsg::sensor_msgs::JointState ros_struct;

    yarp::os::BufferedPort<yarp::sig::Vector>  outputPositionStatePort;   // Port /state:o streaming out the encoder positions
    yarp::os::BufferedPort<CommandMessage>     inputStreamingPort;        // Input streaming port for high frequency commands
    yarp::os::Port inputRPCPort;                // Input RPC port for set/get remote calls
    yarp::sig::Vector times;                    // time for each joint

    // Buffer associated to the extendedOutputStatePort port; in this case we will use the type generated
    // from the YARP .thrift file
    yarp::os::PortWriterBuffer<yarp::dev::impl::jointData>           extendedOutputState_buffer;
    yarp::os::Port extendedOutputStatePort;         // Port /stateExt:o streaming out the struct with the robot data

    // ROS state publisher
    ROSTopicUsageType                                   useROS {ROS_disabled};               // decide if open ROS topic or not
    std::vector<std::string>                            jointNames;                          // name of the joints
    std::string                                         rosNodeName;                         // name of the rosNode
    std::string                                         rosTopicName;                        // name of the rosTopic
    yarp::os::Node                                      *rosNode {nullptr};                  // add a ROS node
    yarp::os::NetUint32                                 rosMsgCounter {0};                   // incremental counter in the ROS message
    yarp::os::PortWriterBuffer<yarp::rosmsg::sensor_msgs::JointState> rosOutputState_buffer; // Buffer associated to the ROS topic
    yarp::os::Publisher<yarp::rosmsg::sensor_msgs::JointState> rosPublisherPort;             // Dedicated ROS topic publisher

    yarp::os::PortReaderBuffer<yarp::os::Bottle>    inputRPC_buffer;                // Buffer associated to the inputRPCPort port
    RPCMessagesParser              RPC_parser;                     // Message parser associated to the inputRPCPort port
    StreamingMessagesParser        streaming_parser;               // Message parser associated to the inputStreamingPort port

    static constexpr double default_period = 0.02; // s
    double period {default_period};

    yarp::os::Bottle getOptions();
    bool updateAxisName();
    bool checkROSParams(yarp::os::Searchable &config);
    bool initialize_ROS();
    bool initialize_YARP(yarp::os::Searchable &prop);
    void cleanup_yarpPorts();

    // Default usage
    // Open the wrapper only, the attach method needs to be called before using it
    bool openDeferredAttach(yarp::os::Property& prop);

    // For the simulator, if a subdevice parameter is given to the wrapper, it will
    // open it and attach to it immediately.
    yarp::dev::PolyDriver *subDeviceOwned {nullptr};
    bool openAndAttachSubDevice(yarp::os::Property& prop);

    bool ownDevices {true};

    void calculateMaxNumOfJointsInDevices();

public:
    ControlBoardWrapper();
    ControlBoardWrapper(const ControlBoardWrapper&) = delete;
    ControlBoardWrapper(ControlBoardWrapper&&) = delete;
    ControlBoardWrapper& operator=(const ControlBoardWrapper&) = delete;
    ControlBoardWrapper& operator=(ControlBoardWrapper&&) = delete;
    ~ControlBoardWrapper() override;


    /* Return id of this device */
    std::string getId()
    {
        return partName;
    }

    /**
    * Close the device driver by deallocating all resources and closing ports.
    * @return true if successful or false otherwise.
    */
    bool close() override;


    /**
    * Open the device driver.
    * @param prop is a Searchable object which contains the parameters.
    * Allowed parameters are:
    * - name to specify the prefix of the port names.
    * - subdevice [optional] if specified, the openAndAttachSubDevice will be
    *             called, otherwise openDeferredAttach is called.
    * and all parameters required by the wrapper.
    */
    bool open(yarp::os::Searchable& prop) override;

    bool detachAll() override;

    bool attachAll(const yarp::dev::PolyDriverList &l) override;

    /**
    * The thread main loop deals with writing on ports here.
    */
    void run() override;
};


#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPER_H
