/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Lorenzo Natale <lorenzo.natale@iit.it> , Giorgio Metta <giorgio.metta@iit.it>, Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_PIDCONTROL_H
#define YARP_DEV_PIDCONTROL_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardPid.h>

namespace yarp
{
    namespace dev
    {
        class IPidControlRaw;
        class IPidControl;

        enum YARP_dev_API PidControlTypeEnum
        {
            VOCAB_PIDTYPE_POSITION = VOCAB3('p', 'o', 's'),
            VOCAB_PIDTYPE_VELOCITY = VOCAB3('v', 'e', 'l'),
            VOCAB_PIDTYPE_TORQUE   = VOCAB3('t', 'r', 'q'),
            VOCAB_PIDTYPE_CURRENT  = VOCAB3('c', 'u', 'r')
        };
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

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
    /** Set new position pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use setPidRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool setPidRaw(int j, const Pid  &pid) {return setPidRaw(VOCAB_PIDTYPE_POSITION, j, pid);}

    /** Set new position pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use setPidsRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool setPidsRaw(const Pid *pids)  {return setPidsRaw(VOCAB_PIDTYPE_POSITION, pids);}


    /** Set the controller reference point for a given axis.
     * Warning this method can result in very large torques
     * and should be used carefully. If you do not understand
     * this warning you should avoid using this method.
     * Have a look at other interfaces (e.g. position control).
     * @param j joint number
     * @param ref new reference point
     * @return true/false upon success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use setPidReferenceRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool setReferenceRaw(int j, double ref)   {return setPidReferenceRaw(VOCAB_PIDTYPE_POSITION, j, ref);}

    /** Set the controller reference points, multiple axes.
     * Warning this method can result in very large torques
     * and should be used carefully. If you do not understand
     * this warning you should avoid using this method.
     * Have a look at other interfaces (e.g. position control).
     * @param refs pointer to the vector that contains the new reference points.
     * @return true/false upon success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use setPidReferencesRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool setReferencesRaw(const double *refs)  {return setPidReferencesRaw(VOCAB_PIDTYPE_POSITION, refs);}

    /** Set the error limit for the position controller on a specific joint
     * @param j joint number
     * @param limit limit value
     * @return true/false on success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use setPidErrorLimitRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool setErrorLimitRaw(int j, double limit)  {return setPidErrorLimitRaw(VOCAB_PIDTYPE_POSITION, j, limit);}

    /** Get the error limit for the position controller on all joints.
     * @param limits pointer to the vector with the new limits
     * @return true/false on success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use setPidErrorLimitsRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool setErrorLimitsRaw(const double *limits) {return setPidErrorLimitsRaw(VOCAB_PIDTYPE_POSITION, limits);}

    /** Get the current position error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidErrorRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getErrorRaw(int j, double *err)  {return getPidErrorRaw(VOCAB_PIDTYPE_POSITION, j, err);}

    /** Get the position error of all joints.
     * @param errs pointer to the vector that will store the errors
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidErrorsRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getErrorsRaw(double *errs) {return getPidErrorsRaw(VOCAB_PIDTYPE_POSITION, errs);}

    /** Get the output of the position controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidOutputRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getOutputRaw(int j, double *out) {return getPidOutputRaw(VOCAB_PIDTYPE_POSITION, j, out);}

    /** Get the output of the position controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidOutputsRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getOutputsRaw(double *outs) {return getPidOutputsRaw(VOCAB_PIDTYPE_POSITION, outs);}

    /** Get current position pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getPidRaw(int j, Pid *pid) {return getPidRaw(VOCAB_PIDTYPE_POSITION, j ,pid);}

    /** Get current position pid value for a specific joint.
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidsRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getPidsRaw(Pid *pids) {return getPidsRaw(VOCAB_PIDTYPE_POSITION, pids);}

    /** Get the current reference position of the controller for a specific joint.
     * @param j joint number
     * @param ref pointer to storage for return value
     * @return reference value
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidReferenceRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getReferenceRaw(int j, double *ref) {return getPidReferenceRaw(VOCAB_PIDTYPE_POSITION, j ,ref);}

    /** Get the current reference position of all controllers.
     * @param refs vector that will store the output.
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidReferencesRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getReferencesRaw(double *refs) {return getPidReferencesRaw(VOCAB_PIDTYPE_POSITION, refs);}

    /** Get the position error limit for the controller on a specific joint
     * @param j joint number
     * @param limit pointer to storage
     * @return success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidErrorLimitRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getErrorLimitRaw(int j, double *limit) {return getPidErrorLimitRaw(VOCAB_PIDTYPE_POSITION, j, limit);}

    /** Get the position error limit for all controllers
     * @param limits pointer to the array that will store the output
     * @return success or failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidErrorLimitsRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getErrorLimitsRaw(double *limits) {return getPidErrorLimitsRaw(VOCAB_PIDTYPE_POSITION, limits);}

    /** Reset the position controller of a given joint, usually sets the
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     * @return true on success, false on failure.
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use resetPidRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool resetPidRaw(int j)  {return resetPidRaw(VOCAB_PIDTYPE_POSITION, j);}

    /** Disable the pid computation for a joint
    * @deprecated since YARP 2.3.68*/
    YARP_DEPRECATED_MSG("Use disablePidRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool disablePidRaw(int j)  {return disablePidRaw(VOCAB_PIDTYPE_POSITION, j);}

    /** Enable the pid computation for a joint
    * @deprecated since YARP 2.3.68*/
    YARP_DEPRECATED_MSG("Use enablePidRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool enablePidRaw(int j)  {return enablePidRaw(VOCAB_PIDTYPE_POSITION, j);}

    /** Set offset value for a given position controller
    * @deprecated since YARP 2.3.68*/
    YARP_DEPRECATED_MSG("Use setPidOffsetRaw(VOCAB_PIDTYPE_POSITION,...) instead")
    bool setOffsetRaw(int j, double v)  {return setPidOffsetRaw(VOCAB_PIDTYPE_POSITION, j, v);}
#endif //#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70

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

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
    /** Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use the setPid(VOCAB_PIDTYPE_POSITION,...) instead")
    bool setPid(int j, const Pid &pid) {return setPid(VOCAB_PIDTYPE_POSITION, j ,pid);}

    /** Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use the setPids(VOCAB_PIDTYPE_POSITION,...) instead")
    bool setPids(const Pid *pids)  {return setPids(VOCAB_PIDTYPE_POSITION, pids);}

    /** Set the controller reference point for a given axis.
     * Warning this method can result in very large torques
     * and should be used carefully. If you do not understand
     * this warning you should avoid using this method.
     * Have a look at other interfaces (e.g. position control).
     * @param j joint number
     * @param ref new reference point
     * @return true/false upon success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use setPidReference(VOCAB_PIDTYPE_POSITION,...) instead")
    bool setReference(int j, double ref)  {return setPidReference(VOCAB_PIDTYPE_POSITION, j ,ref);}

    /** Set the controller reference points, multiple axes.
     * Warning this method can result in very large torques
     * and should be used carefully. If you do not understand
     * this warning you should avoid using this method.
     * Have a look at other interfaces (e.g. position control).
     * @param refs pointer to the vector that contains the new reference points.
     * @return true/false upon success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use setPidReferences(VOCAB_PIDTYPE_POSITION,...) instead")
    bool setReferences(const double *refs) {return setPidReferences(VOCAB_PIDTYPE_POSITION, refs);}

    /** Set the error limit for the controller on a specifi joint
     * @param j joint number
     * @param limit limit value
     * @return true/false on success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use setPidErrorLimit(VOCAB_PIDTYPE_POSITION,...) instead")
    bool setErrorLimit(int j, double limit) {return setPidErrorLimit(VOCAB_PIDTYPE_POSITION, j, limit);}

    /** Get the error limit for the controller on all joints.
     * @param limits pointer to the vector with the new limits
     * @return true/false on success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use setPidErrorLimits(VOCAB_PIDTYPE_POSITION,...) instead")
    bool setErrorLimits(const double *limits) {return setPidErrorLimits(VOCAB_PIDTYPE_POSITION, limits);}

    /** Get the current error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidError(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getError(int j, double *err) {return getPidError(VOCAB_PIDTYPE_POSITION, j, err);}

    /** Get the error of all joints.
     * @param errs pointer to the vector that will store the errors
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidErrors(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getErrors(double *errs)  {return getPidErrors(VOCAB_PIDTYPE_POSITION, errs);}

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidOutput(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getOutput(int j, double *out)  {return getPidOutput(VOCAB_PIDTYPE_POSITION, j, out);}

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidOutputs(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getOutputs(double *outs)   {return getPidOutputs(VOCAB_PIDTYPE_POSITION, outs);}

    /** Get current pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPid(VOCAB_PIDTYPE_POSITION,...)  instead")
    bool getPid(int j, Pid *pid) {return getPid(VOCAB_PIDTYPE_POSITION, j, pid);}

    /** Get current pid value for a specific joint.
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPids(VOCAB_PIDTYPE_POSITION,...)  instead")
    bool getPids(Pid *pids) {return getPids(VOCAB_PIDTYPE_POSITION, pids);}

    /** Get the current reference position of the controller for a specific joint.
     * @param j joint number
     * @param ref pointer to storage for return value
     * @return reference value
     */
    YARP_DEPRECATED_MSG("Use getPidReference(VOCAB_PIDTYPE_POSITION,...) instead")
    bool getReference(int j, double *ref) {return getPidReference(VOCAB_PIDTYPE_POSITION, j, ref);}

    /** Get the current reference position of all controllers.
     * @param refs vector that will store the output.
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidReferences(VOCAB_PIDTYPE_POSITION,...)  instead")
    bool getReferences(double *refs) {return getPidReferences(VOCAB_PIDTYPE_POSITION, refs);}

    /** Get the error limit for the controller on a specific joint
     * @param j joint number
     * @param limit pointer to storage
     * @return success/failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidErrorLimit(VOCAB_PIDTYPE_POSITION,...)  instead")
    bool getErrorLimit(int j, double *limit) {return getPidErrorLimit(VOCAB_PIDTYPE_POSITION, j, limit);}

    /** Get the error limit for all controllers
     * @param limits pointer to the array that will store the output
     * @return success or failure
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use getPidErrorLimits(VOCAB_PIDTYPE_POSITION,...)  instead")
    bool getErrorLimits(double *limits) {return getPidErrorLimits(VOCAB_PIDTYPE_POSITION, limits);}

    /** Reset the controller of a given joint, usually sets the
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     * @return true on success, false on failure.
     * @deprecated since YARP 2.3.68
     */
    YARP_DEPRECATED_MSG("Use resetPid(VOCAB_PIDTYPE_POSITION,...)  instead")
    bool resetPid(int j) {return resetPid(VOCAB_PIDTYPE_POSITION, j);}

    /** Disable the pid computation for a joint
    *@deprecated since YARP 2.3.68*/
    YARP_DEPRECATED_MSG("Use disablePid(VOCAB_PIDTYPE_POSITION,...)  instead")
    bool disablePid(int j) {return disablePid(VOCAB_PIDTYPE_POSITION, j);}

    /** Enable the pid computation for a joint
    *@deprecated since YARP 2.3.68*/
    YARP_DEPRECATED_MSG("Use enablePid(VOCAB_PIDTYPE_POSITION,...)  instead")
    bool enablePid(int j) {return enablePid(VOCAB_PIDTYPE_POSITION, j);}

    /** Set offset value for a given controller
    *@deprecated since YARP 2.3.68*/
    YARP_DEPRECATED_MSG("Use setPidOffset(VOCAB_PIDTYPE_POSITION,...)  instead")
    bool setOffset(int j, double v) {return setPidOffset(VOCAB_PIDTYPE_POSITION, j ,v);}
#endif //#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70

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
