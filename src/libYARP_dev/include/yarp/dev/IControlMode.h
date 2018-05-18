/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_ICONTROLMODE_H
#define YARP_DEV_ICONTROLMODE_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
    class IControlModeRaw;
    class IControlMode;
    }
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for setting control mode in control board.
 */
class YARP_dev_API yarp::dev::IControlMode
{
public:
    virtual ~IControlMode(){}

    /**
    * Get the current control mode.
    * @param j joint number
    * @param mode a vocab of the current control mode for joint j.
    * @return: true/false success failure.
    */
    virtual bool getControlMode(int j, int *mode)=0;

    /**
    * Get the current control mode (multiple joints).
    * @param modes a vector containing vocabs for the current control modes of the joints.
    * @return: true/false success failure.
    */
    virtual bool getControlModes(int *modes)=0;

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
class yarp::dev::IControlModeRaw
{
public:
    virtual ~IControlModeRaw(){}
    virtual bool getControlModeRaw(int j, int *mode)=0;
    virtual bool getControlModesRaw(int* modes)=0;
};


// new style VOCABS
// Interface
#define VOCAB_ICONTROLMODE          VOCAB4('i','c','m','d')

// Methods
#define VOCAB_CM_CONTROL_MODE           VOCAB4('c','m','o','d')
#define VOCAB_CM_CONTROL_MODE_GROUP     VOCAB4('c','m','o','g')
#define VOCAB_CM_CONTROL_MODES          VOCAB4('c','m','d','s')

// Values
// Read / Write
#define VOCAB_CM_IDLE               VOCAB3('i','d','l')
#define VOCAB_CM_TORQUE             VOCAB4('t','o','r','q')
#define VOCAB_CM_POSITION           VOCAB3('p','o','s')
#define VOCAB_CM_POSITION_DIRECT    VOCAB4('p','o','s','d')
#define VOCAB_CM_VELOCITY           VOCAB3('v','e','l')
#define VOCAB_CM_CURRENT            VOCAB4('i','c','u','r')
#define VOCAB_CM_PWM                VOCAB4('i','p','w','m')
#define VOCAB_CM_IMPEDANCE_POS      VOCAB4('i','m','p','o')  // deprecated
#define VOCAB_CM_IMPEDANCE_VEL      VOCAB4('i','m','v','e')  // deprecated

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

// Read only (cannot be set from user)
#define VOCAB_CM_UNKNOWN            VOCAB4('u','n','k','w')

#endif // YARP_DEV_ICONTROLMODE_H
