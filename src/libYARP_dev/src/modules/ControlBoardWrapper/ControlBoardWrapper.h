/*
* Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
* Author: Lorenzo Natale
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
#include <yarp/os/RateThread.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/Wrapper.h>

#include <string>
#include <vector>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <jointData.h>           // struct for YARP extended port

#include "StreamingMessagesParser.h"
#include "RPCMessagesParser.h"
#include "SubDevice.h"

// ROS state publisher
#include <yarpRosHelper.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <sensor_msgs_JointState.h>  // Defines ROS jointState msg; it already includes TickTime and Header
#endif // DOXYGEN_SHOULD_SKIP_THIS

#ifdef MSVC
    #pragma warning(disable:4355)
#endif

#define PROTOCOL_VERSION_MAJOR 1
#define PROTOCOL_VERSION_MINOR 5
#define PROTOCOL_VERSION_TWEAK 1

/*
 * To optimize memory allocation, for group of joints we can have one mem reserver for rpc port
 * and on e for streaming. The size could be numOfSubDevices*maxNumOfjointForSubdevice.
 * (we could also use the actual joint number for each subdevice using a for loop). TODO
 */

/* Using yarp::dev::impl namespace for all helper class inside yarp::dev to reduce
 * name conflicts
 */

namespace yarp {
    namespace dev {
        class ControlBoardWrapper;
        namespace impl {
            class CommandsHelper;
            class SubDevice;
            class WrappedDevice;
            class MultiJointData;
        }
    }
}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

enum MAX_VALUES_FOR_ALLOCATION_TABLE_TMP_DATA { MAX_DEVICES=5, MAX_JOINTS_ON_DEVICE=32};


class yarp::dev::impl::MultiJointData
{
public:
    int deviceNum;
    int maxJointsNumForDevice;

    int *subdev_jointsVectorLen;                 // number of joints belonging to each subdevice
    int **jointNumbers;
    int **modes;
    double **values;
    yarp::dev::impl::SubDevice **subdevices_p;

    MultiJointData()
    {
        subdev_jointsVectorLen  = NULL;
        jointNumbers            = NULL;
        modes                   = NULL;
        values                  = NULL;
        subdevices_p            = NULL;
    }

    void resize(int _deviceNum, int _maxJointsNumForDevice, yarp::dev::impl::WrappedDevice *_device)
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

        subdevices_p = new yarp::dev::impl::SubDevice *[deviceNum];
        subdevices_p[0] = _device->getSubdevice(0);

        for (int i = 1; i < deviceNum; i++)
        {
            jointNumbers[i] =  jointNumbers[i-1] + _maxJointsNumForDevice;   // set pointer to correct location
            values      [i] = values[i-1] + _maxJointsNumForDevice;      // set pointer to correct location
            subdevices_p[i] = _device->getSubdevice(i);
        }
    }

    void destroy()
    {
        // relese matrix memory
        delete[] jointNumbers[0];
        delete[] values[0];
        delete[] modes[0];

        // relese vector of pointers
        delete[] jointNumbers;
        delete[] values;
        delete[] modes;

        // delete other vectors
        delete[] subdev_jointsVectorLen;
        delete[] subdevices_p;
    }
};

#endif // DOXYGEN_SHOULD_SKIP_THIS

/**
 *  @ingroup dev_impl_wrapper
 *
 * \section ControlBoardWrapper Description of input parameters
 * A updated version of the controlBoard network wrapper.
 * It can merge toghether more than one control board device, or use only a
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
 * |   -            |  ROS_TopicName | string  |  -             |   -           |  if ROS group is present    | set the name for ROS topic                                        | must start with a leading '/' |
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
 * XML format, using 'networks' keywork. This file is meant to be used in junction with yarprobotinterface executable,
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

class yarp::dev::ControlBoardWrapper:   public yarp::dev::DeviceDriver,
                                        public yarp::os::RateThread,
                                        public yarp::dev::IPidControl,
                                        public yarp::dev::IPositionControl2,
                                        public yarp::dev::IPositionDirect,
                                        public yarp::dev::IVelocityControl2,
                                        public yarp::dev::IEncodersTimed,
                                        public yarp::dev::IMotor,
                                        public yarp::dev::IMotorEncoders,
                                        public yarp::dev::IAmplifierControl,
                                        public yarp::dev::IControlLimits2,
                                        public yarp::dev::IRemoteCalibrator,
                                        public yarp::dev::IControlCalibration,
                                        public yarp::dev::IControlCalibration2,
                                        public yarp::dev::IOpenLoopControl,
                                        public yarp::dev::ITorqueControl,
                                        public yarp::dev::IImpedanceControl,
                                        public yarp::dev::IControlMode2,
                                        public yarp::dev::IMultipleWrapper,
                                        public yarp::dev::IAxisInfo,
                                        public yarp::dev::IPreciselyTimed,
                                        public yarp::dev::IInteractionMode,
                                        public yarp::dev::IRemoteVariables
{
private:

#ifndef    DOXYGEN_SHOULD_SKIP_THIS
    std::string rootName;
    yarp::dev::impl::WrappedDevice device;

    bool checkPortName(yarp::os::Searchable &params);


    yarp::os::BufferedPort<yarp::sig::Vector>  outputPositionStatePort;   // Port /state:o streaming out the encoder positions
    yarp::os::BufferedPort<CommandMessage>     inputStreamingPort;        // Input streaming port for high frequency commands
    yarp::os::Port inputRPCPort;                // Input RPC port for set/get remote calls
    yarp::os::Stamp time;                       // envelope to attach to the state port
    yarp::os::Semaphore timeMutex;

    // Buffer associated to the extendedOutputStatePort port; in this case we will use the type generated
    // from the YARP .thrift file
    yarp::os::PortWriterBuffer<jointData>           extendedOutputState_buffer;
    yarp::os::Port extendedOutputStatePort;         // Port /stateExt:o streaming out the struct with the robot data

    // ROS state publisher
    ROSTopicUsageType                                   useROS;                     // decide if open ROS topic or not
    std::vector<std::string>                            jointNames;                 // name of the joints
    std::string                                         rosNodeName;                // name of the rosNode
    std::string                                         rosTopicName;               // name of the rosTopic
    yarp::os::Node                                      *rosNode;                   // add a ROS node
    yarp::os::NetUint32                                 rosMsgCounter;              // incremental counter in the ROS message
    yarp::os::PortWriterBuffer<sensor_msgs_JointState>  rosOutputState_buffer;      // Buffer associated to the ROS topic
    yarp::os::Publisher<sensor_msgs_JointState>         rosPublisherPort;           // Dedicated ROS topic publisher

    yarp::os::PortReaderBuffer<yarp::os::Bottle>    inputRPC_buffer;                // Buffer associated to the inputRPCPort port
    yarp::dev::impl::RPCMessagesParser              RPC_parser;                     // Message parser associated to the inputRPCPort port
    yarp::dev::impl::StreamingMessagesParser        streaming_parser;               // Message parser associated to the inputStreamingPort port


    // RPC calls are concurrent from multiple clients, data used inside the calls has to be protected
    yarp::os::Semaphore                             rpcDataMutex;                   // mutex to avoid concurrency between more clients using rppc port
    yarp::dev::impl::MultiJointData                 rpcData;                        // Structure used to re-arrange data from "multiple_joints" calls.

    std::string         partName;               // to open ports and print more detailed debug messages

    int               controlledJoints;
    int               base;         // to be removed
    int               top;          // to be removed
    int               period;       // thread rate for publishing data
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
    // open it and and attach to it immediatly.
    yarp::dev::PolyDriver *subDeviceOwned;
    bool openAndAttachSubDevice(yarp::os::Property& prop);

    bool ownDevices;
#endif  //DOXYGEN_SHOULD_SKIP_THIS

public:
    /**
    * Constructor.
    */
    ControlBoardWrapper();

    virtual ~ControlBoardWrapper();

    /**
    * Return the value of the verbose flag.
    * @return the verbose flag.
    */
    bool verbose() const { return _verb; }

    /**
    * Default open() method.
    * @return always false since initialization requires parameters.
    */
    virtual bool open() { return false; }

    /**
    * Close the device driver by deallocating all resources and closing ports.
    * @return true if successful or false otherwise.
    */
    virtual bool close();


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
    virtual bool open(yarp::os::Searchable& prop);

    virtual bool detachAll();

    virtual bool attachAll(const yarp::dev::PolyDriverList &l);

    /**
    * The thread main loop deals with writing on ports here.
    */
    virtual void run();

    /* IPidControl */
    /** Set new pid value for a joint axis.
    * @param j joint number
    * @param p new pid value
    * @return true/false on success/failure
    */
    virtual bool setPid(int j, const Pid &p);

    /** Set new pid value on multiple axes.
    * @param ps pointer to a vector of pids
    * @return true/false upon success/failure
    */
    virtual bool setPids(const Pid *ps);

    /** Set the controller reference point for a given axis.
    * Warning this method can result in very large torques
    * and should be used carefully. If you do not understand
    * this warning you should avoid using this method.
    * Have a look at other interfaces (e.g. position control).
    * @param j joint number
    * @param ref new reference point
    * @return true/false upon success/failure
    */
    virtual bool setReference(int j, double ref);

    /** Set the controller reference points, multiple axes.
    * Warning this method can result in very large torques
    * and should be used carefully. If you do not understand
    * this warning you should avoid using this method.
    * Have a look at other interfaces (e.g. position control).
    * @param refs pointer to the vector that contains the new reference points.
    * @return true/false upon success/failure
    */
    virtual bool setReferences(const double *refs);

    /** Set the error limit for the controller on a specifi joint
    * @param j joint number
    * @param limit limit value
    * @return true/false on success/failure
    */
    virtual bool setErrorLimit(int j, double limit);

    /** Get the error limit for the controller on all joints.
    * @param limits pointer to the vector with the new limits
    * @return true/false on success/failure
    */
    virtual bool setErrorLimits(const double *limits);

    /** Get the current error for a joint.
    * @param j joint number
    * @param err pointer to the storage for the return value
    * @return true/false on success failure
    */
    virtual bool getError(int j, double *err);

    /** Get the error of all joints.
    * @param errs pointer to the vector that will store the errors.
    * @return true/false on success/failure.
    */
    virtual bool getErrors(double *errs);

    /** Get the output of the controller (e.g. pwm value)
    * @param j joint number
    * @param out pointer to storage for return value
    * @return success/failure
    */
    virtual bool getOutput(int j, double *out);

    /** Get the output of the controllers (e.g. pwm value)
    * @param outs pinter to the vector that will store the output values
    */
    virtual bool getOutputs(double *outs);

    virtual bool setOffset(int j, double v);

    /** Get current pid value for a specific joint.
    * @param j joint number
    * @param p pointer to storage for the return value.
    * @return success/failure
    */
    virtual bool getPid(int j, Pid *p);

    /** Get current pid value for a specific joint.
    * @param pids vector that will store the values of the pids.
    * @return success/failure
    */
    virtual bool getPids(Pid *pids);

    /** Get the current reference position of the controller for a specific joint.
    * @param j joint number
    * @param ref pointer to storage for return value
    * @return reference value
    */
    virtual bool getReference(int j, double *ref);

    /** Get the current reference position of all controllers.
    * @param refs vector that will store the output.
    */
    virtual bool getReferences(double *refs);

    /** Get the error limit for the controller on a specific joint
    * @param j joint number
    * @param limit pointer to storage
    * @return success/failure
    */
    virtual bool getErrorLimit(int j, double *limit);
    /** Get the error limit for all controllers
    * @param limits pointer to the array that will store the output
    * @return success or failure
    */
    virtual bool getErrorLimits(double *limits);

    /** Reset the controller of a given joint, usually sets the
    * current position of the joint as the reference value for the PID, and resets
    * the integrator.
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual bool resetPid(int j);
    /**
    * Disable the pid computation for a joint
    * @param j is the axis number
    * @return true if successful, false on failure
    **/
    virtual bool disablePid(int j);

    /**
    * Enable the pid computation for a joint
    * @param j is the axis number
    * @return true/false on success/failure
    */
    virtual bool enablePid(int j);

    /* IPositionControl */

    /**
    * Get the number of controlled axes. This command asks the number of controlled
    * axes for the current physical interface.
    * @param ax pointer to storage
    * @return true/false.
    */
    virtual bool getAxes(int *ax);

    /**
    * Set position mode. This command
    * is required by control boards implementing different
    * control methods (e.g. velocity/torque), in some cases
    * it can be left empty.
    * return true/false on success/failure
    */
    virtual bool setPositionMode();
    virtual bool setOpenLoopMode();

    /**
    * Set new reference point for a single axis.
    * @param j joint number
    * @param ref specifies the new ref point
    * @return true/false on success/failure
    */
    virtual bool positionMove(int j, double ref);

    /** Set new reference point for all axes.
    * @param refs array, new reference points.
    * @return true/false on success/failure
    */
    virtual bool positionMove(const double *refs);

    /** Set new reference point for a subset of axis.
     * @param joints pointer to the array of joint numbers
     * @param refs   pointer to the array specifing the new reference points
     * @return true/false on success/failure
     */
    virtual bool positionMove(const int n_joints, const int *joints, const double *refs);

/** Get the last position reference for the specified axis.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    virtual bool getTargetPosition(const int joint, double *ref);

    /** Get the last position reference for all axes.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    virtual bool getTargetPositions(double *refs);

    /** Get the last position reference for the specified group of axes.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    virtual bool getTargetPositions(const int n_joint, const int *joints, double *refs);

    /** Set relative position. The command is relative to the
    * current position of the axis.
    * @param j joint axis number
    * @param delta relative command
    * @return true/false on success/failure
    */
    virtual bool relativeMove(int j, double delta);

    /** Set relative position, all joints.
    * @param deltas pointer to the relative commands
    * @return true/false on success/failure
    */
    virtual bool relativeMove(const double *deltas);

    /** Set relative position for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param deltas pointer to the array of relative commands
     * @return true/false on success/failure
     */
    virtual bool relativeMove(const int n_joints, const int *joints, const double *deltas);

    /**
    * Check if the current trajectory is terminated. Non blocking.
    * @param j the axis
    * @param flag true if the trajectory is terminated, false otherwise
    * @return false on failure
    */
    virtual bool checkMotionDone(int j, bool *flag);
    /**
    * Check if the current trajectory is terminated. Non blocking.
    * @param flag true if the trajectory is terminated, false otherwise
    *        (a single value which is the 'and' of all joints')
    * @return false on failure
    */
    virtual bool checkMotionDone(bool *flag);

    /** Check if the current trajectory is terminated. Non blocking.
     * @param joints pointer to the array of joint numbers
     * @param flag true if the trajectory is terminated, false otherwise
     *        (a single value which is the 'and' of all joints')
     * @return true/false if network communication went well.
     */
    virtual bool checkMotionDone(const int n_joints, const int *joints, bool *flags);

    /** Set reference speed for a joint, this is the speed used during the
    * interpolation of the trajectory.
    * @param j joint number
    * @param sp speed value
    * @return true/false upon success/failure
    */
    virtual bool setRefSpeed(int j, double sp);

    /** Set reference speed on all joints. These values are used during the
    * interpolation of the trajectory.
    * @param spds pointer to the array of speed values.
    * @return true/false upon success/failure
    */
    virtual bool setRefSpeeds(const double *spds);

    /** Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param spds   pointer to the array with speed values.
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeeds(const int n_joints, const int *joints, const double *spds);

    /** Set reference acceleration for a joint. This value is used during the
    * trajectory generation.
    * @param j joint number
    * @param acc acceleration value
    * @return true/false upon success/failure
    */
    virtual bool setRefAcceleration(int j, double acc);

    /** Set reference acceleration on all joints. This is the valure that is
    * used during the generation of the trajectory.
    * @param accs pointer to the array of acceleration values
    * @return true/false upon success/failure
    */
    virtual bool setRefAccelerations(const double *accs);

    /** Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array with acceleration values
     * @return true/false upon success/failure
     */
    virtual bool setRefAccelerations(const int n_joints, const int *joints, const double *accs);

    /** Get reference speed for a joint. Returns the speed used to
     * generate the trajectory profile.
     * @param j joint number
     * @param ref pointer to storage for the return value
     * @return true/false on success or failure
     */
    virtual bool getRefSpeed(int j, double *ref);

    /** Get reference speed of all joints. These are the  values used during the
    * interpolation of the trajectory.
    * @param spds pointer to the array that will store the speed values.
    * @return true/false on success/failure.
    */
    virtual bool getRefSpeeds(double *spds);

    /** Get reference speed of all joints. These are the  values used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param spds   pointer to the array that will store the speed values.
     * @return true/false upon success/failure
     */
    virtual bool getRefSpeeds(const int n_joints, const int *joints, double *spds);

    /** Get reference acceleration for a joint. Returns the acceleration used to
    * generate the trajectory profile.
    * @param j joint number
    * @param acc pointer to storage for the return value
    * @return true/false on success/failure
    */
    virtual bool getRefAcceleration(int j, double *acc);

    /** Get reference acceleration of all joints. These are the values used during the
    * interpolation of the trajectory.
    * @param accs pointer to the array that will store the acceleration values.
    * @return true/false on success or failure
    */
    virtual bool getRefAccelerations(double *accs);

    /** Get reference acceleration for a joint. Returns the acceleration used to
     * generate the trajectory profile.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array that will store the acceleration values
     * @return true/false on success/failure
     */
    virtual bool getRefAccelerations(const int n_joints, const int *joints, double *accs);

    /** Stop motion, single joint
    * @param j joint number
    * @return true/false on success/failure
    */
    virtual bool stop(int j);

    /**
    * Stop motion, multiple joints
    * @return true/false on success/failure
    */
    virtual bool stop();


    /** Stop motion for subset of joints
     * @param joints pointer to the array of joint numbers
     * @return true/false on success/failure
     */
    virtual bool stop(const int n_joints, const int *joints);

    /* IVelocityControl */

    /**
    * Set new reference speed for a single axis.
    * @param j joint number
    * @param v specifies the new ref speed
    * @return true/false on success/failure
    */
    virtual bool velocityMove(int j, double v);

    /**
    * Set a new reference speed for all axes.
    * @param v is a vector of double representing the requested speed.
    * @return true/false on success/failure.
    */
    virtual bool velocityMove(const double *v);

    /**
    * Set the controller to velocity mode.
    * @return true/false on success/failure.
    */
    virtual bool setVelocityMode();

    /* IEncoders */

    /**
    * Reset encoder, single joint. Set the encoder value to zero
    * @param j is the axis number
    * @return true/false on success/failure
    */
    virtual bool resetEncoder(int j);

    /**
    * Reset encoders. Set the encoder values to zero for all axes
    * @return true/false
    */
    virtual bool resetEncoders();

    /**
    * Set the value of the encoder for a given joint.
    * @param j encoder number
    * @param val new value
    * @return true/false
    */
    virtual bool setEncoder(int j, double val);

    /**
    * Set the value of all encoders.
    * @param vals pointer to the new values
    * @return true/false
    */
    virtual bool setEncoders(const double *vals);

    /**
    * Read the value of an encoder.
    * @param j encoder number
    * @param v pointer to storage for the return value
    * @return true/false, upon success/failure (you knew it, uh?)
    */
    virtual bool getEncoder(int j, double *v);

    /**
    * Read the position of all axes.
    * @param encs pointer to the array that will contain the output
    * @return true/false on success/failure
    */
    virtual bool getEncoders(double *encs);

    virtual bool getEncodersTimed(double *encs, double *t);

    virtual bool getEncoderTimed(int j, double *v, double *t);

    /**
    * Read the istantaneous speed of an axis.
    * @param j axis number
    * @param sp pointer to storage for the output
    * @return true if successful, false ... otherwise.
    */
    virtual bool getEncoderSpeed(int j, double *sp);

    /**
    * Read the instantaneous speed of all axes.
    * @param spds pointer to storage for the output values
    * @return guess what? (true/false on success or failure).
    */
    virtual bool getEncoderSpeeds(double *spds);

    /**
    * Read the instantaneous acceleration of an axis.
    * @param j axis number
    * @param acc pointer to the array that will contain the output
    */
    virtual bool getEncoderAcceleration(int j, double *acc);
    /**
    * Read the istantaneous acceleration of all axes.
    * @param accs pointer to the array that will contain the output
    * @return true if all goes well, false if anything bad happens.
    */
    virtual bool getEncoderAccelerations(double *accs);

    /* IMotorEncoders */

    /**
     * Get the number of available motor encoders.
     * @param m pointer to a value representing the number of available motor encoders.
     * @return true/false
     */
    virtual bool getNumberOfMotorEncoders(int *num);

    /**
    * Reset encoder, single joint. Set the encoder value to zero
    * @param j is the axis number
    * @return true/false on success/failure
    */
    virtual bool resetMotorEncoder(int m);

    /**
    * Reset encoders. Set the encoder values to zero for all axes
    * @return true/false
    */
    virtual bool resetMotorEncoders();

    /**
     * Sets number of counts per revolution for motor encoder m.
     * @param m motor encoder number
     * @param cpr new parameter
     * @return true/false
     */
    virtual bool setMotorEncoderCountsPerRevolution(int m, const double cpr);

    /**
     * gets number of counts per revolution for motor encoder m.
     * @param m motor encoder number
     * @param cpr pointer to storage for the return value
     * @return true/false
     */
    virtual bool getMotorEncoderCountsPerRevolution(int m, double *cpr);

    /**
    * Set the value of the encoder for a given joint.
    * @param j encoder number
    * @param val new value
    * @return true/false
    */
    virtual bool setMotorEncoder(int m, const double val);

    /**
    * Set the value of all encoders.
    * @param vals pointer to the new values
    * @return true/false
    */
    virtual bool setMotorEncoders(const double *vals);

    /**
    * Read the value of an encoder.
    * @param j encoder number
    * @param v pointer to storage for the return value
    * @return true/false, upon success/failure (you knew it, uh?)
    */
    virtual bool getMotorEncoder(int m, double *v);

    /**
    * Read the position of all axes.
    * @param encs pointer to the array that will contain the output
    * @return true/false on success/failure
    */
    virtual bool getMotorEncoders(double *encs);

    virtual bool getMotorEncodersTimed(double *encs, double *t);

    virtual bool getMotorEncoderTimed(int m, double *v, double *t);

    /**
    * Read the istantaneous speed of an axis.
    * @param j axis number
    * @param sp pointer to storage for the output
    * @return true if successful, false ... otherwise.
    */
    virtual bool getMotorEncoderSpeed(int m, double *sp);

    /**
    * Read the instantaneous speed of all axes.
    * @param spds pointer to storage for the output values
    * @return guess what? (true/false on success or failure).
    */
    virtual bool getMotorEncoderSpeeds(double *spds);

    /**
    * Read the instantaneous acceleration of an axis.
    * @param j axis number
    * @param acc pointer to the array that will contain the output
    */
    virtual bool getMotorEncoderAcceleration(int m, double *acc);
    /**
    * Read the istantaneous acceleration of all axes.
    * @param accs pointer to the array that will contain the output
    * @return true if all goes well, false if anything bad happens.
    */
    virtual bool getMotorEncoderAccelerations(double *accs);

    /* IAmplifierControl */

    /**
    * Enable the amplifier on a specific joint. Be careful, check that the output
    * of the controller is appropriate (usually zero), to avoid
    * generating abrupt movements.
    * @return true/false on success/failure
    */
    virtual bool enableAmp(int j);

    /**
    * Disable the amplifier on a specific joint. All computations within the board
    * will be carried out normally, but the output will be disabled.
    * @return true/false on success/failure
    */
    virtual bool disableAmp(int j);

    /**
    * Get the status of the amplifiers, coded in a 32 bits integer for
    * each amplifier (at the moment contains only the fault, it will be
    * expanded in the future).
    * @param st pointer to storage
    * @return true in good luck, false otherwise.
    */
    virtual bool getAmpStatus(int *st);

    virtual bool getAmpStatus(int j, int *v);

    /**
    * Read the electric current going to all motors.
    * @param vals pointer to storage for the output values
    * @return hopefully true, false in bad luck.
    */
    virtual bool getCurrents(double *vals);

    /**
    * Read the electric current going to a given motor.
    * @param j motor number
    * @param val pointer to storage for the output value
    * @return probably true, might return false in bad times
    */
    virtual bool getCurrent(int j, double *val);

    /**
    * Set the maximum electric current going to a given motor. The behavior
    * of the board/amplifier when this limit is reached depends on the
    * implementation.
    * @param j motor number
    * @param v the new value
    * @return probably true, might return false in bad times
    */
    virtual bool setMaxCurrent(int j, double v);

    /**
    * Returns the maximum electric current allowed for a given motor. The behavior
    * of the board/amplifier when this limit is reached depends on the
    * implementation.
    * @param j motor number
    * @param v the return value
    * @return probably true, might return false in bad times
    */
    virtual bool getMaxCurrent(int j, double *v);

    /* Get the the nominal current which can be kept for an indefinite amount of time
     * without harming the motor. This value is specific for each motor and it is typically
     * found in its datasheet. The units are Ampere.
     * This value and the peak current may be used by the firmware to configure
     * an I2T filter.
     * @param m motor number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool getNominalCurrent(int m, double *val);

    /* Get the the peak current which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor datasheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param m motor number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool getPeakCurrent(int m, double *val);

    /* Set the the peak current. This value  which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor datasheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param m motor number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool setPeakCurrent(int m, const double val);

    /* Get the the current PWM value used to control the motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param m motor number
     * @param val filled with PWM value.
     * @return true/false success failure.
     */
    virtual bool getPWM(int m, double* val);

    /* Get the PWM limit fot the given motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param m motor number
     * @param val filled with PWM limit value.
     * @return true/false success failure.
     */
    virtual bool getPWMLimit(int m, double* val);

    /* Set the PWM limit fot the given motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param m motor number
     * @param val new value for the PWM limit.
     * @return true/false success failure.
     */
    virtual bool setPWMLimit(int m, const double val);

    /* Get the power source voltage for the given motor in Volt.
     * @param m motor number
     * @param val filled with return value.
     * @return true/false success failure.
     */
    virtual bool getPowerSupplyVoltage(int m, double* val);

    /* IControlLimits */

    /**
    * Set the software limits for a particular axis, the behavior of the
    * control card when these limits are exceeded, depends on the implementation.
    * @param j joint number (why am I telling you this)
    * @param min the value of the lower limit
    * @param max the value of the upper limit
    * @return true or false on success or failure
    */
    virtual bool setLimits(int j, double min, double max);

    /**
    * Get the software limits for a particular axis.
    * @param j joint number
    * @param min pointer to store the value of the lower limit
    * @param max pointer to store the value of the upper limit
    * @return true if everything goes fine, false if something bad happens (yes, sometimes life is tough)
    */
    virtual bool getLimits(int j, double *min, double *max);

    /**
    * Set the software velocity limits for a particular axis, the behavior of the
    * control card when these limits are exceeded, depends on the implementation.
    * @param j joint number
    * @param min the value of the lower limit
    * @param max the value of the upper limit
    * @return true or false on success or failure
    */
    virtual bool setVelLimits(int j, double min, double max);

    /**
    * Get the software velocity limits for a particular axis.
    * @param j joint number
    * @param min pointer to store the value of the lower limit
    * @param max pointer to store the value of the upper limit
    * @return true if everything goes fine, false if something bad happens
    */
    virtual bool getVelLimits(int j, double *min, double *max);

    /* IRemoteVariables */

    virtual bool getRemoteVariable(yarp::os::ConstString key, yarp::os::Bottle& val);

    virtual bool setRemoteVariable(yarp::os::ConstString key, const yarp::os::Bottle& val);

    virtual bool getRemoteVariablesList(yarp::os::Bottle* listOfKeys);

    /* IRemoteCalibrator */

    bool isCalibratorDevicePresent(bool *isCalib);

    /**
     * @brief getCalibratorDevice: return the pointer stored with the setCalibratorDevice
     * @return yarp::dev::IRemotizableCalibrator pointer or NULL if not valid
     */
    virtual yarp::dev::IRemoteCalibrator *getCalibratorDevice();

    /**
     * @brief calibrateSingleJoint: call the calibration procedure for the single joint
     * @param j: joint to be calibrated
     * @return true if calibration was successful
     */
    virtual bool calibrateSingleJoint(int j);

    /**
     * @brief calibrateWholePart: call the procedure for calibrating the whole device
     * @return true if calibration was successful
     */
    virtual bool calibrateWholePart();

    /**
     * @brief homingSingleJoint: call the homing procedure for a single joint
     * @param j: joint to be calibrated
     * @return true if homing was succesful, false otherwise
     */
    virtual bool homingSingleJoint(int j);

    /**
     * @brief homingWholePart: call the homing procedure for a the whole part/device
     * @return true if homing was succesful, false otherwise
     */
    virtual bool homingWholePart();

    /**
     * @brief parkSingleJoint(): start the parking procedure for the single joint
     * @return true if succesful
     */
    virtual bool parkSingleJoint(int j, bool _wait=true);

    /**
     * @brief parkWholePart: start the parking procedure for the whole part
     * @return true if succesful
     */
    virtual bool parkWholePart();

    /**
     * @brief quitCalibrate: interrupt the calibration procedure
     * @return true if succesful
     */
    virtual bool quitCalibrate();

    /**
     * @brief quitPark: interrupt the park procedure
     * @return true if succesful
     */
    virtual bool quitPark();

    /* IControlCalibration */

    using yarp::dev::IControlCalibration2::calibrate;

    /**
    * Calibrate a single joint, the calibration method accepts a parameter
    * that is used to accomplish various things internally and is implementation
    * dependent.
    * @param j the axis number.
    * @param p is a double value that is passed to the calibration procedure.
    * @return true/false on success/failure.
    */
    virtual bool calibrate(int j, double p);

    virtual bool calibrate2(int j, unsigned int ui, double v1, double v2, double v3);

    virtual bool setCalibrationParameters(int j, const CalibrationParameters& params);

    /**
    * Check whether the calibration has been completed.
    * @param j is the joint that has started a calibration procedure.
    * @return true/false on success/failure.
    */
    virtual bool done(int j);

    virtual bool abortPark();

    virtual bool abortCalibration();

    /* IMotor */
    virtual bool getNumberOfMotors   (int *num);

    virtual bool getTemperature      (int m, double* val);

    virtual bool getTemperatures     (double *vals);

    virtual bool getTemperatureLimit (int m, double* val);

    virtual bool setTemperatureLimit (int m, const double val);

    virtual bool getGearboxRatio(int m, double* val);

    virtual bool setGearboxRatio(int m, const double val);

    /* IAxisInfo */
    virtual bool getAxisName(int j, yarp::os::ConstString& name);

    virtual bool getJointType(int j, yarp::dev::JointTypeEnum& type);

    virtual bool setTorqueMode();

    virtual bool getRefTorques(double *refs);

    virtual bool getRefTorque(int j, double *t);

    virtual bool setRefTorques(const double *t);

    virtual bool setRefTorque(int j, double t);

    virtual bool setRefTorques(const int n_joint, const int *joints, const double *t);

    virtual bool getBemfParam(int j, double *t);

    virtual bool setBemfParam(int j, double t);

    virtual bool getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params);

    virtual bool setMotorTorqueParams(int j,  const yarp::dev::MotorTorqueParameters params);

    virtual bool setTorquePid(int j, const Pid &pid);

    virtual bool setImpedance(int j, double stiff, double damp);

    virtual bool setImpedanceOffset(int j, double offset);

    virtual bool getTorque(int j, double *t);

    virtual bool getTorques(double *t);

    virtual bool getTorqueRange(int j, double *min, double *max);

    virtual bool getTorqueRanges(double *min, double *max);

    virtual bool setTorquePids(const Pid *pids);

    virtual bool setTorqueErrorLimit(int j, double limit);

    virtual bool setTorqueErrorLimits(const double *limits);

    virtual bool getTorqueError(int j, double *err);

    virtual bool getTorqueErrors(double *errs);

    virtual bool getTorquePidOutput(int j, double *out);

    virtual bool getTorquePidOutputs(double *outs);

    virtual bool getTorquePid(int j, Pid *pid);

    virtual bool getImpedance(int j, double* stiff, double* damp);

    virtual bool getImpedanceOffset(int j, double* offset);

    virtual bool getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp);

    virtual bool getTorquePids(Pid *pids);

    virtual bool getTorqueErrorLimit(int j, double *limit);

    virtual bool getTorqueErrorLimits(double *limits);

    virtual bool resetTorquePid(int j);

    virtual bool disableTorquePid(int j);

    virtual bool enableTorquePid(int j);

    virtual bool setTorqueOffset(int j, double v);

    virtual bool setPositionMode(int j);

    virtual bool setTorqueMode(int j);

    virtual bool setImpedancePositionMode(int j);

    virtual bool setImpedanceVelocityMode(int j);

    virtual bool setVelocityMode(int j);

    virtual bool setOpenLoopMode(int j);

    virtual bool getControlMode(int j, int *mode);

    virtual bool getControlModes(int *modes);

    // iControlMode2
    virtual bool getControlModes(const int n_joint, const int *joints, int *modes);

    bool legacySetControlMode(const int j, const int mode);

    virtual bool setControlMode(const int j, const int mode);

    virtual bool setControlModes(const int n_joints, const int *joints, int *modes);

    virtual bool setControlModes(int *modes);

    virtual bool setRefOutput(int j, double v);

    virtual bool setRefOutputs(const double *outs);

    virtual bool setPositionDirectMode();

    virtual bool setPosition(int j, double ref);

    virtual bool setPositions(const int n_joints, const int *joints, double *dpos);

    virtual bool setPositions(const double *refs);

        /** Get the last position reference for the specified axis.
     *  This is the dual of setPositionsRaw and shall return only values sent using
     *  IPositionDirect interface.
     *  If other interfaces like IPositionControl are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionControl::PositionMove.
     * @param ref last reference sent using setPosition(s) functions
     * @return true/false on success/failure
     */
    virtual bool getRefPosition(const int joint, double *ref);

    /** Get the last position reference for all axes.
     *  This is the dual of setPositionsRaw and shall return only values sent using
     *  IPositionDirect interface.
     *  If other interfaces like IPositionControl are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionControl::PositionMove.
     * @param ref array containing last reference sent using setPosition(s) functions
     * @return true/false on success/failure
     */
    virtual bool getRefPositions(double *refs);

    /** Get the last position reference for the specified group of axes.
     *  This is the dual of setPositionsRaw and shall return only values sent using
     *  IPositionDirect interface.
     *  If other interfaces like IPositionControl are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionControl::PositionMove.
     * @param ref array containing last reference sent using setPosition(s) functions
     * @return true/false on success/failure
     */
    virtual bool getRefPositions(const int n_joint, const int *joints, double *refs);

    virtual yarp::os::Stamp getLastInputStamp();

    //
    // IVelocityControl2 Interface
    //
    virtual bool velocityMove(const int n_joints, const int *joints, const double *spds);

    virtual bool getRefVelocity(const int joint, double* vel);

    virtual bool getRefVelocities(double* vels);

    virtual bool getRefVelocities(const int n_joint, const int* joints, double* vels);

    virtual bool setVelPid(int j, const Pid &pid);

    virtual bool setVelPids(const Pid *pids);

    virtual bool getVelPid(int j, Pid *pid);

    virtual bool getVelPids(Pid *pids);

    virtual bool getInteractionMode(int j, yarp::dev::InteractionModeEnum* mode);

    virtual bool getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes);

    virtual bool getInteractionModes(yarp::dev::InteractionModeEnum* modes);

    virtual bool setInteractionMode(int j, yarp::dev::InteractionModeEnum mode);

    virtual bool setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes);

    virtual bool setInteractionModes(yarp::dev::InteractionModeEnum* modes);

    /**
     * Get the last reference sent using the setOutput function
     * @param outs pointer to the vector that will store the output values
     * @return true/false on success/failure
     */
    virtual bool getRefOutput(int j, double *out);

    /**
     * Get the last reference sent using the setOutputs function
     * @param outs pointer to the vector that will store the output values
     * @return true/false on success/failure
     */
    virtual bool getRefOutputs(double *outs);
};

#endif
