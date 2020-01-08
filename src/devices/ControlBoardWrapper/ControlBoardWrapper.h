/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPER_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPER_H


// ControlBoardWrapper
// A modified version of the remote control board class
// which remaps joints, it can also merge networks into a single part.
//

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/ControlBoardHelpers.h>

#include <mutex>
#include <string>
#include <vector>

#include <yarp/dev/impl/jointData.h>           // struct for YARP extended port

#include "SubDevice.h"
#include "StreamingMessagesParser.h"
#include "RPCMessagesParser.h"

// ROS state publisher
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/rosmsg/sensor_msgs/JointState.h>
#include <yarp/rosmsg/impl/yarpRosHelper.h>

#ifdef MSVC
    #pragma warning(disable:4355)
#endif

#define PROTOCOL_VERSION_MAJOR 1
#define PROTOCOL_VERSION_MINOR 9
#define PROTOCOL_VERSION_TWEAK 0

/*
 * To optimize memory allocation, for group of joints we can have one mem reserver for rpc port
 * and on e for streaming. The size could be numOfSubDevices*maxNumOfjointForSubdevice.
 * (we could also use the actual joint number for each subdevice using a for loop). TODO
 */

class CommandsHelper;
class SubDevice;
class WrappedDevice;

class MultiJointData
{
public:
    int deviceNum{0};
    int maxJointsNumForDevice{0};

    int *subdev_jointsVectorLen{nullptr}; // number of joints belonging to each subdevice
    int **jointNumbers{nullptr};
    int **modes{nullptr};
    double **values{nullptr};
    SubDevice **subdevices_p{nullptr};

    MultiJointData() = default;

    void resize(int _deviceNum, int _maxJointsNumForDevice, WrappedDevice *_device)
    {
        deviceNum = _deviceNum;
        maxJointsNumForDevice = _maxJointsNumForDevice;
        subdev_jointsVectorLen    = new int  [deviceNum];
        jointNumbers    = new int *[deviceNum];                             // alloc a vector of pointers
        jointNumbers[0] = new int[deviceNum * _maxJointsNumForDevice];      // alloc real memory for data

        modes           = new int *[deviceNum];                             // alloc a vector of pointers
        modes[0]        = new int[deviceNum * _maxJointsNumForDevice];      // alloc real memory for data

        values      = new double *[deviceNum];                          // alloc a vector of pointers
        values[0]   = new double[deviceNum * _maxJointsNumForDevice];   // alloc real memory for data

        subdevices_p = new SubDevice *[deviceNum];
        subdevices_p[0] = _device->getSubdevice(0);

        for (int i = 1; i < deviceNum; i++)
        {
            jointNumbers[i] =  jointNumbers[i-1] + _maxJointsNumForDevice;   // set pointer to correct location
            values      [i] = values[i-1] + _maxJointsNumForDevice;      // set pointer to correct location
            modes       [i] = modes[i-1]  + _maxJointsNumForDevice;      // set pointer to correct location
            subdevices_p[i] = _device->getSubdevice(i);
        }
    }

    void destroy()
    {
        // release matrix memory
        delete[] jointNumbers[0];
        delete[] values[0];
        delete[] modes[0];

        // release vector of pointers
        delete[] jointNumbers;
        delete[] values;
        delete[] modes;

        // delete other vectors
        delete[] subdev_jointsVectorLen;
        delete[] subdevices_p;
    }
};

/**
 *  @ingroup dev_impl_wrapper
 *
 * \section ControlBoardWrapper Description of input parameters
 * A updated version of the controlBoard network wrapper.
 * It can merge together more than one control board device, or use only a
 * portion of it by remapping functionality.
 * Allows also deferred attach/detach of a subdevice.
 *
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
 *   <elem name="FirstSetOfJoints">  0  3  0  3  </elem>
 *   <elem name="SecondSetOfJoints"> 4  6  0  2  </elem>
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
        public yarp::dev::IPidControl,
        public yarp::dev::IPositionControl,
        public yarp::dev::IPositionDirect,
        public yarp::dev::IVelocityControl,
        public yarp::dev::IEncodersTimed,
        public yarp::dev::IMotor,
        public yarp::dev::IMotorEncoders,
        public yarp::dev::IAmplifierControl,
        public yarp::dev::IControlLimits,
        public yarp::dev::IRemoteCalibrator,
        public yarp::dev::IControlCalibration,
        public yarp::dev::ITorqueControl,
        public yarp::dev::IImpedanceControl,
        public yarp::dev::IControlMode,
        public yarp::dev::IMultipleWrapper,
        public yarp::dev::IAxisInfo,
        public yarp::dev::IPreciselyTimed,
        public yarp::dev::IInteractionMode,
        public yarp::dev::IRemoteVariables,
        public yarp::dev::IPWMControl,
        public yarp::dev::ICurrentControl
{
private:
    std::string rootName;
    WrappedDevice device;

    bool checkPortName(yarp::os::Searchable &params);

    yarp::rosmsg::sensor_msgs::JointState ros_struct;

    yarp::os::BufferedPort<yarp::sig::Vector>  outputPositionStatePort;   // Port /state:o streaming out the encoder positions
    yarp::os::BufferedPort<CommandMessage>     inputStreamingPort;        // Input streaming port for high frequency commands
    yarp::os::Port inputRPCPort;                // Input RPC port for set/get remote calls
    yarp::os::Stamp time;                       // envelope to attach to the state port
    yarp::sig::Vector times;                    // time for each joint
    std::mutex timeMutex;

    // Buffer associated to the extendedOutputStatePort port; in this case we will use the type generated
    // from the YARP .thrift file
    yarp::os::PortWriterBuffer<yarp::dev::impl::jointData>           extendedOutputState_buffer;
    yarp::os::Port extendedOutputStatePort;         // Port /stateExt:o streaming out the struct with the robot data

    // ROS state publisher
    ROSTopicUsageType                                   useROS;                     // decide if open ROS topic or not
    std::vector<std::string>                            jointNames;                 // name of the joints
    std::string                                         rosNodeName;                // name of the rosNode
    std::string                                         rosTopicName;               // name of the rosTopic
    yarp::os::Node                                      *rosNode;                   // add a ROS node
    yarp::os::NetUint32                                 rosMsgCounter;              // incremental counter in the ROS message
    yarp::os::PortWriterBuffer<yarp::rosmsg::sensor_msgs::JointState> rosOutputState_buffer; // Buffer associated to the ROS topic
    yarp::os::Publisher<yarp::rosmsg::sensor_msgs::JointState> rosPublisherPort;    // Dedicated ROS topic publisher

    yarp::os::PortReaderBuffer<yarp::os::Bottle>    inputRPC_buffer;                // Buffer associated to the inputRPCPort port
    RPCMessagesParser              RPC_parser;                     // Message parser associated to the inputRPCPort port
    StreamingMessagesParser        streaming_parser;               // Message parser associated to the inputStreamingPort port


    // RPC calls are concurrent from multiple clients, data used inside the calls has to be protected
    std::mutex                                 rpcDataMutex;                   // mutex to avoid concurrency between more clients using rppc port
    MultiJointData                 rpcData;                        // Structure used to re-arrange data from "multiple_joints" calls.

    std::string         partName;               // to open ports and print more detailed debug messages

    int               controlledJoints;
    int               base;         // to be removed
    int               top;          // to be removed
    double            period;       // thread rate for publishing data
    bool              _verb;        // make it work and propagate to subdevice if --subdevice option is used

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
    yarp::dev::PolyDriver *subDeviceOwned;
    bool openAndAttachSubDevice(yarp::os::Property& prop);

    bool ownDevices;
    inline void printError(std::string func_name, std::string info, bool result)
    {
        //If result is false, this means that en error occurred in function named func_name, otherwise means that the device doesn't implement the interface to witch func_name belongs to.
       // if(false == result)
       //    yError() << "CBW(" << partName << "): " << func_name.c_str() << " on device" << info.c_str() << " returns false";
        //Commented in order to maintain the old behaviour (none message appear if device desn't implement the interface)
        //else
            // yError() << "CBW(" << partName << "): " << func_name.c_str() << " on device" << info.c_str() << ": the interface is not available.";
    }

    void calculateMaxNumOfJointsInDevices();

public:
    ControlBoardWrapper();
    ControlBoardWrapper(const ControlBoardWrapper&) = delete;
    ControlBoardWrapper(ControlBoardWrapper&&) = delete;
    ControlBoardWrapper& operator=(const ControlBoardWrapper&) = delete;
    ControlBoardWrapper& operator=(ControlBoardWrapper&&) = delete;
    ~ControlBoardWrapper() override;

    /**
    * Return the value of the verbose flag.
    * @return the verbose flag.
    */
    bool verbose() const
    {
        return _verb;
    }

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
    * - verbose or v to print diagnostic information while running..
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

    /* IPidControl
    These methods are documented by Doxygen in IPidControl.h*/
    bool setPid(const yarp::dev::PidControlTypeEnum& pidtype, int j, const yarp::dev::Pid &p) override;
    bool setPids(const yarp::dev::PidControlTypeEnum& pidtype, const yarp::dev::Pid *ps) override;
    bool setPidReference(const yarp::dev::PidControlTypeEnum& pidtype, int j, double ref) override;
    bool setPidReferences(const yarp::dev::PidControlTypeEnum& pidtype, const double *refs) override;
    bool setPidErrorLimit(const yarp::dev::PidControlTypeEnum& pidtype, int j, double limit) override;
    bool setPidErrorLimits(const yarp::dev::PidControlTypeEnum& pidtype, const double *limits) override;
    bool getPidError(const yarp::dev::PidControlTypeEnum& pidtype, int j, double *err) override;
    bool getPidErrors(const yarp::dev::PidControlTypeEnum& pidtype, double *errs) override;
    bool getPidOutput(const yarp::dev::PidControlTypeEnum& pidtype, int j, double *out) override;
    bool getPidOutputs(const yarp::dev::PidControlTypeEnum& pidtype, double *outs) override;
    bool setPidOffset(const yarp::dev::PidControlTypeEnum& pidtype, int j, double v) override;
    bool getPid(const yarp::dev::PidControlTypeEnum& pidtype, int j, yarp::dev::Pid *p) override;
    bool getPids(const yarp::dev::PidControlTypeEnum& pidtype, yarp::dev::Pid *pids) override;
    bool getPidReference(const yarp::dev::PidControlTypeEnum& pidtype, int j, double *ref) override;
    bool getPidReferences(const yarp::dev::PidControlTypeEnum& pidtype, double *refs) override;
    bool getPidErrorLimit(const yarp::dev::PidControlTypeEnum& pidtype, int j, double *limit) override;
    bool getPidErrorLimits(const yarp::dev::PidControlTypeEnum& pidtype, double *limits) override;
    bool resetPid(const yarp::dev::PidControlTypeEnum& pidtype, int j) override;
    bool disablePid(const yarp::dev::PidControlTypeEnum& pidtype, int j) override;
    bool enablePid(const yarp::dev::PidControlTypeEnum& pidtype, int j) override;
    bool isPidEnabled(const yarp::dev::PidControlTypeEnum& pidtype, int j, bool* enabled) override;

    /* IPositionControl */

    /**
    * Get the number of controlled axes. This command asks the number of controlled
    * axes for the current physical interface.
    * @param ax pointer to storage
    * @return true/false.
    */
    bool getAxes(int *ax) override;

    /**
    * Set new reference point for a single axis.
    * @param j joint number
    * @param ref specifies the new ref point
    * @return true/false on success/failure
    */
    bool positionMove(int j, double ref) override;

    /** Set new reference point for all axes.
    * @param refs array, new reference points.
    * @return true/false on success/failure
    */
    bool positionMove(const double *refs) override;

    /** Set new reference point for a subset of axis.
     * @param joints pointer to the array of joint numbers
     * @param refs   pointer to the array specifying the new reference points
     * @return true/false on success/failure
     */
    bool positionMove(const int n_joints, const int *joints, const double *refs) override;

/** Get the last position reference for the specified axis.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    bool getTargetPosition(const int joint, double *ref) override;

    /** Get the last position reference for all axes.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    bool getTargetPositions(double *refs) override;

    /** Get the last position reference for the specified group of axes.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    bool getTargetPositions(const int n_joint, const int *joints, double *refs) override;

    /** Set relative position. The command is relative to the
    * current position of the axis.
    * @param j joint axis number
    * @param delta relative command
    * @return true/false on success/failure
    */
    bool relativeMove(int j, double delta) override;

    /** Set relative position, all joints.
    * @param deltas pointer to the relative commands
    * @return true/false on success/failure
    */
    bool relativeMove(const double *deltas) override;

    /** Set relative position for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param deltas pointer to the array of relative commands
     * @return true/false on success/failure
     */
    bool relativeMove(const int n_joints, const int *joints, const double *deltas) override;

    /**
    * Check if the current trajectory is terminated. Non blocking.
    * @param j the axis
    * @param flag true if the trajectory is terminated, false otherwise
    * @return false on failure
    */
    bool checkMotionDone(int j, bool *flag) override;
    /**
    * Check if the current trajectory is terminated. Non blocking.
    * @param flag true if the trajectory is terminated, false otherwise
    *        (a single value which is the 'and' of all joints')
    * @return false on failure
    */
    bool checkMotionDone(bool *flag) override;

    /** Check if the current trajectory is terminated. Non blocking.
     * @param joints pointer to the array of joint numbers
     * @param flag true if the trajectory is terminated, false otherwise
     *        (a single value which is the 'and' of all joints')
     * @return true/false if network communication went well.
     */
    bool checkMotionDone(const int n_joints, const int *joints, bool *flags) override;

    /** Set reference speed for a joint, this is the speed used during the
    * interpolation of the trajectory.
    * @param j joint number
    * @param sp speed value
    * @return true/false upon success/failure
    */
    bool setRefSpeed(int j, double sp) override;

    /** Set reference speed on all joints. These values are used during the
    * interpolation of the trajectory.
    * @param spds pointer to the array of speed values.
    * @return true/false upon success/failure
    */
    bool setRefSpeeds(const double *spds) override;

    /** Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param spds   pointer to the array with speed values.
     * @return true/false upon success/failure
     */
    bool setRefSpeeds(const int n_joints, const int *joints, const double *spds) override;

    /** Set reference acceleration for a joint. This value is used during the
    * trajectory generation.
    * @param j joint number
    * @param acc acceleration value
    * @return true/false upon success/failure
    */
    bool setRefAcceleration(int j, double acc) override;

    /** Set reference acceleration on all joints. This is the valure that is
    * used during the generation of the trajectory.
    * @param accs pointer to the array of acceleration values
    * @return true/false upon success/failure
    */
    bool setRefAccelerations(const double *accs) override;

    /** Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array with acceleration values
     * @return true/false upon success/failure
     */
    bool setRefAccelerations(const int n_joints, const int *joints, const double *accs) override;

    /** Get reference speed for a joint. Returns the speed used to
     * generate the trajectory profile.
     * @param j joint number
     * @param ref pointer to storage for the return value
     * @return true/false on success or failure
     */
    bool getRefSpeed(int j, double *ref) override;

    /** Get reference speed of all joints. These are the  values used during the
    * interpolation of the trajectory.
    * @param spds pointer to the array that will store the speed values.
    * @return true/false on success/failure.
    */
    bool getRefSpeeds(double *spds) override;

    /** Get reference speed of all joints. These are the  values used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param spds   pointer to the array that will store the speed values.
     * @return true/false upon success/failure
     */
    bool getRefSpeeds(const int n_joints, const int *joints, double *spds) override;

    /** Get reference acceleration for a joint. Returns the acceleration used to
    * generate the trajectory profile.
    * @param j joint number
    * @param acc pointer to storage for the return value
    * @return true/false on success/failure
    */
    bool getRefAcceleration(int j, double *acc) override;

    /** Get reference acceleration of all joints. These are the values used during the
    * interpolation of the trajectory.
    * @param accs pointer to the array that will store the acceleration values.
    * @return true/false on success or failure
    */
    bool getRefAccelerations(double *accs) override;

    /** Get reference acceleration for a joint. Returns the acceleration used to
     * generate the trajectory profile.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array that will store the acceleration values
     * @return true/false on success/failure
     */
    bool getRefAccelerations(const int n_joints, const int *joints, double *accs) override;

    /** Stop motion, single joint
    * @param j joint number
    * @return true/false on success/failure
    */
    bool stop(int j) override;

    /**
    * Stop motion, multiple joints
    * @return true/false on success/failure
    */
    bool stop() override;


    /** Stop motion for subset of joints
     * @param joints pointer to the array of joint numbers
     * @return true/false on success/failure
     */
    bool stop(const int n_joints, const int *joints) override;

    /* IVelocityControl */

    /**
    * Set new reference speed for a single axis.
    * @param j joint number
    * @param v specifies the new ref speed
    * @return true/false on success/failure
    */
    bool velocityMove(int j, double v) override;

    /**
    * Set a new reference speed for all axes.
    * @param v is a vector of double representing the requested speed.
    * @return true/false on success/failure.
    */
    bool velocityMove(const double *v) override;

    /* IEncoders */

    /**
    * Reset encoder, single joint. Set the encoder value to zero
    * @param j is the axis number
    * @return true/false on success/failure
    */
    bool resetEncoder(int j) override;

    /**
    * Reset encoders. Set the encoder values to zero for all axes
    * @return true/false
    */
    bool resetEncoders() override;

    /**
    * Set the value of the encoder for a given joint.
    * @param j encoder number
    * @param val new value
    * @return true/false
    */
    bool setEncoder(int j, double val) override;

    /**
    * Set the value of all encoders.
    * @param vals pointer to the new values
    * @return true/false
    */
    bool setEncoders(const double *vals) override;

    /**
    * Read the value of an encoder.
    * @param j encoder number
    * @param v pointer to storage for the return value
    * @return true/false, upon success/failure (you knew it, uh?)
    */
    bool getEncoder(int j, double *v) override;

    /**
    * Read the position of all axes.
    * @param encs pointer to the array that will contain the output
    * @return true/false on success/failure
    */
    bool getEncoders(double *encs) override;

    bool getEncodersTimed(double *encs, double *t) override;

    bool getEncoderTimed(int j, double *v, double *t) override;

    /**
    * Read the istantaneous speed of an axis.
    * @param j axis number
    * @param sp pointer to storage for the output
    * @return true if successful, false ... otherwise.
    */
    bool getEncoderSpeed(int j, double *sp) override;

    /**
    * Read the instantaneous speed of all axes.
    * @param spds pointer to storage for the output values
    * @return guess what? (true/false on success or failure).
    */
    bool getEncoderSpeeds(double *spds) override;

    /**
    * Read the instantaneous acceleration of an axis.
    * @param j axis number
    * @param acc pointer to the array that will contain the output
    */
    bool getEncoderAcceleration(int j, double *acc) override;
    /**
    * Read the istantaneous acceleration of all axes.
    * @param accs pointer to the array that will contain the output
    * @return true if all goes well, false if anything bad happens.
    */
    bool getEncoderAccelerations(double *accs) override;

    /* IMotorEncoders */

    /**
     * Get the number of available motor encoders.
     * @param m pointer to a value representing the number of available motor encoders.
     * @return true/false
     */
    bool getNumberOfMotorEncoders(int *num) override;

    /**
    * Reset encoder, single joint. Set the encoder value to zero
    * @param j is the axis number
    * @return true/false on success/failure
    */
    bool resetMotorEncoder(int m) override;

    /**
    * Reset encoders. Set the encoder values to zero for all axes
    * @return true/false
    */
    bool resetMotorEncoders() override;

    /**
     * Sets number of counts per revolution for motor encoder m.
     * @param m motor encoder number
     * @param cpr new parameter
     * @return true/false
     */
    bool setMotorEncoderCountsPerRevolution(int m, const double cpr) override;

    /**
     * gets number of counts per revolution for motor encoder m.
     * @param m motor encoder number
     * @param cpr pointer to storage for the return value
     * @return true/false
     */
    bool getMotorEncoderCountsPerRevolution(int m, double *cpr) override;

    /**
    * Set the value of the encoder for a given joint.
    * @param j encoder number
    * @param val new value
    * @return true/false
    */
    bool setMotorEncoder(int m, const double val) override;

    /**
    * Set the value of all encoders.
    * @param vals pointer to the new values
    * @return true/false
    */
    bool setMotorEncoders(const double *vals) override;

    /**
    * Read the value of an encoder.
    * @param j encoder number
    * @param v pointer to storage for the return value
    * @return true/false, upon success/failure (you knew it, uh?)
    */
    bool getMotorEncoder(int m, double *v) override;

    /**
    * Read the position of all axes.
    * @param encs pointer to the array that will contain the output
    * @return true/false on success/failure
    */
    bool getMotorEncoders(double *encs) override;

    bool getMotorEncodersTimed(double *encs, double *t) override;

    bool getMotorEncoderTimed(int m, double *v, double *t) override;

    /**
    * Read the istantaneous speed of an axis.
    * @param j axis number
    * @param sp pointer to storage for the output
    * @return true if successful, false ... otherwise.
    */
    bool getMotorEncoderSpeed(int m, double *sp) override;

    /**
    * Read the instantaneous speed of all axes.
    * @param spds pointer to storage for the output values
    * @return guess what? (true/false on success or failure).
    */
    bool getMotorEncoderSpeeds(double *spds) override;

    /**
    * Read the instantaneous acceleration of an axis.
    * @param j axis number
    * @param acc pointer to the array that will contain the output
    */
    bool getMotorEncoderAcceleration(int m, double *acc) override;
    /**
    * Read the istantaneous acceleration of all axes.
    * @param accs pointer to the array that will contain the output
    * @return true if all goes well, false if anything bad happens.
    */
    bool getMotorEncoderAccelerations(double *accs) override;

    /* IAmplifierControl */

    /**
    * Enable the amplifier on a specific joint. Be careful, check that the output
    * of the controller is appropriate (usually zero), to avoid
    * generating abrupt movements.
    * @return true/false on success/failure
    */
    bool enableAmp(int j) override;

    /**
    * Disable the amplifier on a specific joint. All computations within the board
    * will be carried out normally, but the output will be disabled.
    * @return true/false on success/failure
    */
    bool disableAmp(int j) override;

    /**
    * Get the status of the amplifiers, coded in a 32 bits integer for
    * each amplifier (at the moment contains only the fault, it will be
    * expanded in the future).
    * @param st pointer to storage
    * @return true in good luck, false otherwise.
    */
    bool getAmpStatus(int *st) override;

    bool getAmpStatus(int j, int *v) override;

    /**
    * Read the electric current going to all motors.
    * @param vals pointer to storage for the output values
    * @return hopefully true, false in bad luck.
    */
    bool getCurrents(double *vals) override;

    /**
    * Read the electric current going to a given motor.
    * @param j motor number
    * @param val pointer to storage for the output value
    * @return probably true, might return false in bad times
    */
    bool getCurrent(int j, double *val) override;

    /**
    * Set the maximum electric current going to a given motor. The behavior
    * of the board/amplifier when this limit is reached depends on the
    * implementation.
    * @param j motor number
    * @param v the new value
    * @return probably true, might return false in bad times
    */
    bool setMaxCurrent(int j, double v) override;

    /**
    * Returns the maximum electric current allowed for a given motor. The behavior
    * of the board/amplifier when this limit is reached depends on the
    * implementation.
    * @param j motor number
    * @param v the return value
    * @return probably true, might return false in bad times
    */
    bool getMaxCurrent(int j, double *v) override;

    /* Get the the nominal current which can be kept for an indefinite amount of time
     * without harming the motor. This value is specific for each motor and it is typically
     * found in its datasheet. The units are Ampere.
     * This value and the peak current may be used by the firmware to configure
     * an I2T filter.
     * @param m motor number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    bool getNominalCurrent(int m, double *val) override;

    /* Set the the nominal current which can be kept for an indefinite amount of time
    * without harming the motor. This value is specific for each motor and it is typically
    * found in its datasheet. The units are Ampere.
    * This value and the peak current may be used by the firmware to configure
    * an I2T filter.
    * @param m motor number
    * @param val storage for return value. [Ampere]
    * @return true/false success failure.
    */
    bool setNominalCurrent(int m, const double val) override;

    /* Get the the peak current which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor datasheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param m motor number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    bool getPeakCurrent(int m, double *val) override;

    /* Set the the peak current. This value  which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor datasheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param m motor number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    bool setPeakCurrent(int m, const double val) override;

    /* Get the the current PWM value used to control the motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param m motor number
     * @param val filled with PWM value.
     * @return true/false success failure.
     */
    bool getPWM(int m, double* val) override;

    /* Get the PWM limit for the given motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param m motor number
     * @param val filled with PWM limit value.
     * @return true/false success failure.
     */
    bool getPWMLimit(int m, double* val) override;

    /* Set the PWM limit for the given motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param m motor number
     * @param val new value for the PWM limit.
     * @return true/false success failure.
     */
    bool setPWMLimit(int m, const double val) override;

    /* Get the power source voltage for the given motor in Volt.
     * @param m motor number
     * @param val filled with return value.
     * @return true/false success failure.
     */
    bool getPowerSupplyVoltage(int m, double* val) override;

    /* IControlLimits */

    /**
    * Set the software limits for a particular axis, the behavior of the
    * control card when these limits are exceeded, depends on the implementation.
    * @param j joint number (why am I telling you this)
    * @param min the value of the lower limit
    * @param max the value of the upper limit
    * @return true or false on success or failure
    */
    bool setLimits(int j, double min, double max) override;

    /**
    * Get the software limits for a particular axis.
    * @param j joint number
    * @param min pointer to store the value of the lower limit
    * @param max pointer to store the value of the upper limit
    * @return true if everything goes fine, false if something bad happens (yes, sometimes life is tough)
    */
    bool getLimits(int j, double *min, double *max) override;

    /**
    * Set the software velocity limits for a particular axis, the behavior of the
    * control card when these limits are exceeded, depends on the implementation.
    * @param j joint number
    * @param min the value of the lower limit
    * @param max the value of the upper limit
    * @return true or false on success or failure
    */
    bool setVelLimits(int j, double min, double max) override;

    /**
    * Get the software velocity limits for a particular axis.
    * @param j joint number
    * @param min pointer to store the value of the lower limit
    * @param max pointer to store the value of the upper limit
    * @return true if everything goes fine, false if something bad happens
    */
    bool getVelLimits(int j, double *min, double *max) override;

    /* IRemoteVariables */

    bool getRemoteVariable(std::string key, yarp::os::Bottle& val) override;

    bool setRemoteVariable(std::string key, const yarp::os::Bottle& val) override;

    bool getRemoteVariablesList(yarp::os::Bottle* listOfKeys) override;

    /* IRemoteCalibrator */

    bool isCalibratorDevicePresent(bool *isCalib) override;

    /**
     * @brief getCalibratorDevice: return the pointer stored with the setCalibratorDevice
     * @return yarp::dev::IRemotizableCalibrator pointer or NULL if not valid
     */
    yarp::dev::IRemoteCalibrator *getCalibratorDevice() override;

    /**
     * @brief calibrateSingleJoint: call the calibration procedure for the single joint
     * @param j: joint to be calibrated
     * @return true if calibration was successful
     */
    bool calibrateSingleJoint(int j) override;

    /**
     * @brief calibrateWholePart: call the procedure for calibrating the whole device
     * @return true if calibration was successful
     */
    bool calibrateWholePart() override;

    /**
     * @brief homingSingleJoint: call the homing procedure for a single joint
     * @param j: joint to be calibrated
     * @return true if homing was successful, false otherwise
     */
    bool homingSingleJoint(int j) override;

    /**
     * @brief homingWholePart: call the homing procedure for a the whole part/device
     * @return true if homing was successful, false otherwise
     */
    bool homingWholePart() override;

    /**
     * @brief parkSingleJoint(): start the parking procedure for the single joint
     * @return true if successful
     */
    bool parkSingleJoint(int j, bool _wait=true) override;

    /**
     * @brief parkWholePart: start the parking procedure for the whole part
     * @return true if successful
     */
    bool parkWholePart() override;

    /**
     * @brief quitCalibrate: interrupt the calibration procedure
     * @return true if successful
     */
    bool quitCalibrate() override;

    /**
     * @brief quitPark: interrupt the park procedure
     * @return true if successful
     */
    bool quitPark() override;

    /* IControlCalibration */
    bool calibrateAxisWithParams(int j, unsigned int ui, double v1, double v2, double v3) override;

    bool setCalibrationParameters(int j, const yarp::dev::CalibrationParameters& params) override;

    /**
    * Check whether the calibration has been completed.
    * @param j is the joint that has started a calibration procedure.
    * @return true/false on success/failure.
    */
    bool calibrationDone(int j) override;

    bool abortPark() override;

    bool abortCalibration() override;

    /* IMotor */
    bool getNumberOfMotors   (int *num) override;

    bool getTemperature      (int m, double* val) override;

    bool getTemperatures     (double *vals) override;

    bool getTemperatureLimit (int m, double* val) override;

    bool setTemperatureLimit (int m, const double val) override;

    bool getGearboxRatio(int m, double* val) override;

    bool setGearboxRatio(int m, const double val) override;

    /* IAxisInfo */
    bool getAxisName(int j, std::string& name) override;

    bool getJointType(int j, yarp::dev::JointTypeEnum& type) override;

    bool getRefTorques(double *refs) override;

    bool getRefTorque(int j, double *t) override;

    bool setRefTorques(const double *t) override;

    bool setRefTorque(int j, double t) override;

    bool setRefTorques(const int n_joint, const int *joints, const double *t) override;

    bool getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params) override;

    bool setMotorTorqueParams(int j,  const yarp::dev::MotorTorqueParameters params) override;

     bool setImpedance(int j, double stiff, double damp) override;

    bool setImpedanceOffset(int j, double offset) override;

    bool getTorque(int j, double *t) override;

    bool getTorques(double *t) override;

    bool getTorqueRange(int j, double *min, double *max) override;

    bool getTorqueRanges(double *min, double *max) override;

    bool getImpedance(int j, double* stiff, double* damp) override;

    bool getImpedanceOffset(int j, double* offset) override;

    bool getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp) override;

    bool getControlMode(int j, int *mode) override;

    bool getControlModes(int *modes) override;

    // iControlMode2
    bool getControlModes(const int n_joint, const int *joints, int *modes) override;

    bool setControlMode(const int j, const int mode) override;

    bool setControlModes(const int n_joints, const int *joints, int *modes) override;

    bool setControlModes(int *modes) override;

    // IPositionDirect

    bool setPosition(int j, double ref) override;

    bool setPositions(const int n_joints, const int *joints, const double *dpos) override;

    bool setPositions(const double *refs) override;

        /** Get the last position reference for the specified axis.
     *  This is the dual of setPositions and shall return only values sent using
     *  IPositionDirect interface.
     *  If other interfaces like IPositionControl are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionControl::PositionMove.
     * @param ref last reference sent using setPosition(s) functions
     * @return true/false on success/failure
     */
    bool getRefPosition(const int joint, double *ref) override;

    /** Get the last position reference for all axes.
     *  This is the dual of setPositions and shall return only values sent using
     *  IPositionDirect interface.
     *  If other interfaces like IPositionControl are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionControl::PositionMove.
     * @param ref array containing last reference sent using setPosition(s) functions
     * @return true/false on success/failure
     */
    bool getRefPositions(double *refs) override;

    /** Get the last position reference for the specified group of axes.
     *  This is the dual of setPositions and shall return only values sent using
     *  IPositionDirect interface.
     *  If other interfaces like IPositionControl are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionControl::PositionMove.
     * @param ref array containing last reference sent using setPosition(s) functions
     * @return true/false on success/failure
     */
    bool getRefPositions(const int n_joint, const int *joints, double *refs) override;

    yarp::os::Stamp getLastInputStamp() override;

    //
    // IVelocityControl2 Interface
    //
    bool velocityMove(const int n_joints, const int *joints, const double *spds) override;

    bool getRefVelocity(const int joint, double* vel) override;

    bool getRefVelocities(double* vels) override;

    bool getRefVelocities(const int n_joint, const int* joints, double* vels) override;

    bool getInteractionMode(int j, yarp::dev::InteractionModeEnum* mode) override;

    bool getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override;

    bool getInteractionModes(yarp::dev::InteractionModeEnum* modes) override;

    bool setInteractionMode(int j, yarp::dev::InteractionModeEnum mode) override;

    bool setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override;

    bool setInteractionModes(yarp::dev::InteractionModeEnum* modes) override;

    //
    // IPWMControl Interface
    //

    bool setRefDutyCycle(int j, double v) override;
    bool setRefDutyCycles(const double *v) override;
    bool getRefDutyCycle(int j, double *v) override;
    bool getRefDutyCycles(double *v) override;
    bool getDutyCycle(int j, double *v) override;
    bool getDutyCycles(double *v) override;

    //
    // ICurrentControl Interface
    //

    //bool getAxes(int *ax) override;
    //bool getCurrent(int j, double *t) override;
    //bool getCurrents(double *t) override;
    bool getCurrentRange(int j, double *min, double *max) override;
    bool getCurrentRanges(double *min, double *max) override;
    bool setRefCurrents(const double *t) override;
    bool setRefCurrent(int j, double t) override;
    bool setRefCurrents(const int n_joint, const int *joints, const double *t) override;
    bool getRefCurrents(double *t) override;
    bool getRefCurrent(int j, double *t) override;
};


#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPER_H
