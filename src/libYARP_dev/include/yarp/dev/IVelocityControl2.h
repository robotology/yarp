/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IVELOCITYCONTROL2_H
#define YARP_DEV_IVELOCITYCONTROL2_H

#include <yarp/dev/IVelocityControl.h>
#include <yarp/dev/ControlBoardPid.h>

//TO PROVIDE BACKWARD COMPATIBILITY FOR DEPRECATED METHODS: TO BE REMOVED LATER!
#include <yarp/dev/IPidControl.h>
#include <yarp/os/LogStream.h>

namespace yarp {
    namespace dev {
        class IVelocityControl2;
        class IVelocityControl2Raw;
    }
}


/**
 * @ingroup dev_iface_motor
 *
 * Interface for control boards implementing velocity control.
 */
class YARP_dev_API yarp::dev::IVelocityControl2 : public IVelocityControl
{
public:
    /**
     * Destructor.
     */
    virtual ~IVelocityControl2() {}

    // Inherit from IVelocityControl
    using IVelocityControl::getAxes;
    using IVelocityControl::velocityMove;
    using IVelocityControl::setRefAcceleration;
    using IVelocityControl::setRefAccelerations;
    using IVelocityControl::getRefAcceleration;
    using IVelocityControl::getRefAccelerations;
    using IVelocityControl::stop;

    /** Start motion at a given speed for a subset of joints.
     * @param n_joint how many joints this command is referring to
     * @param joints of joints controlled. The size of this array is n_joints
     * @param spds pointer to the array containing the new speed values, one value for each joint, the size of the array is n_joints.
     * The first value will be the new reference fot the joint joints[0].
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          spds    10 30 40
     * @return true/false on success/failure
     */
    virtual bool velocityMove(const int n_joint, const int *joints, const double *spds)=0;

    /** Get the last reference speed set by velocityMove for single joint.
     * @param j joint number
     * @param vel returns the requested reference.
     * @return true/false on success/failure
     */
    virtual bool getRefVelocity(const int joint, double *vel) {return false;};

    /** Get the last reference speed set by velocityMove for all joints.
     * @param vels pointer to the array containing the new speed values, one value for each joint
     * @return true/false on success/failure
     */
    virtual bool getRefVelocities(double *vels) {return false;};

    /** Get the last reference speed set by velocityMove for a group of joints.
     * @param n_joint how many joints this command is referring to
     * @param joints of joints controlled. The size of this array is n_joints
     * @param vels pointer to the array containing the requested values, one value for each joint.
     *  The size of the array is n_joints.
     * @return true/false on success/failure
     */
    virtual bool getRefVelocities(const int n_joint, const int *joints, double *vels) { return false;};

    /** Set reference acceleration for a subset of joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param n_joint how many joints this command is referring to
     * @param joints list of joints controlled. The size of this array is n_joints
     * @param accs   pointer to the array containing acceleration values, one value for each joint, the size of the array is n_joints.
     * The first value will be the new reference fot the joint joints[0].
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          accs    10 30 40
     * @return true/false on success/failure
     */
    virtual bool setRefAccelerations(const int n_joint, const int *joints, const double *accs)=0;

    /** Get reference acceleration for a subset of joints. These are the values used during the
     * interpolation of the trajectory.
     * @param n_joint how many joints this command is referring to
     * @param joints list of joints controlled. The size of this array is n_joints
     * @param accs   pointer to the array containing acceleration values, one value for each joint, the size of the array is n_joints.
     * The first value will be the new reference fot the joint joints[0].
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          accs    10 30 40
     * @return true/false on success/failure
     */
    virtual bool getRefAccelerations(const int n_joint, const int *joints, double *accs)=0;

    /** Stop motion for a subset of joints
     * @param n_joint how many joints this command is referring to
     * @param joints joints pointer to the array of joint numbers
     * @return true/false on success or failure
     */
    virtual bool stop(const int n_joint, const int *joints)=0;

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.68
    /** Set new velocity pid value for a joint
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use setPid(VOCAB_PIDTYPE_VELOCITY,...) instead")
    virtual bool setVelPid(int j, const yarp::dev::Pid &pid)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->setPid(VOCAB_PIDTYPE_VELOCITY,j,pid);} else {yError()<< "setVelPid is DEPRECATED, use setPid(VOCAB_PIDTYPE_VELOCITY,...) instead" ;return false;}}

    /** Set new velocity pid value on multiple joints
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    YARP_DEPRECATED_MSG("Use setPids(VOCAB_PIDTYPE_VELOCITY,...) instead")
    virtual bool setVelPids(const yarp::dev::Pid *pids)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->setPids(VOCAB_PIDTYPE_VELOCITY,pids);} else {yError()<< "setVelPids is DEPRECATED, use setPids(VOCAB_PIDTYPE_VELOCITY,...) instead" ;return false;}}


    /** Get current velocity pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return success/failure
     */
    YARP_DEPRECATED_MSG("Use getPid(VOCAB_PIDTYPE_VELOCITY,...) instead")
    virtual bool getVelPid(int j, yarp::dev::Pid *pid)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->getPid(VOCAB_PIDTYPE_VELOCITY,j,pid);} else {yError()<< "getVelPid is DEPRECATED, use getPid(VOCAB_PIDTYPE_VELOCITY,...) instead" ;return false;}}


    /** Get current velocity pid value for a specific subset of joints.
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     */
    YARP_DEPRECATED_MSG("Use getPids(VOCAB_PIDTYPE_VELOCITY,...) instead")
    virtual bool getVelPids(yarp::dev::Pid *pids)
    { IPidControl* p; p=dynamic_cast<IPidControl*>(this); if (p) {return p->getPids(VOCAB_PIDTYPE_VELOCITY,pids);} else {yError()<< "getVelPids is DEPRECATED, use getPids(VOCAB_PIDTYPE_VELOCITY,...) instead" ;return false;}}

#endif
};



/**
 * @ingroup dev_iface_motor
 *
 * Interface for control boards implementig velocity control in encoder coordinates.
 */
class yarp::dev::IVelocityControl2Raw : public yarp::dev::IVelocityControlRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IVelocityControl2Raw() {}

    // Inherit from IVelocityControl
    using IVelocityControlRaw::getAxes;
    using IVelocityControlRaw::velocityMoveRaw;
    using IVelocityControlRaw::setRefAccelerationRaw;
    using IVelocityControlRaw::setRefAccelerationsRaw;
    using IVelocityControlRaw::getRefAccelerationRaw;
    using IVelocityControlRaw::getRefAccelerationsRaw;
    using IVelocityControlRaw::stopRaw;

    /**
     * Start motion at a given speed for a subset of joints.
     * @param n_joint how many joints this command is referring to
     * @param joints pointer to the array of joint numbers
     * @param spds    pointer to the array containing the new speed values
     * @return true/false upon success/failure
     */
    virtual bool velocityMoveRaw(const int n_joint, const int *joints, const double *spds)=0;

     /** Get the last reference speed set by velocityMove for single joint.
     * @param j joint number
     * @param vel returns the requested reference.
     * @return true/false on success/failure
     */
    virtual bool getRefVelocityRaw(const int joint, double *vel) {return false;};

    /** Get the last reference speed set by velocityMove for all joints.
     * @param vels pointer to the array containing the new speed values, one value for each joint
     * @return true/false on success/failure
     */
    virtual bool getRefVelocitiesRaw(double *vels) {return false;};

    /** Get the last reference speed set by velocityMove for a group of joints.
     * @param n_joint how many joints this command is referring to
     * @param joints of joints controlled. The size of this array is n_joints
     * @param vels pointer to the array containing the requested values, one value for each joint.
     *  The size of the array is n_joints.
     * @return true/false on success/failure
     */
    virtual bool getRefVelocitiesRaw(const int n_joint, const int *joints, double *vels) { return false;};

    /** Set reference acceleration for a subset of joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array containing acceleration values
     * @return true/false upon success/failure
     */
    virtual bool setRefAccelerationsRaw(const int n_joint, const int *joints, const double *accs)=0;

    /** Get reference acceleration for a subset of joints. These are the values used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array that will store the acceleration values.
     * @return true/false on success or failure
     */
    virtual bool getRefAccelerationsRaw(const int n_joint, const int *joints, double *accs)=0;

    /** Stop motion for a subset of joints
     * @param joints pointer to the array of joint numbers
     * @return true/false on success or failure
     */
    virtual bool stopRaw(const int n_joint, const int *joints)=0;

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.68
    /** Set new velocity pid value for a joint
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    YARP_DEPRECATED_MSG("Use setPidRaw(VOCAB_PIDTYPE_VELOCITY,...) instead")
    virtual bool setVelPidRaw(int j, const yarp::dev::Pid &pid)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->setPidRaw(VOCAB_PIDTYPE_VELOCITY,j,pid);} else {yError()<< "setVelPidRaw is DEPRECATED, use setPidRaw(VOCAB_PIDTYPE_VELOCITY,...) instead" ;return false;}}

    /** Set new velocity pid value on multiple joints
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    YARP_DEPRECATED_MSG("Use setPidsRaw(VOCAB_PIDTYPE_VELOCITY,...) instead")
    virtual bool setVelPidsRaw(const yarp::dev::Pid *pids)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->setPidsRaw(VOCAB_PIDTYPE_VELOCITY,pids);} else {yError()<< "setVelPidsRaw is DEPRECATED, use setPidsRaw(VOCAB_PIDTYPE_VELOCITY,...) instead" ;return false;}}

    /** Get current velocity pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return success/failure
     */
    YARP_DEPRECATED_MSG("Use getPidRaw(VOCAB_PIDTYPE_VELOCITY,...) instead")
    virtual bool getVelPidRaw(int j, yarp::dev::Pid *pid)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->getPidRaw(VOCAB_PIDTYPE_VELOCITY,j,pid);} else {yError()<< "getVelPidRaw is DEPRECATED, use getPidRaw(VOCAB_PIDTYPE_VELOCITY,...) instead" ;return false;}}

    /** Get current velocity pid value for a specific subset of joints.
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     */
    YARP_DEPRECATED_MSG("Use getPidsRaw(VOCAB_PIDTYPE_VELOCITY,...) instead")
    virtual bool getVelPidsRaw(yarp::dev::Pid *pids)
    { IPidControlRaw* p; p=dynamic_cast<IPidControlRaw*>(this); if (p) {return p->getPidsRaw(VOCAB_PIDTYPE_VELOCITY,pids);} else {yError()<< "getVelPidsRaw is DEPRECATED, use getPidsRaw(VOCAB_PIDTYPE_VELOCITY,...) instead" ;return false;}}

#endif
};

#define VOCAB_VELOCITY_MOVE_GROUP VOCAB4('v','m','o','g')
#define VOCAB_VEL_PID  VOCAB3('v','p','d')
#define VOCAB_VEL_PIDS VOCAB4('v','p','d','s')


#endif // YARP_DEV_IVELOCITYCONTROL2_H
