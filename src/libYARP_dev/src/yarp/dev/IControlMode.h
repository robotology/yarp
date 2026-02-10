/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ICONTROLMODE_H
#define YARP_DEV_ICONTROLMODE_H

#include <vector>
#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {
class IControlModeRaw;
class IControlMode;

enum class SelectableControlModeEnum
{
    VOCAB_CM_IDLE            =   yarp::os::createVocab32('i','d','l'),
    VOCAB_CM_TORQUE          =   yarp::os::createVocab32('t','o','r','q'),
    VOCAB_CM_POSITION        =   yarp::os::createVocab32('p','o','s'),
    VOCAB_CM_POSITION_DIRECT =   yarp::os::createVocab32('p','o','s','d'),
    VOCAB_CM_VELOCITY        =   yarp::os::createVocab32('v','e','l'),
    VOCAB_CM_VELOCITY_DIRECT =   yarp::os::createVocab32('v','e','l','d'),
    VOCAB_CM_CURRENT         =   yarp::os::createVocab32('i','c','u','r'),
    VOCAB_CM_PWM             =   yarp::os::createVocab32('i','p','w','m'),
    VOCAB_CM_IMPEDANCE_POS   =   yarp::os::createVocab32('i','m','p','o'),  // deprecated
    VOCAB_CM_IMPEDANCE_VEL   =   yarp::os::createVocab32('i','m','v','e'),  // deprecated
    VOCAB_CM_MIXED           =   yarp::os::createVocab32('m','i','x'),
    VOCAB_CM_FORCE_IDLE      =   yarp::os::createVocab32('f','i','d','l')
};

enum class ControlModeEnum
{
    VOCAB_CM_IDLE            =   yarp::os::createVocab32('i','d','l'),
    VOCAB_CM_TORQUE          =   yarp::os::createVocab32('t','o','r','q'),
    VOCAB_CM_POSITION        =   yarp::os::createVocab32('p','o','s'),
    VOCAB_CM_POSITION_DIRECT =   yarp::os::createVocab32('p','o','s','d'),
    VOCAB_CM_VELOCITY        =   yarp::os::createVocab32('v','e','l'),
    VOCAB_CM_VELOCITY_DIRECT =   yarp::os::createVocab32('v','e','l','d'),
    VOCAB_CM_CURRENT         =   yarp::os::createVocab32('i','c','u','r'),
    VOCAB_CM_PWM             =   yarp::os::createVocab32('i','p','w','m'),
    VOCAB_CM_IMPEDANCE_POS   =   yarp::os::createVocab32('i','m','p','o'),  // deprecated
    VOCAB_CM_IMPEDANCE_VEL   =   yarp::os::createVocab32('i','m','v','e'),  // deprecated
    VOCAB_CM_MIXED           =   yarp::os::createVocab32('m','i','x'),
    VOCAB_CM_FORCE_IDLE      =   yarp::os::createVocab32('f','i','d','l'),
    VOCAB_CM_HW_FAULT        =   yarp::os::createVocab32('h','w','f','a'),
    VOCAB_CM_CALIBRATING     =   yarp::os::createVocab32('c','a','l'),     // the joint is calibrating
    VOCAB_CM_CALIB_DONE      =   yarp::os::createVocab32('c','a','l','d'), // calibration successfully completed
    VOCAB_CM_NOT_CONFIGURED  =   yarp::os::createVocab32('c','f','g','n'), // missing initial configuration (default value at start-up)
    VOCAB_CM_CONFIGURED      =   yarp::os::createVocab32('c','f','g','y'), // initial configuration completed, if any
    VOCAB_CM_UNKNOWN         =   yarp::os::createVocab32('u','n','k','w')
};
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

    bool isAvailable(int j,yarp::dev::SelectableControlModeEnum)
    {
    }

    /**
    * Return a list of available control mode for the given joint.
    * @param j joint number
    * @param avail a vector that will contain the list of available control modes for joint j.
    * @return: true/false success failure.
    */
    virtual yarp::dev::ReturnValue getAvailableControlModes(int j, std::vector<yarp::dev::SelectableControlModeEnum>& avail)=0;

    /**
    * Get the current control mode.
    * @param j joint number
    * @param mode a vocab of the current control mode for joint j.
    * @return: true/false success failure.
    */
    virtual yarp::dev::ReturnValue getControlMode(int j, yarp::dev::ControlModeEnum& mode)=0;
    virtual yarp::dev::ReturnValue getControlMode(int j, int *mode)
    {
        yarp::dev::ControlModeEnum mode_e;
        yarp::dev::ReturnValue ret = getControlMode(j, mode_e);
        *mode = static_cast<int>(mode_e);
        return ret;
    }

    /**
    * Get the current control mode (multiple joints).
    * @param modes a vector containing vocabs for the current control modes of the joints.
    * @return: true/false success failure.
    */
    virtual yarp::dev::ReturnValue getControlModes(std::vector<yarp::dev::ControlModeEnum>& mode)=0;

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
    virtual yarp::dev::ReturnValue getControlModes(std::vector<int> joints, std::vector<yarp::dev::ControlModeEnum>& mode)=0;

    /**
    * Set the current control mode.
    * @param j: joint number
    * @param mode: a vocab of the desired control mode for joint j.
    * @return true if the new controlMode was successfully set, false if the message was not received or
    *         the joint was unable to switch to the desired controlMode
    *         (e.g. the joint is on a fault condition or the desired mode is not implemented).    */
    virtual yarp::dev::ReturnValue setControlMode(int j, yarp::dev::SelectableControlModeEnum mode)=0;
    virtual yarp::dev::ReturnValue setControlMode(const int j, const int mode)
    {
        yarp::dev::SelectableControlModeEnum emode = static_cast<yarp::dev::SelectableControlModeEnum>(mode);
        yarp::dev::ReturnValue ret = setControlMode(j, emode);
        return ret;
    }

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
    virtual yarp::dev::ReturnValue setControlModes(std::vector<int> joints, std::vector<yarp::dev::SelectableControlModeEnum> mode)=0;

    /**
    * Set the current control mode (multiple joints).
    * @param modes: a vector containing vocabs for the desired control modes of the joints.
    * @return true if the new controlMode was successfully set, false if the message was not received or
    *         the joint was unable to switch to the desired controlMode
    *         (e.g. the joint is on a fault condition or the desired mode is not implemented).
    */
    virtual yarp::dev::ReturnValue setControlModes(std::vector<yarp::dev::SelectableControlModeEnum> mode)=0;

};


/**
 * @ingroup dev_iface_motor_raw
 *
 * Interface for setting control mode in control board. See IControlMode for
 * more documentation.
 */
class YARP_dev_API yarp::dev::IControlModeRaw
{
public:
    virtual ~IControlModeRaw(){}
    virtual yarp::dev::ReturnValue getAvailableControlModesRaw(int j, std::vector<yarp::dev::SelectableControlModeEnum>& avail)=0;

    virtual yarp::dev::ReturnValue getControlModeRaw(int j, yarp::dev::ControlModeEnum& mode)=0;
    virtual yarp::dev::ReturnValue getControlModeRaw(int j, int *mode)
    {
        yarp::dev::ControlModeEnum mode_enum;
        yarp::dev::ReturnValue ret = getControlModeRaw(j, mode_enum);
        *mode = static_cast<int>(mode_enum);
        return ret;
    }

    virtual yarp::dev::ReturnValue getControlModesRaw(std::vector<yarp::dev::ControlModeEnum>& mode)=0;
    virtual yarp::dev::ReturnValue getControlModesRaw(std::vector<int> joints, std::vector<yarp::dev::ControlModeEnum>& mode)=0;


    virtual yarp::dev::ReturnValue setControlModeRaw(int j, yarp::dev::SelectableControlModeEnum mode)=0;
    virtual yarp::dev::ReturnValue setControlModeRaw(const int j, const int mode)
    {
        yarp::dev::ReturnValue ret = setControlModeRaw(j, static_cast<yarp::dev::SelectableControlModeEnum>(mode));
        return ret;
    }

    virtual yarp::dev::ReturnValue setControlModesRaw(std::vector<int> joints, std::vector<yarp::dev::SelectableControlModeEnum> mode)=0;
    virtual yarp::dev::ReturnValue setControlModesRaw(const std::vector<yarp::dev::SelectableControlModeEnum> mode)=0;
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
constexpr yarp::conf::vocab32_t VOCAB_CM_VELOCITY_DIRECT =   yarp::os::createVocab32('v','e','l','d');
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
