/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IPOSITIONCONTROL2_H
#define YARP_DEV_IPOSITIONCONTROL2_H

#include <yarp/dev/IPositionControl.h>

namespace yarp {
    namespace dev {
        class IPositionControl2;
        class IPositionControl2Raw;
    }
}


/**
 * @ingroup dev_iface_motor
 *
 * Interface for a generic control board device implementing position control in encoder
 * coordinates.
 * This interface expand the previous IPositionControlRaw adding a new method for
 * sending commands to a eterogeneous group of axis.
 */

class yarp::dev::IPositionControl2Raw: public IPositionControlRaw
{
public:
    //     IPositionControlRaw interface
    using IPositionControlRaw::getAxes;
    using IPositionControlRaw::positionMoveRaw;
    using IPositionControlRaw::relativeMoveRaw;
    using IPositionControlRaw::checkMotionDoneRaw;
    using IPositionControlRaw::setRefSpeedRaw;
    using IPositionControlRaw::setRefSpeedsRaw;
    using IPositionControlRaw::setRefAccelerationRaw;
    using IPositionControlRaw::setRefAccelerationsRaw;
    using IPositionControlRaw::getRefSpeedRaw;
    using IPositionControlRaw::getRefSpeedsRaw;
    using IPositionControlRaw::getRefAccelerationRaw;
    using IPositionControlRaw::getRefAccelerationsRaw;
    using IPositionControlRaw::stopRaw;


    /**
     * Destructor.
     */
    virtual ~IPositionControl2Raw() {}

    /** Set new reference point for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param refs   pointer to the array specifies the new reference points
     * @return true/false on success/failure
     */
    virtual bool positionMoveRaw(const int n_joint, const int *joints, const double *refs)=0;

    /** Set relative position for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param deltas pointer to the array of relative commands
     * @return true/false on success/failure
     */
    virtual bool relativeMoveRaw(const int n_joint, const int *joints, const double *deltas)=0;

    /** Check if the current trajectory is terminated. Non blocking.
     * @param joints pointer to the array of joint numbers
     * @param flag true if the trajectory is terminated, false otherwise
     *        (a single value which is the 'and' of all joints')
     * @return true/false if network communication went well.
     */
    virtual bool checkMotionDoneRaw(const int n_joint, const int *joints, bool *flags)=0;

    /** Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param spds   pointer to the array with speed values.
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeedsRaw(const int n_joint, const int *joints, const double *spds)=0;

    /** Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array with acceleration values
     * @return true/false upon success/failure
     */
    virtual bool setRefAccelerationsRaw(const int n_joint, const int *joints, const double *accs)=0;

    /** Get reference speed of all joints. These are the  values used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param spds   pointer to the array that will store the speed values.
     * @return true/false upon success/failure
     */
    virtual bool getRefSpeedsRaw(const int n_joint, const int *joints, double *spds)=0;

    /** Get reference acceleration for a joint. Returns the acceleration used to
     * generate the trajectory profile.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array that will store the acceleration values
     * @return true/false on success/failure
     */
    virtual bool getRefAccelerationsRaw(const int n_joint, const int *joints, double *accs)=0;

    /** Stop motion for subset of joints
     * @param joints pointer to the array of joint numbers
     * @return true/false on success/failure
     */
    virtual bool stopRaw(const int n_joint, const int *joints)=0;

    /** Get the last position reference for the specified axis.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    virtual bool getTargetPositionRaw(const int joint, double *ref) { return false;};

    /** Get the last position reference for all axes.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    virtual bool getTargetPositionsRaw(double *refs) { return false;};

    /** Get the last position reference for the specified group of axes.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    virtual bool getTargetPositionsRaw(const int n_joint, const int *joints, double *refs) { return false;};
};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for a generic control board device implementing position control.
 */
class YARP_dev_API yarp::dev::IPositionControl2 : public IPositionControl
{
public:

    //     IPositionControl interface
    using IPositionControl::getAxes;
    using IPositionControl::positionMove;
    using IPositionControl::relativeMove;
    using IPositionControl::checkMotionDone;
    using IPositionControl::setRefSpeed;
    using IPositionControl::setRefSpeeds;
    using IPositionControl::setRefAcceleration;
    using IPositionControl::setRefAccelerations;
    using IPositionControl::getRefSpeed;
    using IPositionControl::getRefSpeeds;
    using IPositionControl::getRefAcceleration;
    using IPositionControl::getRefAccelerations;
    using IPositionControl::stop;

    /**
     * Destructor.
     */
    virtual ~IPositionControl2() {}

    /** Set new reference point for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param refs   pointer to the array specifing the new reference points
     * @return true/false on success/failure
     */
    virtual bool positionMove(const int n_joint, const int *joints, const double *refs)=0;

    /** Set relative position for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param deltas pointer to the array of relative commands
     * @return true/false on success/failure
     */
    virtual bool relativeMove(const int n_joint, const int *joints, const double *deltas)=0;

    /** Check if the current trajectory is terminated. Non blocking.
     * @param joints pointer to the array of joint numbers
     * @param flags  pointer to return value (logical "and" of all set of joints)
     * @return true/false if network communication went well.
     */
    virtual bool checkMotionDone(const int n_joint, const int *joints, bool *flags)=0;

    /** Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param spds   pointer to the array with speed values.
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeeds(const int n_joint, const int *joints, const double *spds)=0;

    /** Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array with acceleration values
     * @return true/false upon success/failure
     */
    virtual bool setRefAccelerations(const int n_joint, const int *joints, const double *accs)=0;

    /** Get reference speed of all joints. These are the  values used during the
     * interpolation of the trajectory.
     * @param joints pointer to the array of joint numbers
     * @param spds   pointer to the array that will store the speed values.
     * @return true/false upon success/failure
     */
    virtual bool getRefSpeeds(const int n_joint, const int *joints, double *spds)=0;

    /** Get reference acceleration for a joint. Returns the acceleration used to
     * generate the trajectory profile.
     * @param joints pointer to the array of joint numbers
     * @param accs   pointer to the array that will store the acceleration values
     * @return true/false on success/failure
     */
    virtual bool getRefAccelerations(const int n_joint, const int *joints, double *accs)=0;

    /** Stop motion for subset of joints
     * @param joints pointer to the array of joint numbers
     * @return true/false on success/failure
     */
    virtual bool stop(const int n_joint, const int *joints)=0;

        /** Get the last position reference for the specified axis.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    virtual bool getTargetPosition(const int joint, double *ref) { return false;};

    /** Get the last position reference for all axes.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    virtual bool getTargetPositions(double *refs) { return false;};

    /** Get the last position reference for the specified group of axes.
     *  This is the dual of PositionMove and shall return only values sent using
     *  IPositionControl interface.
     *  If other interfaces like IPositionDirect are implemented by the device, this call
     *  must ignore their values, i.e. this call must never return a reference sent using
     *  IPositionDirect::SetPosition
     * @param ref last reference sent using PositionMove functions
     * @return true/false on success/failure
     */
    virtual bool getTargetPositions(const int n_joint, const int *joints, double *refs) { return false;};
};

#define VOCAB_POSITION_MOVE_GROUP    VOCAB4('p','o','s','g')
#define VOCAB_RELATIVE_MOVE_GROUP    VOCAB4('r','e','l','g')
#define VOCAB_MOTION_DONE_GROUP      VOCAB4('d','o','n','g')
#define VOCAB_REF_SPEED_GROUP        VOCAB4('v','e','l','g')
#define VOCAB_REF_ACCELERATION_GROUP VOCAB4('a','c','c','g')
#define VOCAB_STOP_GROUP             VOCAB4('s','t','o','g')


#endif // YARP_DEV_IPOSITIONCONTROL2_H
