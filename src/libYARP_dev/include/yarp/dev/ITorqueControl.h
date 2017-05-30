/*
 * Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_ITORQUECONTROL_H
#define YARP_DEV_ITORQUECONTROL_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/ControlBoardPid.h>

//TO PROVIDE BACKWARD COMPATIBILITY FOR DEPRECATED METHODS: TO BE REMOVED LATER!
#include <yarp/dev/IPidControl.h>
#include <yarp/os/LogStream.h>

namespace yarp {
    namespace dev {
        class ITorqueControlRaw;
        class ITorqueControl;
        class MotorTorqueParameters;
      }
}

class YARP_dev_API yarp::dev::MotorTorqueParameters
{
    public:
    double bemf;
    double bemf_scale;
    double ktau;
    double ktau_scale;
    MotorTorqueParameters() : bemf(0), bemf_scale(0), ktau(0), ktau_scale(0) {};
};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control boards implementing torque control.
 */
class YARP_dev_API yarp::dev::ITorqueControl
{
public:
    /**
     * Destructor.
     */
    virtual ~ITorqueControl() {}

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     */
    virtual bool getAxes(int *ax) = 0;

   /** Get the reference value of the torque for all joints.
     * This is NOT the feedback (see getTorques instead).
     * @param t pointer to the array of torque values
     * @return true/false on success/failure
     */
    virtual bool getRefTorques(double *t)=0;

    /** Get the reference value of the torque for a given joint.
     * This is NOT the feedback (see getTorque instead).
     * @param j joint number
     * @param t the returned reference torque of joint j
     * @return true/false on success/failure
     */
    virtual bool getRefTorque(int j, double *t)=0;

    /** Set the reference value of the torque for all joints.
     * @param t pointer to the array of torque values
     * @return true/false on success/failure
     */
    virtual bool setRefTorques(const double *t)=0;

    /** Set the reference value of the torque for a given joint.
     * @param j joint number
     * @param t new value
     * @return true/false on success/failure
     */
    virtual bool setRefTorque(int j, double t)=0;

    /** Set new torque reference for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param refs   pointer to the array specifing the new torque reference
     * @return true/false on success/failure
     */
    virtual bool setRefTorques(const int n_joint, const int *joints, const double *t) {return false;}  // this function has a default implementation to keep backward compatibility with existing devices

    /** Get the back-emf compensation gain for a given joint.
     * @param j joint number
     * @param bemf the returned bemf gain of joint j
     * @return true/false on success/failure
     */
    virtual bool getBemfParam(int j, double *bemf)=0;

    /** Set the back-emf compensation gain for a given joint.
     * @param j joint number
     * @param bemf new value
     * @return true/false on success/failure
     */
    virtual bool setBemfParam(int j, double bemf)=0;

    /** Get a subset of motor parameters (bemf, ktau etc) useful for torque control.
     * @param j joint number
     * @param params a struct containing the motor parameters to be retrieved
     * @return true/false on success/failure
     */
    virtual bool getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params) {return false;}

    /** Set a subset of motor parameters (bemf, ktau etc) useful for torque control.
     * @param j joint number
     * @param params a struct containing the motor parameters to be set
     * @return true/false on success/failure
     */
    virtual bool setMotorTorqueParams(int j,  const yarp::dev::MotorTorqueParameters params) {return false;}

    /** Get the value of the torque on a given joint (this is the
     * feedback if you have a torque sensor).
     * @param j joint number
     * @param t pointer to the result value
     * @return true/false on success/failure
     */
    virtual bool getTorque(int j, double *t)=0;

    /** Get the value of the torque for all joints (this is
     * the feedback if you have torque sensors).
     * @param t pointer to the array that will store the output
     * @return true/false on success/failure
     */
    virtual bool getTorques(double *t)=0;

    /** Get the full scale of the torque sensor of a given joint
     * @param j joint number
     * @param min minimum torque of the joint j
     * @param max maximum torque of the joint j
     * @return true/false on success/failure
     */
    virtual bool getTorqueRange(int j, double *min, double *max)=0;

    /** Get the full scale of the torque sensors of all joints
     * @param min pointer to the array that will store minimum torques of the joints
     * @param max pointer to the array that will store maximum torques of the joints
     * @return true/false on success/failure
     */
    virtual bool getTorqueRanges(double *min, double *max)=0;

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.68

     /** Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use setPid(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool setTorquePid(int j, const Pid &pid)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->setPid(VOCAB_PIDTYPE_TORQUE,j,pid);} else {yError()<< "setTorquePid is DEPRECATED, use setPid(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}


    /** Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    YARP_DEPRECATED_MSG("Use setPids(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool setTorquePids(const Pid *pids)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->setPids(VOCAB_PIDTYPE_TORQUE,pids);} else {yError()<< "setTorquePids is DEPRECATED, use setPids(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Set the torque error limit for the controller on a specific joint
     * @param j joint number
     * @param limit limit value
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use setPidErrorLimits(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool setTorqueErrorLimit(int j, double limit)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->setPidErrorLimit(VOCAB_PIDTYPE_TORQUE,j,limit);} else {yError()<< "setTorqueErrorLimit is DEPRECATED, use setPidErrorLimit(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}


    /** Get the torque error limit for the controller on all joints.
     * @param limits pointer to the vector with the new limits
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use setPidErrorLimits(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool setTorqueErrorLimits(const double *limits)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->setPidErrorLimits(VOCAB_PIDTYPE_TORQUE,limits);} else {yError()<< "setTorqueErrorLimits is DEPRECATED, use setPidErrorLimits(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}


    /** Get the current torque error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    YARP_DEPRECATED_MSG("Use getPidError(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorqueError(int j, double *err)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->getPidError(VOCAB_PIDTYPE_TORQUE,j,err);} else {yError()<< "getTorqueError is DEPRECATED, use getPidError(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Get the torque error of all joints.
     * @param errs pointer to the vector that will store the errors
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use getPidErrors(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorqueErrors(double *errs)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->getPidErrors(VOCAB_PIDTYPE_TORQUE,errs);} else {yError()<< "getTorqueErrors is DEPRECATED, use getPidErrors(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}


    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use getPidOutput(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorquePidOutput(int j, double *out)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->getPidOutput(VOCAB_PIDTYPE_TORQUE,j,out);} else {yError()<< "getTorquePidOutput is DEPRECATED, use getPidOutput(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}


    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pointer to the vector that will store the output values
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use getPidOutputs(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorquePidOutputs(double *outs)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->getPidOutputs(VOCAB_PIDTYPE_TORQUE,outs);} else {yError()<< "getTorquePidOutputs is DEPRECATED, use getPidOutputs(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}


    /** Get current pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use getPid(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorquePid(int j, Pid *pid)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->getPid(VOCAB_PIDTYPE_TORQUE,j,pid);} else {yError()<< "getTorquePid is DEPRECATED, use getPid(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Get current pid value for a specific joint.
     * @param pids vector that will store the values of the pids.
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use getPid(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorquePids(Pid *pids)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->getPids(VOCAB_PIDTYPE_TORQUE,pids);} else {yError()<< "getTorquePids is DEPRECATED, use getPids(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Get the torque error limit for the controller on a specific joint
     * @param j joint number
     * @param limit pointer to the result value
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use getPidErrorLimit(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorqueErrorLimit(int j, double *limit)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->getPidErrorLimit(VOCAB_PIDTYPE_TORQUE,j,limit);} else {yError()<< "getTorqueErrorLimit is DEPRECATED, use getPidErrorLimit(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Get the torque error limit for all controllers
     * @param limits pointer to the array that will store the output
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use getPidErrorLimits(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorqueErrorLimits(double *limits)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->getPidErrorLimits(VOCAB_PIDTYPE_TORQUE,limits);} else {yError()<< "getTorqueErrorLimits is DEPRECATED, use getPidErrorLimits(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Reset the controller of a given joint, usually sets the
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use resetPid(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool resetTorquePid(int j)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->resetPid(VOCAB_PIDTYPE_TORQUE, j);} else {yError()<< "resetTorquePid is DEPRECATED, use resetPid(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}


    /** Disable the pid computation for a joint
     * @param j joint number
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use disablePid(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool disableTorquePid(int j)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->disablePid(VOCAB_PIDTYPE_TORQUE, j);} else {yError()<< "disableTorquePid is DEPRECATED, use disablePid(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}


    /** Enable the pid computation for a joint
     * @param j joint number
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use enablePid(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool enableTorquePid(int j)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->enablePid(VOCAB_PIDTYPE_TORQUE, j);} else {yError()<< "enableTorquePid is DEPRECATED, use enablePid(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}


    /** Set offset value for a given pid
     * @param j joint number
     * @param v the new value
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use setPidOffset(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool setTorqueOffset(int j, double v)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->setPidOffset(VOCAB_PIDTYPE_TORQUE,j,v);} else {yError()<< "setTorqueOffset is DEPRECATED, use setPidOffset(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}


#endif
};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control boards implementing torque control.
 */
class yarp::dev::ITorqueControlRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~ITorqueControlRaw() {}

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     * @return true/false on success/failure
     */
    virtual bool getAxes(int *ax) = 0;

    /** Get the value of the torque on a given joint (this is the
     * feedback if you have a torque sensor).
     * @param j joint number
     * @return torque value
     * @return true/false on success/failure
     */
    virtual bool getTorqueRaw(int j, double *t)=0;

    /** Get the value of the torque for all joints (this is
     * the feedback if you have torque sensors).
     * @param t pointer to the array that will store the output
     * @return true/false on success/failure
     */
    virtual bool getTorquesRaw(double *t)=0;

    /** Get the full scale of the torque sensor of a given joint
     * @param j joint number
     * @param min minimum torque of the joint j
     * @param max maximum torque of the joint j
     * @return true/false on success/failure
     */
    virtual bool getTorqueRangeRaw(int j, double *min, double *max)=0;

    /** Get the full scale of the torque sensors of all joints
     * @param min pointer to the array that will store minimum torques of the joints
     * @param max pointer to the array that will store maximum torques of the joints
     * @return true/false on success/failure
     */
    virtual bool getTorqueRangesRaw(double *min, double *max)=0;

    /** Set the reference value of the torque for all joints.
     * @param t pointer to the array of torque values
     * @return true/false on success/failure
     */
    virtual bool setRefTorquesRaw(const double *t)=0;

    /** Set the reference value of the torque for a given joint.
     * @param j joint number
     * @param t new value
     * @return true/false on success/failure
     */
    virtual bool setRefTorqueRaw(int j, double t)=0;

    /** Set new torque reference for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param refs   pointer to the array specifing the new torque reference
     * @return true/false on success/failure
     */
    virtual bool setRefTorquesRaw(const int n_joint, const int *joints, const double *t) {return false;}  // this function has a default implementation to keep backward compatibility with existing devices

    /** Get the reference value of the torque for all joints.
     * This is NOT the feedback (see getTorques instead).
     * @param t pointer to the array of torque values
     * @return true/false on success/failure
     */
    virtual bool getRefTorquesRaw(double *t)=0;

    /** Set the reference value of the torque for a given joint.
     * This is NOT the feedback (see getTorque instead).
     * @param j joint number
     * @param t new value
     * @return true/false on success/failure
     */
    virtual bool getRefTorqueRaw(int j, double *t)=0;

    /** Set the back-efm compensation gain for a given joint.
     * @param j joint number
     * @param bemf the returned bemf gain of joint j
     * @return true/false on success/failure
     */
    virtual bool getBemfParamRaw(int j, double *bemf)=0;

    /** Set the back-efm compensation gain for a given joint.
     * @param j joint number
     * @param bemf new value
     * @return true/false on success/failure
     */
    virtual bool setBemfParamRaw(int j, double bemf)=0;

    /** Get the motor parameters.
     * @param j joint number
     * @param params a struct containing the motor parameters to be retrieved
     * @return true/false on success/failure
     */
    virtual bool getMotorTorqueParamsRaw(int j,  yarp::dev::MotorTorqueParameters *params) {return false;}

    /** Set the motor parameters.
     * @param j joint number
     * @param params a struct containing the motor parameters to be set
     * @return true/false on success/failure
     */
    virtual bool setMotorTorqueParamsRaw(int j,  const yarp::dev::MotorTorqueParameters params) {return false;}

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.68
     /** Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use setPidRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool setTorquePidRaw(int j, const Pid &pid)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->setPidRaw(VOCAB_PIDTYPE_TORQUE,j,pid);} else {yError()<< "setTorquePidRaw is DEPRECATED, use setPidRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
     YARP_DEPRECATED_MSG("Use setPidsRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool setTorquePidsRaw(const Pid *pids)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->setPidsRaw(VOCAB_PIDTYPE_TORQUE,pids);} else {yError()<< "setTorquePidsRaw is DEPRECATED, use setPidsRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Set the torque error limit for the controller on a specific joint
     * @param j joint number
     * @param limit limit value
     * @return true/false on success/failure
     */
     YARP_DEPRECATED_MSG("Use setPidErrorLimitRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool setTorqueErrorLimitRaw(int j, double limit)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->setPidErrorLimitRaw(VOCAB_PIDTYPE_TORQUE,j,limit);} else {yError()<< "setTorqueErrorLimitRaw is DEPRECATED, use setPidErrorLimitRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Get the torque error limit for the controller on all joints.
     * @param limits pointer to the vector with the new limits
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use setPidErrorLimitsRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool setTorqueErrorLimitsRaw(const double *limits)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->setPidErrorLimitsRaw(VOCAB_PIDTYPE_TORQUE,limits);} else {yError()<< "setTorqueErrorLimitsRaw is DEPRECATED, use setPidErrorLimitsRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Get the current torque error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    YARP_DEPRECATED_MSG("Use getPidErrorRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorqueErrorRaw(int j, double *err)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->getPidErrorRaw(VOCAB_PIDTYPE_TORQUE,j,err);} else {yError()<< "getTorqueErrorRaw is DEPRECATED, use getPidErrorRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Get the torque error of all joints.
     * @param errs pointer to the vector that will store the errors
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use getPidErrorsRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorqueErrorsRaw(double *errs)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->getPidErrorsRaw(VOCAB_PIDTYPE_TORQUE,errs);} else {yError()<< "getTorqueErrorsRaw is DEPRECATED, use getPidErrorsRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use getPidOutputRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorquePidOutputRaw(int j, double *out)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->getPidOutputRaw(VOCAB_PIDTYPE_TORQUE,j,out);} else {yError()<< "getTorquePidOutputRaw is DEPRECATED, use getPidOutputRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use getPidOutputsRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorquePidOutputsRaw(double *outs)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->getPidOutputsRaw(VOCAB_PIDTYPE_TORQUE,outs);} else {yError()<< "getTorquePidOutputsRaw is DEPRECATED, use getPidOutputsRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Get current pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use getPidRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorquePidRaw(int j, Pid *pid)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->getPidRaw(VOCAB_PIDTYPE_TORQUE,j,pid);} else {yError()<< "getTorquePidRaw is DEPRECATED, use getPidRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Get current pid value for a specific joint.
     * @param pids vector that will store the values of the pids.
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use getPidRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorquePidsRaw(Pid *pids)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->getPidsRaw(VOCAB_PIDTYPE_TORQUE,pids);} else {yError()<< "getTorquePidsRaw is DEPRECATED, use getPidsRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Get the torque error limit for the controller on a specific joint
     * @param j joint number
     * @param limit pointer to storage
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use getPidErrorLimitRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorqueErrorLimitRaw(int j, double *limit)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->getPidErrorLimitRaw(VOCAB_PIDTYPE_TORQUE,j,limit);} else {yError()<< "getTorqueErrorLimitRaw is DEPRECATED, use getPidErrorLimitRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Get the torque error limit for all controllers
     * @param limits pointer to the array that will store the output
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use getPidErrorLimitsRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool getTorqueErrorLimitsRaw(double *limits)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->getPidErrorLimitsRaw(VOCAB_PIDTYPE_TORQUE,limits);} else {yError()<< "getTorqueErrorLimitsRaw is DEPRECATED, use getPidErrorLimitsRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Reset the controller of a given joint, usually sets the
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use resetPidRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool resetTorquePidRaw(int j)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->resetPidRaw(VOCAB_PIDTYPE_TORQUE, j);} else {yError()<< "resetTorquePidRaw is DEPRECATED, use resetPidRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Disable the pid computation for a joint
     * @param j joint number
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use disablePidRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool disableTorquePidRaw(int j)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->disablePidRaw(VOCAB_PIDTYPE_TORQUE, j);} else {yError()<< "disableTorquePidRaw is DEPRECATED, use disablePidRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Enable the pid computation for a joint
     * @param j joint number
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use enablePidRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool enableTorquePidRaw(int j)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->enablePidRaw(VOCAB_PIDTYPE_TORQUE, j);} else {yError()<< "enableTorquePidRaw is DEPRECATED, use enablePidRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    /** Set offset value for a given pid
     * @param j joint number
     * @param v the new value
     * @return true/false on success/failure
     */
   YARP_DEPRECATED_MSG("Use setPidOffsetRaw(VOCAB_PIDTYPE_TORQUE,...) instead")
    virtual bool setTorqueOffsetRaw(int j, double v)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->setPidOffsetRaw(VOCAB_PIDTYPE_TORQUE,j,v);} else {yError()<< "setTorqueOffsetRaw is DEPRECATED, use setPidOffsetRaw(VOCAB_PIDTYPE_TORQUE,...) instead" ;return false;}}

    #endif
};

#endif // YARP_DEV_ITORQUECONTROL_H
