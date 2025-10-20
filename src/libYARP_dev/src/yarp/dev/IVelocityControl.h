/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IVELOCITYCONTROL_H
#define YARP_DEV_IVELOCITYCONTROL_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {
class IVelocityControl;
class IVelocityControlRaw;
}

/**
 * @ingroup dev_iface_motor_raw
 *
 * Interface for control boards implementing velocity control in encoder coordinates.
 */
class YARP_dev_API yarp::dev::IVelocityControlRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IVelocityControlRaw() {}

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @param axis pointer to storage, return value
     * @return true/false.
     */
    virtual yarp::dev::ReturnValue getAxes(int *axis) = 0;

    /**
     * Start motion at a given speed, single joint.
     * @param j joint number
     * @param sp speed value
     * @return bool/false upon success/failure
     */
    virtual yarp::dev::ReturnValue velocityMoveRaw(int j, double sp)=0;

    /**
     * Start motion at a given speed, multiple joints.
     * @param sp pointer to the array containing the new speed values
     * @return true/false upon success/failure
     */
    virtual yarp::dev::ReturnValue velocityMoveRaw(const double *sp)=0;

    /** Set reference acceleration for a joint. This value is used during the
     * trajectory generation.
     * @param j joint number
     * @param acc acceleration value
     * @return true/false upon success/failure
     */
    virtual yarp::dev::ReturnValue setTrajAccelerationRaw(int j, double acc)=0;

    /** Set reference acceleration on all joints. This is the value that is
     * used during the generation of the trajectory.
     * @param accs pointer to the array of acceleration values
     * @return true/false upon success/failure
     */
    virtual yarp::dev::ReturnValue setTrajAccelerationsRaw(const double *accs)=0;

    /** Get reference acceleration for a joint. Returns the acceleration used to
     * generate the trajectory profile.
     * @param j joint number
     * @param acc pointer to storage for the return value
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTrajAccelerationRaw(int j, double *acc)=0;

    /** Get reference acceleration of all joints. These are the values used during the
     * interpolation of the trajectory.
     * @param accs pointer to the array that will store the acceleration values.
     * @return true/false on success or failure
     */
    virtual yarp::dev::ReturnValue getTrajAccelerationsRaw(double *accs)=0;

    /** Stop motion, single joint
     * @param j joint number
     * @return true/false on success or failure
     */
    virtual yarp::dev::ReturnValue stopRaw(int j)=0;

    /** Stop motion, multiple joints
     * @return true/false on success or failure
     */
    virtual yarp::dev::ReturnValue stopRaw()=0;

    /**
     * Start motion at a given speed for a subset of joints.
     * @param n_joint how many joints this command is referring to
     * @param joints pointer to the array of joint numbers
     * @param spds    pointer to the array containing the new speed values
     * @return true/false upon success/failure
     */
    virtual yarp::dev::ReturnValue velocityMoveRaw(const int n_joint, const int *joints, const double *spds)=0;

     /** Get the last reference speed set by velocityMove for single joint.
     * @param j joint number
     * @param vel returns the requested reference.
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTargetVelocityRaw(const int joint, double *vel)=0;

    /** Get the last reference speed set by velocityMove for all joints.
     * @param vels pointer to the array containing the new speed values, one value for each joint
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTargetVelocitiesRaw(double *vels)=0;

    /** Get the last reference speed set by velocityMove for a group of joints.
     * @param n_joint how many joints this command is referring to
     * @param joints of joints controlled. The size of this array is n_joints
     * @param vels pointer to the array containing the requested values, one value for each joint.
     *  The size of the array is n_joints.
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTargetVelocitiesRaw(const int n_joint, const int *joints, double *vels)=0;

    /** Set reference acceleration for a subset of joints. This is the value that is
     * used during the generation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array containing acceleration values
     * @return true/false upon success/failure
     */
    virtual yarp::dev::ReturnValue setTrajAccelerationsRaw(const int n_joint, const int *joints, const double *accs)=0;

    /** Get reference acceleration for a subset of joints. These are the values used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array that will store the acceleration values.
     * @return true/false on success or failure
     */
    virtual yarp::dev::ReturnValue getTrajAccelerationsRaw(const int n_joint, const int *joints, double *accs)=0;

    /** Stop motion for a subset of joints
     * @param joints pointer to the array of joint numbers
     * @return true/false on success or failure
     */
    virtual yarp::dev::ReturnValue stopRaw(const int n_joint, const int *joints)=0;
};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control boards implementing velocity control.
 */
class YARP_dev_API yarp::dev::IVelocityControl
{
public:
    /**
     * Destructor.
     */
    virtual ~IVelocityControl() {}

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * param axes pointer to storage
     * @return true/false.
     */
    virtual yarp::dev::ReturnValue getAxes(int *axes) = 0;

    /**
     * Start motion at a given speed, single joint.
     * @param j joint number
     * @param sp speed value
     * @return bool/false upon success/failure
     */
    virtual yarp::dev::ReturnValue velocityMove(int j, double sp)=0;

    /**
     * Start motion at a given speed, multiple joints.
     * @param sp pointer to the array containing the new speed values
     * @return true/false upon success/failure
     */
    virtual yarp::dev::ReturnValue velocityMove(const double *sp)=0;

    /** Set reference acceleration for a joint. This value is used during the
     * trajectory generation.
     * @param j joint number
     * @param acc acceleration value
     * @return true/false upon success/failure
     */
    virtual yarp::dev::ReturnValue setTrajAcceleration(int j, double acc)=0;

    /** Set reference acceleration on all joints. This is the value that is
     * used during the generation of the trajectory.
     * @param accs pointer to the array of acceleration values
     * @return true/false upon success/failure
     */
    virtual yarp::dev::ReturnValue setTrajAccelerations(const double *accs)=0;

    /** Get reference acceleration for a joint. Returns the acceleration used to
     * generate the trajectory profile.
     * @param j joint number
     * @param acc pointer to storage for the return value
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTrajAcceleration(int j, double *acc)=0;

    /** Get reference acceleration of all joints. These are the values used during the
     * interpolation of the trajectory.
     * @param accs pointer to the array that will store the acceleration values.
     * @return true/false on success or failure
     */
    virtual yarp::dev::ReturnValue getTrajAccelerations(double *accs)=0;

    /** Stop motion, single joint
     * @param j joint number
     * @return true/false on success or failure
     */
    virtual yarp::dev::ReturnValue stop(int j)=0;

    /** Stop motion, multiple joints
     * @return true/false on success or failure
     */
    virtual yarp::dev::ReturnValue stop()=0;

    /** Start motion at a given speed for a subset of joints.
     * @param n_joint how many joints this command is referring to
     * @param joints of joints controlled. The size of this array is n_joints
     * @param spds pointer to the array containing the new speed values, one value for each joint, the size of the array is n_joints.
     * The first value will be the new reference for the joint joints[0].
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          spds    10 30 40
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue velocityMove(const int n_joint, const int *joints, const double *spds)=0;

    /** Get the last reference speed set by velocityMove for single joint.
     * @param j joint number
     * @param vel returns the requested reference.
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTargetVelocity(const int joint, double *vel)=0;

    /** Get the last reference speed set by velocityMove for all joints.
     * @param vels pointer to the array containing the new speed values, one value for each joint
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTargetVelocities(double *vels)=0;

    /** Get the last reference speed set by velocityMove for a group of joints.
     * @param n_joint how many joints this command is referring to
     * @param joints of joints controlled. The size of this array is n_joints
     * @param vels pointer to the array containing the requested values, one value for each joint.
     *  The size of the array is n_joints.
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTargetVelocities(const int n_joint, const int *joints, double *vels)=0;

    /** Set reference acceleration for a subset of joints. This is the value that is
     * used during the generation of the trajectory.
     * @param n_joint how many joints this command is referring to
     * @param joints list of joints controlled. The size of this array is n_joints
     * @param accs   pointer to the array containing acceleration values, one value for each joint, the size of the array is n_joints.
     * The first value will be the new reference for the joint joints[0].
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          accs    10 30 40
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue setTrajAccelerations(const int n_joint, const int *joints, const double *accs)=0;

    /** Get reference acceleration for a subset of joints. These are the values used during the
     * interpolation of the trajectory.
     * @param n_joint how many joints this command is referring to
     * @param joints list of joints controlled. The size of this array is n_joints
     * @param accs   pointer to the array containing acceleration values, one value for each joint, the size of the array is n_joints.
     * The first value will be the new reference for the joint joints[0].
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          accs    10 30 40
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTrajAccelerations(const int n_joint, const int *joints, double *accs)=0;

    /** Stop motion for a subset of joints
     * @param n_joint how many joints this command is referring to
     * @param joints joints pointer to the array of joint numbers
     * @return true/false on success or failure
     */
    virtual yarp::dev::ReturnValue stop(const int n_joint, const int *joints)=0;
};

// Interface name
constexpr yarp::conf::vocab32_t VOCAB_VELOCITYCONTROL_INTERFACE   = yarp::os::createVocab32('i','v','e','l');

constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_MOVE  = yarp::os::createVocab32('v','m','o');
constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_MOVES = yarp::os::createVocab32('v','m','o','s');
constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_MOVE_GROUP = yarp::os::createVocab32('v','m','o','g') ;
constexpr yarp::conf::vocab32_t VOCAB_VEL_PID  = yarp::os::createVocab32('v','p','d');
constexpr yarp::conf::vocab32_t VOCAB_VEL_PIDS = yarp::os::createVocab32('v','p','d','s');

#endif // YARP_DEV_IVELOCITYCONTROL_H
