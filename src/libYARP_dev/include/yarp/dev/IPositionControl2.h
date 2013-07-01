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

#ifndef __YARPPOSITIONCONTROLINTERFACE_V2__
#define __YARPPOSITIONCONTROLINTERFACE_V2__

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

class yarp::dev::IPositionControl2Raw: public yarp::dev::IPositionControlRaw
{
public:
    //     IPositionControlRaw interface
    using IPositionControlRaw::getAxes;
    using IPositionControlRaw::setPositionModeRaw;
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

    /** Set position mode. This command
     * is required by control boards implementing different
     * control methods (e.g. velocity/torque), in some cases
     * it can be left empty.
     * @param joints pointer to the array of joints to change into PositionMode
     * return true/false on success failure
     */
    virtual bool setPositionModeRaw(const int n_joint, const int *joints)=0;

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
     * @param flags  pointer to the array with actual answer of the checkMotionDone
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
	using IPositionControl::setPositionMode;
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

    /** Set position mode. This command
     * is required by control boards implementing different
     * control methods (e.g. velocity/torque), in some cases
     * it can be left empty.
     * @param joints pointer to the array of joints to change into PositionMode
     * return true/false on success failure
     */
    virtual bool setPositionMode(const int n_joint, const int *joints)=0;

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
     * @param flags  pointer to the array that will store the actual value of the checkMotionDone
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
};

#endif


