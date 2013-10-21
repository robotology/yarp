// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef __YARPVELOCITYCONTROL_V2__
#define __YARPVELOCITYCONTROL_V2__

#include <yarp/dev/IVelocityControl.h>

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
    using IVelocityControl::setVelocityMode;
    using IVelocityControl::velocityMove;
    using IVelocityControl::setRefAcceleration;
    using IVelocityControl::setRefAccelerations;
    using IVelocityControl::getRefAcceleration;
    using IVelocityControl::getRefAccelerations;
    using IVelocityControl::stop;

    /**
     * Start motion at a given speed for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param sps    pointer to the array containing the new speed values
     * @return true/false upon success/failure
     */
    virtual bool velocityMove(const int n_joint, const int *joints, const double *spds)=0;

    /** Set reference acceleration for a subset of joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array containing acceleration values
     * @return true/false upon success/failure
     */
    virtual bool setRefAccelerations(const int n_joint, const int *joints, const double *accs)=0;

    /** Get reference acceleration for a subset of joints. These are the values used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array that will store the acceleration values.
     * @return true/false on success or failure
     */
    virtual bool getRefAccelerations(const int n_joint, const int *joints, double *accs)=0;

    /** Stop motion for a subset of joints
     * @param joints pointer to the array of joint numbers
     * @return true/false on success or failure
     */
    virtual bool stop(const int n_joint, const int *joints)=0;

    /** Set new velocity pid value for a joint
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    virtual bool setVelPid(int j, const Pid &pid)=0;

    /** Set new velocity pid value on multiple joints
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setVelPids(const Pid *pids)=0;

    /** Get current velocity pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return success/failure
     */
    virtual bool getVelPid(int j, Pid *pid)=0;

    /** Get current velocity pid value for a specific subset of joints.
     * @param n_joints: number of joints handled by this call
     * @param joints pointer to the array of joint numbers
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     */
    virtual bool getVelPids(Pid *pids)=0;
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
    using IVelocityControlRaw::setVelocityModeRaw;
    using IVelocityControlRaw::velocityMoveRaw;
    using IVelocityControlRaw::setRefAccelerationRaw;
    using IVelocityControlRaw::setRefAccelerationsRaw;
    using IVelocityControlRaw::getRefAccelerationRaw;
    using IVelocityControlRaw::getRefAccelerationsRaw;
    using IVelocityControlRaw::stopRaw;

    /**
     * Start motion at a given speed for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param sps    pointer to the array containing the new speed values
     * @return true/false upon success/failure
     */
    virtual bool velocityMoveRaw(const int n_joint, const int *joints, const double *spds)=0;

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

    /** Set new velocity pid value for a joint
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    virtual bool setVelPidRaw(int j, const Pid &pid)=0;

    /** Set new velocity pid value on multiple joints
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setVelPidsRaw(const Pid *pids)=0;

    /** Get current velocity pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return success/failure
     */
    virtual bool getVelPidRaw(int j, Pid *pid)=0;

    /** Get current velocity pid value for a specific subset of joints.
     * @param n_joints: number of joints handled by this call
     * @param joints pointer to the array of joint numbers
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     */
    virtual bool getVelPidsRaw(Pid *pids)=0;
};

#define VOCAB_VELOCITY_MOVE_GROUP VOCAB4('v','m','o','g')
#define VOCAB_VEL_PID  VOCAB3('v','p','d')
#define VOCAB_VEL_PIDS VOCAB4('v','p','d','s')
#endif


