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
namespace yarp {
    namespace dev {
        class IVelocityControl2;
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IVelocityControlRaw instead") IVelocityControlRaw IVelocityControl2Raw;
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
};

#endif // YARP_DEV_IVELOCITYCONTROL2_H
