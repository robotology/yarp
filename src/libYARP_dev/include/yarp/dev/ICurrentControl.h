/*
* Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
* Authors: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#ifndef YARP_DEV_ICURRENTCONTROL_H
#define YARP_DEV_ICURRENTCONTROL_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/ControlBoardPid.h>

namespace yarp {
    namespace dev {
        class ICurrentControlRaw;
        class ICurrentControl;
      }
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control boards implementing current control.
 */
class YARP_dev_API yarp::dev::ICurrentControl
{
public:
    /**
     * Destructor.
     */
    virtual ~ICurrentControl() {}

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
    virtual bool getRefCurrents(double *t) = 0;

    /** Get the reference value of the torque for a given joint.
     * This is NOT the feedback (see getTorque instead).
     * @param j joint number
     * @param t the returned reference torque of joint j
     * @return true/false on success/failure
     */
    virtual bool getRefCurrent(int j, double *t) = 0;

    /** Set the reference value of the torque for all joints.
     * @param t pointer to the array of torque values
     * @return true/false on success/failure
     */
    virtual bool setRefCurrents(const double *t) = 0;

    /** Set the reference value of the torque for a given joint.
     * @param j joint number
     * @param t new value
     * @return true/false on success/failure
     */
    virtual bool setRefCurrent(int j, double t) = 0;

    /** Set new torque reference for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param refs   pointer to the array specifing the new torque reference
     * @return true/false on success/failure
     */
    virtual bool setRefCurrents(const int n_joint, const int *joints, const double *t) { return false; }  // this function has a default implementation to keep backward compatibility with existing devices

     /** Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    virtual bool setCurrentPid(int j, const Pid &pid) = 0;

    /** Get the value of the torque on a given joint (this is the
     * feedback if you have a torque sensor).
     * @param j joint number
     * @param t pointer to the result value
     * @return true/false on success/failure
     */
    virtual bool getCurrent(int j, double *t) = 0;

    /** Get the value of the torque for all joints (this is
     * the feedback if you have torque sensors).
     * @param t pointer to the array that will store the output
     * @return true/false on success/failure
     */
    virtual bool getCurrents(double *t) = 0;

    /** Get the full scale of the torque sensor of a given joint
     * @param j joint number
     * @param min minimum torque of the joint j
     * @param max maximum torque of the joint j
     * @return true/false on success/failure
     */
    virtual bool getCurrentRange(int j, double *min, double *max) = 0;

    /** Get the full scale of the torque sensors of all joints
     * @param min pointer to the array that will store minimum torques of the joints
     * @param max pointer to the array that will store maximum torques of the joints
     * @return true/false on success/failure
     */
    virtual bool getCurrentRanges(double *min, double *max) = 0;

    /** Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setCurrentPids(const Pid *pids) = 0;

    /** Get the current torque error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getCurrentError(int j, double *err) = 0;

    /** Get the torque error of all joints.
     * @param errs pointer to the vector that will store the errors
     * @return true/false on success/failure
     */
    virtual bool getCurrentErrors(double *errs) = 0;

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return true/false on success/failure
     */
    virtual bool getCurrentPidOutput(int j, double *out) = 0;

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pointer to the vector that will store the output values
     * @return true/false on success/failure
     */
    virtual bool getCurrentPidOutputs(double *outs) = 0;

    /** Get current pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return true/false on success/failure
     */
    virtual bool getCurrentPid(int j, Pid *pid) = 0;

    /** Get current pid value for a specific joint.
     * @param pids vector that will store the values of the pids.
     * @return true/false on success/failure
     */
    virtual bool getCurrentPids(Pid *pids)=0;

    /** Reset the controller of a given joint, usually sets the
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool resetCurrentPid(int j)=0;

    /** Disable the pid computation for a joint
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool disableCurrentPid(int j)=0;

    /** Enable the pid computation for a joint
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool enableCurrentPid(int j)=0;
};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control boards implementing torque control.
 */
class yarp::dev::ICurrentControlRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~ICurrentControlRaw() {}

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
    virtual bool getCurrentRaw(int j, double *t)=0;

    /** Get the value of the torque for all joints (this is
     * the feedback if you have torque sensors).
     * @param t pointer to the array that will store the output
     * @return true/false on success/failure
     */
    virtual bool getCurrentsRaw(double *t)=0;

    /** Get the full scale of the torque sensor of a given joint
     * @param j joint number
     * @param min minimum torque of the joint j
     * @param max maximum torque of the joint j
     * @return true/false on success/failure
     */
    virtual bool getCurrentRangeRaw(int j, double *min, double *max)=0;

    /** Get the full scale of the torque sensors of all joints
     * @param min pointer to the array that will store minimum torques of the joints
     * @param max pointer to the array that will store maximum torques of the joints
     * @return true/false on success/failure
     */
    virtual bool getCurrentRangesRaw(double *min, double *max)=0;

    /** Set the reference value of the torque for all joints.
     * @param t pointer to the array of torque values
     * @return true/false on success/failure
     */
    virtual bool setRefCurrentsRaw(const double *t)=0;

    /** Set the reference value of the torque for a given joint.
     * @param j joint number
     * @param t new value
     * @return true/false on success/failure
     */
    virtual bool setRefCurrentRaw(int j, double t)=0;

    /** Set new torque reference for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param refs   pointer to the array specifing the new torque reference
     * @return true/false on success/failure
     */
    virtual bool setRefCurrentsRaw(const int n_joint, const int *joints, const double *t) {return false;}  // this function has a default implementation to keep backward compatibility with existing devices

    /** Get the reference value of the torque for all joints.
     * This is NOT the feedback (see getTorques instead).
     * @param t pointer to the array of torque values
     * @return true/false on success/failure
     */
    virtual bool getRefCurrentsRaw(double *t)=0;

    /** Set the reference value of the torque for a given joint.
     * This is NOT the feedback (see getTorque instead).
     * @param j joint number
     * @param t new value
     * @return true/false on success/failure
     */
    virtual bool getRefCurrentRaw(int j, double *t)=0;

     /** Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    virtual bool setCurrentPidRaw(int j, const Pid &pid)=0;

    /** Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setCurrentPidsRaw(const Pid *pids)=0;

    /** Get the current torque error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getCurrentErrorRaw(int j, double *err)=0;

    /** Get the torque error of all joints.
     * @param errs pointer to the vector that will store the errors
     * @return true/false on success/failure
     */
    virtual bool getCurrentErrorsRaw(double *errs)=0;

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return true/false on success/failure
     */
    virtual bool getCurrentPidOutputRaw(int j, double *out)=0;

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     * @return true/false on success/failure
     */
    virtual bool getCurrentPidOutputsRaw(double *outs)=0;

    /** Get current pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return true/false on success/failure
     */
    virtual bool getCurrentPidRaw(int j, Pid *pid)=0;

    /** Get current pid value for a specific joint.
     * @param pids vector that will store the values of the pids.
     * @return true/false on success/failure
     */
    virtual bool getCurrentPidsRaw(Pid *pids)=0;

    /** Reset the controller of a given joint, usually sets the
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool resetCurrentPidRaw(int j)=0;

    /** Disable the pid computation for a joint
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool disableCurrentPidRaw(int j)=0;

    /** Enable the pid computation for a joint
     * @param j joint number
     * @return true/false on success/failure
     */
    virtual bool enableCurrentPidRaw(int j)=0;

};

// Interface name
#define VOCAB_CURRENTCONTROL_INTERFACE VOCAB4('i','c','u','r')
// methods names
#define VOCAB_CURRENTCONTROL1    VOCAB3('r','e','f')
#define VOCAB_CURRENT_PID        VOCAB4('c','r','p','d')
#define VOCAB_CURRENT_PIDS       VOCAB4('c','p','d','s')
#define VOCAB_CURRENT_REF        VOCAB4('c','r','r','f')
#define VOCAB_CURRENT_REFS       VOCAB4('c','r','f','s')
#define VOCAB_CURRENT_REF_GROUP  VOCAB4('c','r','f','g')
#define VOCAB_CURRENT_PID_OUTPUT  VOCAB4('c','p','o','t')
#define VOCAB_CURRENT_PID_OUTPUTS  VOCAB4('c','p','o','s')
#define VOCAB_CURRENT_RANGES     VOCAB4('r','n','g','s')
#define VOCAB_CURRENT_RANGE      VOCAB3('r','n','g')


#endif // YARP_DEV_ICURRENTCONTROL_H
