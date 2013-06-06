// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
* Author:  Alberto Cardellino
* email:   alberto.cardellino@iit.it
* website: www.robotcub.org
* Permission is granted to copy, distribute, and/or modify this program
* under the terms of the GNU General Public License, version 2 or any
* later version published by the Free Software Foundation.
*
* A copy of the license can be found at
* http://www.robotcub.org/icub/license/gpl.txt
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details
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
class yarp::dev::IVelocityControl2 : public IVelocityControl
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
     * Set Velocity mode. This command
     * is required by control boards implementing different
     * control methods (e.g. velocity/torque), in some cases
     * it can be left empty.
     * @param joints pointer to the array of joint numbers to change into VelocityMode
     * @return true/false on success failure
     */
    virtual bool setVelocityMode(const int n_joint, const int *joints)=0;

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

    /** Set new velocity pid value on a subset of joints
     * @param n_joints: number of joints handled by this call
     * @param joints pointer to the array of joint numbers
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setVelPids(const int n_joint, const int *joints, const Pid *pids)=0;

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
    virtual bool getVelPids(const int n_joint, const int *joints, Pid *pids)=0;

    /** Get current velocity pid value for a specific subset of joints.
     * @param n_joints: number of joints handled by this call
     * @param joints pointer to the array of joint numbers
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     */
    virtual bool getVelPids(Pid *pids)=0;

    /** Get the current error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getVelError(int j, double *err)=0;

    /** Get the current error for a subset of joints.
     * @param n_joints: number of joints handled by this call
     * @param joints pointer to the array of joint numbers
     * @param errs pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getVelErrors(const int n_joint, const int *joints, double *errs)=0;

    /** Get the error of all joints.
     * @param errs pointer to the vector that will store the errors
     */
    virtual bool getVelErrors(double *errs)=0;
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
     * Set Velocity mode. This command
     * is required by control boards implementing different
     * control methods (e.g. velocity/torque), in some cases
     * it can be left empty.
     * @param joints pointer to the array of joints change into VelocityMode
     * @return true/false on success failure
     */
    virtual bool setVelocityModeRaw(const int n_joint, const int *joints)=0;

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

    /** Set new velocity pid value on a subset of joints
     * @param joints pointer to the array of joint numbers
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setVelPidsRaw(const int n_joint, const int *joints, const Pid *pids)=0;

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
    virtual bool getVelPidsRaw(const int n_joint, const int *joints, Pid *pids)=0;

    /** Get current velocity pid value for a specific subset of joints.
     * @param n_joints: number of joints handled by this call
     * @param joints pointer to the array of joint numbers
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     */
    virtual bool getVelPidsRaw(Pid *pids)=0;

    /** Get the current error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getVelErrorRaw(const int j, double *err)=0;

    /** Get the current error for a subset of joints.
     * @param n_joints: number of joints handled by this call
     * @param joints pointer to the array of joint numbers
     * @param errs pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getVelErrorsRaw(const int n_joint, const int *joints, double *errs)=0;

    /** Get the error of all joints.
     * @param errs pointer to the vector that will store the errors
     */
    virtual bool getVelErrorsRaw(double *errs)=0;
};

#endif


