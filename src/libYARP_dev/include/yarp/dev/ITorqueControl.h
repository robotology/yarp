// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Robotics Brain and Cognitive Sciences Department, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARPTORQUECONTROL__
#define __YARPTORQUECONTROL__

#include <yarp/os/Vocab.h>
#include <yarp/dev/ControlBoardPid.h>

namespace yarp {
    namespace dev {
        class ITorqueControlRaw;
        class ITorqueControl;
      }
}

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

    /**
     * Set torque control mode. This command
     * is required by control boards implementing different
     * control methods (e.g. velocity/torque), in some cases
     * it can be left empty.
     * @return true/false on success/failure
     */
    virtual bool setTorqueMode()=0;

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

    /** Set the back-efm compensation gain for a given joint.
     * @param j joint number
     * @param bemf the returned bemf gain of joint j
     * @return true/false on success/failure
     */
    virtual bool getBemfParam(int j, double *bemf)=0;

    /** Set the back-efm compensation gain for a given joint.
     * @param j joint number
     * @param bemf new value
     * @return true/false on success/failure
     */
    virtual bool setBemfParam(int j, double bemf)=0;

     /** Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    virtual bool setTorquePid(int j, const Pid &pid)=0;

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

    /** Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setTorquePids(const Pid *pids)=0;

    /** Set the torque error limit for the controller on a specific joint
     * @param j joint number
     * @param limit limit value
     * @return true/false on success/failure
     */
    virtual bool setTorqueErrorLimit(int j, double limit)=0;

    /** Get the torque error limit for the controller on all joints.
     * @param limits pointer to the vector with the new limits
     * @return true/false on success/failure
     */
    virtual bool setTorqueErrorLimits(const double *limits)=0;

    /** Get the current torque error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getTorqueError(int j, double *err)=0;

    /** Get the torque error of all joints.
     * @param errs pointer to the vector that will store the errors
     * @return true/false on success/failure
     */
    virtual bool getTorqueErrors(double *errs)=0;

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return true/false on success/failure
     */
    virtual bool getTorquePidOutput(int j, double *out)=0;

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pointer to the vector that will store the output values
     * @return true/false on success/failure
     */
    virtual bool getTorquePidOutputs(double *outs)=0;

    /** Get current pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return true/false on success/failure
     */
    virtual bool getTorquePid(int j, Pid *pid)=0;

    /** Get current pid value for a specific joint.
     * @param pids vector that will store the values of the pids.
     * @return true/false on success/failure
     */
    virtual bool getTorquePids(Pid *pids)=0;

    /** Get the torque error limit for the controller on a specific joint
     * @param j joint number
     * @param limit pointer to the result value
     * @return true/false on success/failure
     */
    virtual bool getTorqueErrorLimit(int j, double *limit)=0;

    /** Get the torque error limit for all controllers
     * @param limits pointer to the array that will store the output
     * @return true/false on success/failure
     */
    virtual bool getTorqueErrorLimits(double *limits)=0;

    /** Reset the controller of a given joint, usually sets the 
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool resetTorquePid(int j)=0;

    /** Disable the pid computation for a joint
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool disableTorquePid(int j)=0;

    /** Enable the pid computation for a joint
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool enableTorquePid(int j)=0;

	/** Set offset value for a given pid
     * @param j joint number
     * @param v the new value
     * @return true/false on success/failure
     */
    virtual bool setTorqueOffset(int j, double v)=0;
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

    /**
     * Set torque control mode. This command
     * is required by control boards implementing different
     * control methods (e.g. velocity/torque), in some cases
     * it can be left empty.
     * @return true/false on success failure
     */
    virtual bool setTorqueModeRaw()=0;

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

     /** Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    virtual bool setTorquePidRaw(int j, const Pid &pid)=0;

    /** Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setTorquePidsRaw(const Pid *pids)=0;

    /** Set the torque error limit for the controller on a specific joint
     * @param j joint number
     * @param limit limit value
     * @return true/false on success/failure
     */
    virtual bool setTorqueErrorLimitRaw(int j, double limit)=0;

    /** Get the torque error limit for the controller on all joints.
     * @param limits pointer to the vector with the new limits
     * @return true/false on success/failure
     */
    virtual bool setTorqueErrorLimitsRaw(const double *limits)=0;

    /** Get the current torque error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getTorqueErrorRaw(int j, double *err)=0;

    /** Get the torque error of all joints.
     * @param errs pointer to the vector that will store the errors
     * @return true/false on success/failure
     */
    virtual bool getTorqueErrorsRaw(double *errs)=0;

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return true/false on success/failure
     */
    virtual bool getTorquePidOutputRaw(int j, double *out)=0;

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     * @return true/false on success/failure
     */
    virtual bool getTorquePidOutputsRaw(double *outs)=0;

    /** Get current pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return true/false on success/failure
     */
    virtual bool getTorquePidRaw(int j, Pid *pid)=0;

    /** Get current pid value for a specific joint.
     * @param pids vector that will store the values of the pids.
     * @return true/false on success/failure
     */
    virtual bool getTorquePidsRaw(Pid *pids)=0;

    /** Get the torque error limit for the controller on a specific joint
     * @param j joint number
     * @param limit pointer to storage
     * @return true/false on success/failure
     */
    virtual bool getTorqueErrorLimitRaw(int j, double *limit)=0;

    /** Get the torque error limit for all controllers
     * @param limits pointer to the array that will store the output
     * @return true/false on success/failure
     */
    virtual bool getTorqueErrorLimitsRaw(double *limits)=0;

    /** Reset the controller of a given joint, usually sets the 
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool resetTorquePidRaw(int j)=0;

    /** Disable the pid computation for a joint
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool disableTorquePidRaw(int j)=0;

    /** Enable the pid computation for a joint
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool enableTorquePidRaw(int j)=0;

	/** Set offset value for a given pid
     * @param j joint number
     * @param v the new value
     * @return true/false on success/failure
     */
    virtual bool setTorqueOffsetRaw(int j, double v)=0;
};

#endif

