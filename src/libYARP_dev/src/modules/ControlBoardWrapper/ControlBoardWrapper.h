// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __CONTROLBOARDWRAPPER2__
#define __CONTROLBOARDWRAPPER2__

/*
* Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
* Author: Lorenzo Natale
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

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

//#define ROS_MSG
////#undef  ROS_MSG

//#define YARP_MSG
//#undef  YARP_MSG

#ifdef YARP_MSG
#include "jointData.h"
#endif

#ifdef ROS_MSG
#include "jointState.h"
#endif

#include "StreamingMessagesParser.h"
#include "RPCMessagesParser.h"
#include "SubDevice.h"

// ROS state publisher
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>


#ifdef MSVC
    #pragma warning(disable:4355)
#endif

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
        }
    }
}


#ifndef DOXYGEN_SHOULD_SKIP_THIS


enum MAX_VALUES_FOR_ALLOCATION_TABLE_TMP_DATA { MAX_DEVICES=5, MAX_JOINTS_ON_DEVICE=32};

#endif // DOXYGEN_SHOULD_SKIP_THIS

/*
* A updated version of the controlBoard network wrapper.
* It can merge toghether more than one device, or use only a
* portion of it by remapping utility.
* Allows also deferred attach/detach of a subdevice.
*/
class yarp::dev::ControlBoardWrapper:   public yarp::dev::DeviceDriver,
                                        public yarp::os::RateThread,
                                        public yarp::dev::IPidControl,
                                        public yarp::dev::IPositionControl2,
                                        public yarp::dev::IPositionDirect,
                                        public yarp::dev::IVelocityControl2,
                                        public yarp::dev::IEncodersTimed,
                                        public yarp::dev::IAmplifierControl,
                                        public yarp::dev::IControlLimits2,
                                        public yarp::dev::IControlCalibration,
                                        public yarp::dev::IControlCalibration2,
                                        public yarp::dev::IOpenLoopControl,
                                        public yarp::dev::ITorqueControl,
                                        public yarp::dev::IImpedanceControl,
                                        public yarp::dev::IControlMode2,
                                        public yarp::dev::IMultipleWrapper,
                                        public yarp::dev::IAxisInfo,
                                        public yarp::dev::IPreciselyTimed,
                                        public yarp::dev::IInteractionMode
{
private:

    std::string rootName;
    yarp::dev::impl::WrappedDevice device;

    yarp::os::Port outputPositionStatePort;     // Port /state:o streaming out the encoder positions
    yarp::os::Port outputStructStatePort;       // Port /?????:o streaming out the state as a struct
    yarp::os::Port inputStreamingPort;          // Input streaming port for high frequency commands
    yarp::os::Port inputRPCPort;                // Input RPC port for set/get remote calls
    yarp::os::Stamp time;                       // envelope to attach to the state port
    yarp::os::Semaphore timeMutex;

    yarp::os::PortWriterBuffer<yarp::sig::Vector>   outputPositionState_buffer;     // Buffer associated to the outputPositionStatePort port

#if defined(YARP_MSG)
    // Buffer associated to the extendedOutputStatePort port; in this case we will use the type generated
    // from the YARP .thrift file
    yarp::os::PortWriterBuffer<jointData>           extendedOutputState_buffer;
    yarp::os::Port extendedOutputStatePort;     // Port /stateExt:o streaming out the encoder positions
#endif


#if defined(ROS_MSG)
    // ROS state publisher
    yarp::os::Node *rosNode;   // added a Node
    yarp::os::PortWriterBuffer<jointState>           rosOutputState_buffer;     // Buffer associated to the extendedOutputStatePort port
    yarp::os::Publisher<jointState>  rosPublisherPort;  // changed Port to Publisher
#endif


    yarp::os::PortReaderBuffer<CommandMessage>      inputStreaming_buffer;          // Buffer associated to the inputStreamingPort port
    yarp::os::PortReaderBuffer<yarp::os::Bottle>    inputRPC_buffer;                // Buffer associated to the inputRPCPort port
    yarp::dev::impl::RPCMessagesParser              RPC_parser;                     // Message parser associated to the inputRPCPort port
    yarp::dev::impl::StreamingMessagesParser        streaming_parser;               // Message parser associated to the inputStreamingPort port

    yarp::sig::Vector   CBW_encoders;
    std::string         partName;               // to open ports and debug messages

    int               controlledJoints;
    int               base;         // to be removed
    int               top;          // to be removed
    int               thread_period;
    bool              _verb;        // make it work and propagate to subdevice if --subdevice option is used

    bool closeMain();

    yarp::os::Bottle getOptions();

    // Default usage
    // Open the wrapper only, the attach method needs to be called before using it
    bool openDeferredAttach(yarp::os::Property& prop);

    // For the simulator, if a subdevice parameter is given to the wrapper, it will
    // open it and and attach to it immediatly.
    yarp::dev::PolyDriver *subDeviceOwned;
    bool openAndAttachSubDevice(yarp::os::Property& prop);

    bool ownDevices;

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
    * @return false on failure
    */
    virtual bool checkMotionDone(bool *flag);

    /** Check if the current trajectory is terminated. Non blocking.
     * @param joints pointer to the array of joint numbers
     * @param flags  pointer to the array that will store the actual value of the checkMotionDone
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
    * Get the status of the amplifiers, coded in a 32 bits integer for
    * each amplifier (at the moment contains only the fault, it will be
    * expanded in the future).
    * @param st pointer to storage
    * @return true in good luck, false otherwise.
    */
    virtual bool getAmpStatus(int *st);

    virtual bool getAmpStatus(int j, int *v);

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

    /* IControlCalibration */

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

    /**
    * Check whether the calibration has been completed.
    * @param j is the joint that has started a calibration procedure.
    * @return true/false on success/failure.
    */
    virtual bool done(int j);

    virtual bool abortPark();

    virtual bool abortCalibration();

    /* IAxisInfo */
    virtual bool getAxisName(int j, yarp::os::ConstString& name);

    virtual bool setTorqueMode();

    virtual bool getRefTorques(double *refs);

    virtual bool getRefTorque(int j, double *t);

    virtual bool setRefTorques(const double *t);

    virtual bool setRefTorque(int j, double t);

    virtual bool getBemfParam(int j, double *t);

    virtual bool setBemfParam(int j, double t);

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

    virtual bool setPosition(int j, double ref);

    virtual bool setPositionDirectMode();

    virtual bool setPositions(const int n_joints, const int *joints, double *dpos);

    virtual bool setPositions(const double *refs);

    virtual yarp::os::Stamp getLastInputStamp();

    //
    // IVelocityControl2 Interface
    //
    virtual bool velocityMove(const int n_joints, const int *joints, const double *spds);

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
