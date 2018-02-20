/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_ICONTROLMODE2_H
#define YARP_DEV_ICONTROLMODE2_H

#include <yarp/dev/IControlMode.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
    class IControlMode2Raw;
    class IControlMode2;
    }
}

        /**
 * @ingroup dev_iface_motor
 *
 * Interface for setting control mode in control board.
 */
class YARP_dev_API yarp::dev::IControlMode2 : public yarp::dev::IControlMode
{
public:
    virtual ~IControlMode2(){}

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
    using IControlMode::setPositionMode;
    using IControlMode::setVelocityMode;
    using IControlMode::setTorqueMode;
    using IControlMode::setImpedancePositionMode;
    using IControlMode::setImpedanceVelocityMode;
#endif // YARP_NO_DEPRECATED
    using IControlMode::getControlMode;
    using IControlMode::getControlModes;

    /**
    * Get the current control mode for a subset of axes.
    * @param n_joints how many joints this command is referring to
    * @param joints list of joint numbers, the size of this array is n_joints
    * @param modes array containing the new controlmodes, one value for each joint, the size is n_joints.
    *          The first value will be the new reference fot the joint joints[0].
    *          for example:
    *          n_joint  3
    *          joints   0  2  4
    *          modes    VOCAB_CM_POSITION VOCAB_CM_VELOCITY VOCAB_CM_POSITION
    * @return true/false success failure.
    */
    virtual bool getControlModes(const int n_joint, const int *joints, int *modes)=0;

    /**
    * Set the current control mode.
    * @param j: joint number
    * @param mode: a vocab of the desired control mode for joint j.
    * @return true if the new controlMode was successfully set, false if the message was not received or
    *         the joint was unable to switch to the desired controlMode
    *         (e.g. the joint is on a fault condition or the desired mode is not implemented).    */
    virtual bool setControlMode(const int j, const int mode)=0;

    /**
    * Set the current control mode for a subset of axes.
    * @param n_joints how many joints this command is referring to
    * @param joints list of joint numbers, the size of this array is n_joints
    * @param modes array containing the new controlmodes, one value for each joint, the size is n_joints.
    *          The first value will be the new reference fot the joint joints[0].
    *          for example:
    *          n_joint  3
    *          joints   0  2  4
    *          modes    VOCAB_CM_POSITION VOCAB_CM_VELOCITY VOCAB_CM_POSITION
    * @return true if the new controlMode was successfully set, false if the message was not received or
    *         the joint was unable to switch to the desired controlMode
    *         (e.g. the joint is on a fault condition or the desired mode is not implemented).
    */
    virtual bool setControlModes(const int n_joint, const int *joints, int *modes)=0;

    /**
    * Set the current control mode (multiple joints).
    * @param modes: a vector containing vocabs for the desired control modes of the joints.
    * @return true if the new controlMode was successfully set, false if the message was not received or
    *         the joint was unable to switch to the desired controlMode
    *         (e.g. the joint is on a fault condition or the desired mode is not implemented).
    */
    virtual bool setControlModes(int *modes)=0;
};


/**
 * @ingroup dev_iface_motor
 *
 * Interface for setting control mode in control board. See IControlMode for
 * more documentation.
 */
class yarp::dev::IControlMode2Raw: public IControlModeRaw
{
public:
    virtual ~IControlMode2Raw(){}

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
    using IControlModeRaw::setPositionModeRaw;
    using IControlModeRaw::setVelocityModeRaw;
    using IControlModeRaw::setTorqueModeRaw;
    using IControlModeRaw::setImpedancePositionModeRaw;
    using IControlModeRaw::setImpedanceVelocityModeRaw;
#endif // YARP_NO_DEPRECATED
    using IControlModeRaw::getControlModeRaw;
    using IControlModeRaw::getControlModesRaw;

    virtual bool getControlModesRaw(const int n_joint, const int *joints, int *modes)=0;
    virtual bool setControlModeRaw(const int j, const int mode)=0;
    virtual bool setControlModesRaw(const int n_joint, const int *joints, int *modes)=0;
    virtual bool setControlModesRaw(int *modes)=0;

};



// Values
// Read / Write
#define VOCAB_CM_MIXED              VOCAB3('m','i','x')

// Write only (only from high level toward the joint)
#define VOCAB_CM_FORCE_IDLE         VOCAB4('f','i','d','l')

// Read only (imposed by the board on special events)
#define VOCAB_CM_HW_FAULT           VOCAB4('h','w','f','a')
#define VOCAB_CM_CALIBRATING        VOCAB3('c','a','l')     // the joint is calibrating
#define VOCAB_CM_CALIB_DONE         VOCAB4('c','a','l','d') // calibration successfully completed
#define VOCAB_CM_NOT_CONFIGURED     VOCAB4('c','f','g','n') // missing initial configuration (default value at start-up)
#define VOCAB_CM_CONFIGURED         VOCAB4('c','f','g','y') // initial configuration completed, if any


/*  old style vocabs
#define VOCAB_CM_CONTROL_MODE   VOCAB4('c','m','o','d')
#define VOCAB_CM_CONTROL_MODES  VOCAB4('c','m','d','s')

#define VOCAB_CM_IDLE           VOCAB3('c','m','i')
#define VOCAB_CM_TORQUE         VOCAB3('c','m','t')
#define VOCAB_CM_POSITION       VOCAB4('c','m','p','t')
#define VOCAB_CM_VELOCITY       VOCAB4('c','m','v','l')
#define VOCAB_CM_MIXED          VOCAB4('c','m','m','x')
#define VOCAB_CM_FORCE_IDLE     VOCAB4('c','m','f','i')


#define VOCAB_CM_UNKNOWN        VOCAB4('c','m','u','k')
#define VOCAB_CM_IMPEDANCE_POS  VOCAB4('c','m','i','p')
#define VOCAB_CM_IMPEDANCE_VEL  VOCAB4('c','m','i','v')

#define VOCAB_CM_HW_FAULT       VOCAB4('c','m','f','a')
#define VOCAB_CM_CALIBRATING    VOCAB4('c','m','c','a')
#define VOCAB_CM_CALIB_DONE     VOCAB4('c','m','c','d')
#define VOCAB_CM_NOT_CONFIGURED VOCAB4('c','m','n','c')
#define VOCAB_CM_CONFIGURED     VOCAB4('c','m','c','f')
*/

#endif // YARP_DEV_ICONTROLMODE2_H
