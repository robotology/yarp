/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    *          The first value will be the new reference for the joint joints[0].
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
    *          The first value will be the new reference for the joint joints[0].
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
class YARP_dev_API yarp::dev::IControlModeRaw
{
public:
    virtual ~IControlModeRaw(){}
    virtual bool getControlModeRaw(int j, int *mode)=0;
    virtual bool getControlModesRaw(int* modes)=0;
    virtual bool getControlModesRaw(const int n_joint, const int *joints, int *modes)=0;
    virtual bool setControlModeRaw(const int j, const int mode)=0;
    virtual bool setControlModesRaw(const int n_joint, const int *joints, int *modes)=0;
    virtual bool setControlModesRaw(int *modes)=0;
};


// new style VOCABS
// Interface
constexpr yarp::conf::vocab32_t VOCAB_ICONTROLMODE = yarp::os::createVocab32('i','c','m','d');
// Methods
constexpr yarp::conf::vocab32_t VOCAB_CM_CONTROL_MODE       =   yarp::os::createVocab32('c','m','o','d');
constexpr yarp::conf::vocab32_t VOCAB_CM_CONTROL_MODE_GROUP =   yarp::os::createVocab32('c','m','o','g');
constexpr yarp::conf::vocab32_t VOCAB_CM_CONTROL_MODES      =   yarp::os::createVocab32('c','m','d','s');

// Values
// Read / Write
constexpr yarp::conf::vocab32_t VOCAB_CM_IDLE            =   yarp::os::createVocab32('i','d','l');
constexpr yarp::conf::vocab32_t VOCAB_CM_TORQUE          =   yarp::os::createVocab32('t','o','r','q');
constexpr yarp::conf::vocab32_t VOCAB_CM_POSITION        =   yarp::os::createVocab32('p','o','s');
constexpr yarp::conf::vocab32_t VOCAB_CM_POSITION_DIRECT =   yarp::os::createVocab32('p','o','s','d');
constexpr yarp::conf::vocab32_t VOCAB_CM_VELOCITY        =   yarp::os::createVocab32('v','e','l');
constexpr yarp::conf::vocab32_t VOCAB_CM_CURRENT         =   yarp::os::createVocab32('i','c','u','r');
constexpr yarp::conf::vocab32_t VOCAB_CM_PWM             =   yarp::os::createVocab32('i','p','w','m');
constexpr yarp::conf::vocab32_t VOCAB_CM_IMPEDANCE_POS   =   yarp::os::createVocab32('i','m','p','o');  // deprecated
constexpr yarp::conf::vocab32_t VOCAB_CM_IMPEDANCE_VEL   =   yarp::os::createVocab32('i','m','v','e');  // deprecated

// Values
// Read / Write
constexpr yarp::conf::vocab32_t  VOCAB_CM_MIXED = yarp::os::createVocab32('m','i','x');

// Write only (only from high level toward the joint)
constexpr yarp::conf::vocab32_t  VOCAB_CM_FORCE_IDLE =   yarp::os::createVocab32('f','i','d','l');

// Read only (imposed by the board on special events)
constexpr yarp::conf::vocab32_t  VOCAB_CM_HW_FAULT       =    yarp::os::createVocab32('h','w','f','a');
constexpr yarp::conf::vocab32_t  VOCAB_CM_CALIBRATING    =    yarp::os::createVocab32('c','a','l');     // the joint is calibrating
constexpr yarp::conf::vocab32_t  VOCAB_CM_CALIB_DONE     =    yarp::os::createVocab32('c','a','l','d'); // calibration successfully completed
constexpr yarp::conf::vocab32_t  VOCAB_CM_NOT_CONFIGURED =    yarp::os::createVocab32('c','f','g','n'); // missing initial configuration (default value at start-up)
constexpr yarp::conf::vocab32_t  VOCAB_CM_CONFIGURED     =    yarp::os::createVocab32('c','f','g','y'); // initial configuration completed, if any

// Read only (cannot be set from user)
constexpr yarp::conf::vocab32_t VOCAB_CM_UNKNOWN =   yarp::os::createVocab32('u','n','k','w');

#endif // YARP_DEV_ICONTROLMODE_H
