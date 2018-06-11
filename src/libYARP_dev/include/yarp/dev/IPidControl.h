/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_PIDCONTROL_H
#define YARP_DEV_PIDCONTROL_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/PidEnums.h>
#include <yarp/dev/ControlBoardPid.h>

namespace yarp
{
    namespace dev
    {
        class IPidControlRaw;
        class IPidControl;
    }
}


/**
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
    /** Set new pid value for a joint axis.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param pid new pid value
    * @return true/false on success/failure
    */
    virtual bool setPidRaw(const PidControlTypeEnum& pidtype, int j, const Pid &pid) = 0;

    /** Set new pid value on multiple axes.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param pids pointer to a vector of pids
    * @return true/false upon success/failure
    */
    virtual bool setPidsRaw(const PidControlTypeEnum& pidtype, const Pid *pids) = 0;

    /** Set the controller reference for a given axis.
    * Warning this method can result in very large torques
    * and should be used carefully. If you do not understand
    * this warning you should avoid using this method.
    * Have a look at other interfaces (e.g. position control).
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param ref new reference point
    * @return true/false upon success/failure
    */
    virtual bool setPidReferenceRaw(const PidControlTypeEnum& pidtype, int j, double ref) = 0;

    /** Set the controller reference, multiple axes.
    * Warning this method can result in very large torques
    * and should be used carefully. If you do not understand
    * this warning you should avoid using this method.
    * Have a look at other interfaces (e.g. position control).
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param refs pointer to the vector that contains the new reference points.
    * @return true/false upon success/failure
    */
    virtual bool setPidReferencesRaw(const PidControlTypeEnum& pidtype, const double *refs) = 0;

    /** Set the error limit for the controller on a specific joint
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param limit limit value
    * @return true/false on success/failure
    */
    virtual bool setPidErrorLimitRaw(const PidControlTypeEnum& pidtype, int j, double limit) = 0;

    /** Get the error limit for the controller on all joints.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param limits pointer to the vector with the new limits
    * @return true/false on success/failure
    */
    virtual bool setPidErrorLimitsRaw(const PidControlTypeEnum& pidtype, const double *limits) = 0;

    /** Get the current error for a joint.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param err pointer to the storage for the return value
    * @return true/false on success failure
    */
    virtual bool getPidErrorRaw(const PidControlTypeEnum& pidtype, int j, double *err) = 0;

    /** Get the error of all joints.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param errs pointer to the vector that will store the errors
    */
    virtual bool getPidErrorsRaw(const PidControlTypeEnum& pidtype, double *errs) = 0;

    /** Get the output of the controller (e.g. pwm value)
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param out pointer to storage for return value
    * @return success/failure
    */
    virtual bool getPidOutputRaw(const PidControlTypeEnum& pidtype, int j, double *out) = 0;

    /** Get the output of the controllers (e.g. pwm value)
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param outs pinter to the vector that will store the output values
    */
    virtual bool getPidOutputsRaw(const PidControlTypeEnum& pidtype, double *outs) = 0;

    /** Get current pid value for a specific joint.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param pid pointer to storage for the return value.
    * @return success/failure
    */
    virtual bool getPidRaw(const PidControlTypeEnum& pidtype, int j, Pid *pid) = 0;

    /** Get current pid value for a specific joint.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param pids vector that will store the values of the pids.
    * @return success/failure
    */
    virtual bool getPidsRaw(const PidControlTypeEnum& pidtype, Pid *pids) = 0;

    /** Get the current reference of the pid controller for a specific joint.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param ref pointer to storage for return value
    * @return reference value
    */
    virtual bool getPidReferenceRaw(const PidControlTypeEnum& pidtype, int j, double *ref) = 0;

    /** Get the current reference of all pid controllers.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param refs vector that will store the output.
    */
    virtual bool getPidReferencesRaw(const PidControlTypeEnum& pidtype, double *refs) = 0;

    /** Get the error limit for the controller on a specific joint
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param limit pointer to storage
    * @return success/failure
    */
    virtual bool getPidErrorLimitRaw(const PidControlTypeEnum& pidtype, int j, double *limit) = 0;

    /** Get the error limit for all controllers
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param limits pointer to the array that will store the output
    * @return success or failure
    */
    virtual bool getPidErrorLimitsRaw(const PidControlTypeEnum& pidtype, double *limits) = 0;

    /** Reset the controller of a given joint, usually sets the current status
    * of the joint as the reference value for the PID, and resets the integrator.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual bool resetPidRaw(const PidControlTypeEnum& pidtype, int j) = 0;

    /** Disable the pid computation for a joint
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual bool disablePidRaw(const PidControlTypeEnum& pidtype, int j) = 0;

    /** Enable the pid computation for a joint
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual bool enablePidRaw(const PidControlTypeEnum& pidtype, int j) = 0;

    /** Set an offset value on the ourput of pid controller
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param v the offset to be added to the output of the pid controller
    * @return true on success, false on failure.
    */
    virtual bool setPidOffsetRaw(const PidControlTypeEnum& pidtype, int j, double v) = 0;

   /** Get the current status (enabled/disabled) of the pid controller
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param enabled the current status of the pid controller.
    * @return true on success, false on failure.
    */
    virtual bool isPidEnabledRaw(const PidControlTypeEnum& pidtype, int j, bool* enabled) = 0;
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
    /** Set new pid value for a joint axis.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param pid new pid value
    * @return true/false on success/failure
    */
    virtual bool setPid(const PidControlTypeEnum& pidtype, int j, const Pid &pid) = 0;

    /** Set new pid value on multiple axes.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param pids pointer to a vector of pids
    * @return true/false upon success/failure
    */
    virtual bool setPids(const PidControlTypeEnum& pidtype, const Pid *pids) = 0;

    /** Set the controller reference for a given axis.
    * Warning this method can result in very large torques
    * and should be used carefully. If you do not understand
    * this warning you should avoid using this method.
    * Have a look at other interfaces (e.g. position control).
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param ref new reference point
    * @return true/false upon success/failure
    */
    virtual bool setPidReference(const PidControlTypeEnum& pidtype, int j, double ref) = 0;

    /** Set the controller reference, multiple axes.
    * Warning this method can result in very large torques
    * and should be used carefully. If you do not understand
    * this warning you should avoid using this method.
    * Have a look at other interfaces (e.g. position control).
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param refs pointer to the vector that contains the new reference points.
    * @return true/false upon success/failure
    */
    virtual bool setPidReferences(const PidControlTypeEnum& pidtype, const double *refs) = 0;

    /** Set the error limit for the controller on a specifi joint
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param limit limit value
    * @return true/false on success/failure
    */
    virtual bool setPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double limit) = 0;

    /** Get the error limit for the controller on all joints.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param limits pointer to the vector with the new limits
    * @return true/false on success/failure
    */
    virtual bool setPidErrorLimits(const PidControlTypeEnum& pidtype, const double *limits) = 0;

    /** Get the current error for a joint.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param err pointer to the storage for the return value
    * @return true/false on success failure
    */
    virtual bool getPidError(const PidControlTypeEnum& pidtype, int j, double *err) = 0;

    /** Get the error of all joints.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param errs pointer to the vector that will store the errors
    */
    virtual bool getPidErrors(const PidControlTypeEnum& pidtype, double *errs) = 0;

    /** Get the output of the controller (e.g. pwm value)
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param out pointer to storage for return value
    * @return success/failure
    */
    virtual bool getPidOutput(const PidControlTypeEnum& pidtype, int j, double *out) = 0;

    /** Get the output of the controllers (e.g. pwm value)
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param outs pinter to the vector that will store the output values
    */
    virtual bool getPidOutputs(const PidControlTypeEnum& pidtype, double *outs) = 0;

    /** Get current pid value for a specific joint.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param pid pointer to storage for the return value.
    * @return success/failure
    */
    virtual bool getPid(const PidControlTypeEnum& pidtype, int j, Pid *pid) = 0;

    /** Get current pid value for a specific joint.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param pids vector that will store the values of the pids.
    * @return success/failure
    */
    virtual bool getPids(const PidControlTypeEnum& pidtype, Pid *pids) = 0;

    /** Get the current reference of the pid controller for a specific joint.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param ref pointer to storage for return value
    * @return reference value
    */
    virtual bool getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref) = 0;

    /** Get the current reference of all pid controllers.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param refs vector that will store the output.
    */
    virtual bool getPidReferences(const PidControlTypeEnum& pidtype, double *refs) = 0;

    /** Get the error limit for the controller on a specific joint
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param limit pointer to storage
    * @return success/failure
    */
    virtual bool getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *limit) = 0;

    /** Get the error limit for all controllers
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param limits pointer to the array that will store the output
    * @return success or failure
    */
    virtual bool getPidErrorLimits(const PidControlTypeEnum& pidtype, double *limits) = 0;

    /** Reset the controller of a given joint, usually sets the current status of the joint
    * as the reference value for the PID, and resets the integrator.
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual bool resetPid(const PidControlTypeEnum& pidtype, int j) = 0;

    /** Disable the pid computation for a joint
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual bool disablePid(const PidControlTypeEnum& pidtype, int j) = 0;

    /** Enable the pid computation for a joint
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @return true on success, false on failure.
    */
    virtual bool enablePid(const PidControlTypeEnum& pidtype, int j) = 0;

    /** Set offset value for a given controller
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param v the offset to be added to the output of the pid controller
    * @return true on success, false on failure.
    */
    virtual bool setPidOffset(const PidControlTypeEnum& pidtype, int j, double v) = 0;

    /** Get the current status (enabled/disabled) of the pid
    * @param pidtype the id of the pid that will be affected by the command (e.g. position, velocity etc)
    * @param j joint number
    * @param enabled the current status of the pid controller.
    * @return true on success, false on failure.
    */
    virtual bool isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool* enabled) = 0;
};

/* Vocabs representing the above interfaces */

#define VOCAB_SET VOCAB3('s','e','t')
#define VOCAB_GET VOCAB3('g','e','t')
#define VOCAB_IS VOCAB2('i','s')
#define VOCAB_FAILED VOCAB4('f','a','i','l')
#define VOCAB_OK VOCAB2('o','k')

#define VOCAB_OFFSET VOCAB3('o', 'f', 'f')

// interface IPidControl sets.
#define VOCAB_PID VOCAB3('p','i','d')
#define VOCAB_PIDS VOCAB4('p','i','d','s')
#define VOCAB_REF VOCAB3('r','e','f')
#define VOCAB_REFS VOCAB4('r','e','f','s')
#define VOCAB_REFG VOCAB4('r','e','f','g')
#define VOCAB_LIM VOCAB3('l','i','m')
#define VOCAB_LIMS VOCAB4('l','i','m','s')
#define VOCAB_RESET VOCAB3('r','e','s')
#define VOCAB_DISABLE VOCAB3('d','i','s')
#define VOCAB_ENABLE VOCAB3('e','n','a')

// interface IPidControl gets.
#define VOCAB_ERR VOCAB3('e','r','r')
#define VOCAB_ERRS VOCAB4('e','r','r','s')
#define VOCAB_OUTPUT VOCAB3('o','u','t')
#define VOCAB_OUTPUTS VOCAB4('o','u','t','s')
#define VOCAB_REFERENCE VOCAB3('r','e','f')
#define VOCAB_REFERENCES VOCAB4('r','e','f','s')

// interface IPositionControl gets
#define VOCAB_AXES VOCAB4('a','x','e','s')

#endif // YARP_DEV_CONTROLBOARDINTERFACES_H
