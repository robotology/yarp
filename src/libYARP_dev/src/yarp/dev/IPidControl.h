/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_PIDCONTROL_H
#define YARP_DEV_PIDCONTROL_H

#include <vector>

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/dev/GenericVocabs.h>
#include <yarp/dev/PidEnums.h>
#include <yarp/dev/ControlBoardPid.h>
#include <yarp/dev/PidDataExtraInfo.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {
class IPidControlRaw;
class IPidControl;
}


/**
 * @ingroup dev_iface_motor_raw
 *
 * Interface for a generic control board device implementing a PID controller.
 */
class YARP_dev_API yarp::dev::IPidControlRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IPidControlRaw() {}

    //////////////////////////////////////

    /** Return a list of available pid types mode for the given joint.
    * @param j joint number
    * @param avail a vector that will contain the list of available control modes for joint j.
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue getAvailablePidsRaw(int j, std::vector<PidControlTypeEnum>& avail) = 0;

    /** Set new PID value for a joint axis.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param pid new PID value
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue setPidRaw(const PidControlTypeEnum& pidtype, int j, const Pid &pid) = 0;

    /** Set new PID value on multiple axes.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param pids pointer to a vector of pids
    * @return true/false upon success/failure
    */
    virtual yarp::dev::ReturnValue setPidsRaw(const PidControlTypeEnum& pidtype, const Pid *pids) = 0;

    /** Set the controller reference for a given axis.
    * Warning this method can result in very large torques
    * and should be used carefully. If you do not understand
    * this warning you should avoid using this method.
    * Have a look at other interfaces (e.g. position control).
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param ref new reference point
    * @return true/false upon success/failure
    */
    virtual yarp::dev::ReturnValue setPidReferenceRaw(const PidControlTypeEnum& pidtype, int j, double ref) = 0;

    /** Set the controller reference, multiple axes.
    * Warning this method can result in very large torques
    * and should be used carefully. If you do not understand
    * this warning you should avoid using this method.
    * Have a look at other interfaces (e.g. position control).
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param refs pointer to the vector that contains the new reference points.
    * @return true/false upon success/failure
    */
    virtual yarp::dev::ReturnValue setPidReferencesRaw(const PidControlTypeEnum& pidtype, const double *refs) = 0;

    /** Set the error limit for the controller on a specific joint
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param limit limit value
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue setPidErrorLimitRaw(const PidControlTypeEnum& pidtype, int j, double limit) = 0;

    /** Get the error limit for the controller on all joints.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param limits pointer to the vector with the new limits
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue setPidErrorLimitsRaw(const PidControlTypeEnum& pidtype, const double *limits) = 0;

    /** Get the current error for a joint.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param err pointer to the storage for the return value
    * @return true/false on success failure
    */
    virtual yarp::dev::ReturnValue getPidErrorRaw(const PidControlTypeEnum& pidtype, int j, double *err) = 0;

    /** Get the error of all joints.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param errs pointer to the vector that will store the errors
    */
    virtual yarp::dev::ReturnValue getPidErrorsRaw(const PidControlTypeEnum& pidtype, double *errs) = 0;

    /** Get the output of the controller (e.g. pwm value)
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param out pointer to storage for return value
    * @return success/failure
    */
    virtual yarp::dev::ReturnValue getPidOutputRaw(const PidControlTypeEnum& pidtype, int j, double *out) = 0;

    /** Get the output of the controllers (e.g. pwm value)
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param outs pointer to the vector that will store the output values
    */
    virtual yarp::dev::ReturnValue getPidOutputsRaw(const PidControlTypeEnum& pidtype, double *outs) = 0;

    /** Get current PID value for a specific joint.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param pid pointer to storage for the return value.
    * @return success/failure
    */
    virtual yarp::dev::ReturnValue getPidRaw(const PidControlTypeEnum& pidtype, int j, Pid *pid) = 0;

    /** Get current PID value for a specific joint.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param pids vector that will store the values of the pids.
    * @return success/failure
    */
    virtual yarp::dev::ReturnValue getPidsRaw(const PidControlTypeEnum& pidtype, Pid *pids) = 0;

    /** Get PID extra info and measurement units for a specific joint.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param units the measurement units for the specified PID.
    * @return success/failure
    */
    virtual yarp::dev::ReturnValue getPidExtraInfoRaw(const PidControlTypeEnum& pidtype, int j, yarp::dev::PidExtraInfo& info) = 0;

    /** Get PID extra info and measurement units for a specific joint.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param units vector that will store the info about of the pids.
    * @return success/failure
    */
    virtual yarp::dev::ReturnValue getPidExtraInfosRaw(const PidControlTypeEnum& pidtype, std::vector<yarp::dev::PidExtraInfo>& info) = 0;

    /** Get the current reference of the PID controller for a specific joint.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param ref pointer to storage for return value
    * @return reference value
    */
    virtual yarp::dev::ReturnValue getPidReferenceRaw(const PidControlTypeEnum& pidtype, int j, double *ref) = 0;

    /** Get the current reference of all PID controllers.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param refs vector that will store the output.
    */
    virtual yarp::dev::ReturnValue getPidReferencesRaw(const PidControlTypeEnum& pidtype, double *refs) = 0;

    /** Get the error limit for the controller on a specific joint
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param limit pointer to storage
    * @return success/failure
    */
    virtual yarp::dev::ReturnValue getPidErrorLimitRaw(const PidControlTypeEnum& pidtype, int j, double *limit) = 0;

    /** Get the error limit for all controllers
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param limits pointer to the array that will store the output
    * @return success or failure
    */
    virtual yarp::dev::ReturnValue getPidErrorLimitsRaw(const PidControlTypeEnum& pidtype, double *limits) = 0;

    /** Reset the controller of a given joint, usually sets the current status
    * of the joint as the reference value for the PID, and resets the integrator.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue resetPidRaw(const PidControlTypeEnum& pidtype, int j) = 0;

    /** Disable the PID computation for a joint
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue disablePidRaw(const PidControlTypeEnum& pidtype, int j) = 0;

    /** Enable the PID computation for a joint
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue enablePidRaw(const PidControlTypeEnum& pidtype, int j) = 0;

    /** Set an offset value for a given controller. The value will added to the output of the PID controller
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param v the offset value
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue setPidOffsetRaw(const PidControlTypeEnum& pidtype, int j, double v) = 0;

    /** Set the feedforward value for the specified PID controller. This value will be multiplied by the PID feedforward gain.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param v the feedforward value
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue setPidFeedforwardRaw(const PidControlTypeEnum& pidtype, int j, double v) = 0;

    /** Get the currently set offset value for the specified PID controller.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param v the offset value
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue getPidOffsetRaw(const PidControlTypeEnum& pidtype, int j, double& v) = 0;

    /** Get the currently set feedforward value for the specified PID controller.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param v the feedforward value
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue getPidFeedforwardRaw(const PidControlTypeEnum& pidtype, int j, double& v) = 0;

   /** Get the current status (enabled/disabled) of the PID controller
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param enabled the current status of the PID controller.
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue isPidEnabledRaw(const PidControlTypeEnum& pidtype, int j, bool& enabled) = 0;
};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for a generic control board device implementing a PID controller,
 * with scaled arguments.
 */
class YARP_dev_API yarp::dev::IPidControl
{
public:
    /**
     * Destructor.
     */
    virtual ~IPidControl() {}

    /////////////////////////////////////////////////////////////////

    /** Return a list of available pid types mode for the given joint.
    * @param j joint number
    * @param avail a vector that will contain the list of available control modes for joint j.
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue getAvailablePids(int j, std::vector<PidControlTypeEnum>& avail) = 0;

    /** Set new PID value for a joint axis.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param pid new PID value
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue setPid(const PidControlTypeEnum& pidtype, int j, const Pid &pid) = 0;

    /** Set new PID value on multiple axes.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param pids pointer to a vector of pids
    * @return true/false upon success/failure
    */
    virtual yarp::dev::ReturnValue setPids(const PidControlTypeEnum& pidtype, const Pid *pids) = 0;

    /** Set the controller reference for a given axis.
    * Warning this method can result in very large torques
    * and should be used carefully. If you do not understand
    * this warning you should avoid using this method.
    * Have a look at other interfaces (e.g. position control).
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param ref new reference point
    * @return true/false upon success/failure
    */
    virtual yarp::dev::ReturnValue setPidReference(const PidControlTypeEnum& pidtype, int j, double ref) = 0;

    /** Set the controller reference, multiple axes.
    * Warning this method can result in very large torques
    * and should be used carefully. If you do not understand
    * this warning you should avoid using this method.
    * Have a look at other interfaces (e.g. position control).
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param refs pointer to the vector that contains the new reference points.
    * @return true/false upon success/failure
    */
    virtual yarp::dev::ReturnValue setPidReferences(const PidControlTypeEnum& pidtype, const double *refs) = 0;

    /** Set the error limit for the controller on a specific joint
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param limit limit value
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue setPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double limit) = 0;

    /** Get the error limit for the controller on all joints.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param limits pointer to the vector with the new limits
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue setPidErrorLimits(const PidControlTypeEnum& pidtype, const double *limits) = 0;

    /** Get the current error for a joint.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param err pointer to the storage for the return value
    * @return true/false on success failure
    */
    virtual yarp::dev::ReturnValue getPidError(const PidControlTypeEnum& pidtype, int j, double *err) = 0;

    /** Get the error of all joints.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param errs pointer to the vector that will store the errors
    */
    virtual yarp::dev::ReturnValue getPidErrors(const PidControlTypeEnum& pidtype, double *errs) = 0;

    /** Get the output of the controller (e.g. pwm value)
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param out pointer to storage for return value
    * @return success/failure
    */
    virtual yarp::dev::ReturnValue getPidOutput(const PidControlTypeEnum& pidtype, int j, double *out) = 0;

    /** Get the output of the controllers (e.g. pwm value)
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param outs pointer to the vector that will store the output values
    */
    virtual yarp::dev::ReturnValue getPidOutputs(const PidControlTypeEnum& pidtype, double *outs) = 0;

    /** Get current PID value for a specific joint.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param pid pointer to storage for the return value.
    * @return success/failure
    */
    virtual yarp::dev::ReturnValue getPid(const PidControlTypeEnum& pidtype, int j, Pid *pid) = 0;

    /** Get current PID value for a specific joint.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param pids vector that will store the values of the pids.
    * @return success/failure
    */
    virtual yarp::dev::ReturnValue getPids(const PidControlTypeEnum& pidtype, Pid *pids) = 0;

    /** Get PID extra info and measurement units for a specific joint.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param info the measurement units for the specified PID.
    * @return success/failure
    */
    virtual yarp::dev::ReturnValue getPidExtraInfo(const PidControlTypeEnum& pidtype, int j, yarp::dev::PidExtraInfo& info) = 0;

    /** Get PID extra info and measurement units for a specific joint.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param info vector that will store the info about of the pids.
    * @return success/failure
    */
    virtual yarp::dev::ReturnValue getPidExtraInfos(const PidControlTypeEnum& pidtype, std::vector<yarp::dev::PidExtraInfo>& info) = 0;

    /** Get the current reference of the PID controller for a specific joint.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param ref pointer to storage for return value
    * @return reference value
    */
    virtual yarp::dev::ReturnValue getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref) = 0;

    /** Get the current reference of all PID controllers.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param refs vector that will store the output.
    */
    virtual yarp::dev::ReturnValue getPidReferences(const PidControlTypeEnum& pidtype, double *refs) = 0;

    /** Get the error limit for the controller on a specific joint
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param limit pointer to storage
    * @return success/failure
    */
    virtual yarp::dev::ReturnValue getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *limit) = 0;

    /** Get the error limit for all controllers
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param limits pointer to the array that will store the output
    * @return success or failure
    */
    virtual yarp::dev::ReturnValue getPidErrorLimits(const PidControlTypeEnum& pidtype, double *limits) = 0;

    /** Reset the controller of a given joint, usually sets the current status of the joint
    * as the reference value for the PID, and resets the integrator.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue resetPid(const PidControlTypeEnum& pidtype, int j) = 0;

    /** Disable the PID computation for a joint
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue disablePid(const PidControlTypeEnum& pidtype, int j) = 0;

    /** Enable the PID computation for a joint
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue enablePid(const PidControlTypeEnum& pidtype, int j) = 0;

    /** Set an offset value for a given controller. The value will added to the output of the PID controller
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param v the offset value
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue setPidOffset(const PidControlTypeEnum& pidtype, int j, double v) = 0;

    /** Set the feedforward value for the specified PID controller. This value will be multiplied by the PID feedforward gain.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param v the feedforward value
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue setPidFeedforward(const PidControlTypeEnum& pidtype, int j, double v) = 0;

    /** Get the currently set offset value for the specified PID controller.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param v the offset value
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue getPidOffset(const PidControlTypeEnum& pidtype, int j, double& v) = 0;

    /** Get the currently set feedforward value for the specified PID controller.
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param v the feedforward value
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue getPidFeedforward(const PidControlTypeEnum& pidtype, int j, double& v) = 0;

    /** Get the current status (enabled/disabled) of the PID
    * @param pidtype the id of the PID that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param enabled the current status of the PID controller.
    * @return true on success, false on failure.
    */
    virtual  yarp::dev::ReturnValue isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool& enabled) = 0;
};

// Interface name
constexpr yarp::conf::vocab32_t VOCAB_PIDCONTROL_INTERFACE   = yarp::os::createVocab32('i','p','i','d');

// interface IPidControl
constexpr yarp::conf::vocab32_t VOCAB_PIDCONTROL_SET_OFFSET      = yarp::os::createVocab32('p', 'o', 'f','f');
constexpr yarp::conf::vocab32_t VOCAB_PIDCONTROL_SET_FEEDFORWARD = yarp::os::createVocab32('p', 'f', 'f', 'd');

constexpr yarp::conf::vocab32_t VOCAB_PID     = yarp::os::createVocab32('p','i','d');
constexpr yarp::conf::vocab32_t VOCAB_PIDS    = yarp::os::createVocab32('p','i','d','s');


#endif // YARP_DEV_CONTROLBOARDINTERFACES_H
