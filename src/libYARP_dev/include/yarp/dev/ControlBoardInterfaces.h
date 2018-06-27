/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDINTERFACES_H
#define YARP_DEV_CONTROLBOARDINTERFACES_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardPid.h>
#include <yarp/dev/CalibratorInterfaces.h>

/*! \file ControlBoardInterfaces.h define control board standard interfaces*/

#include <yarp/dev/IEncoders.h>
#include <yarp/dev/IEncodersTimed.h>
#include <yarp/dev/ITorqueControl.h>
#include <yarp/dev/IControlMode2.h>
#include <yarp/dev/IImpedanceControl.h>
#include <yarp/dev/IVelocityControl.h>
#include <yarp/dev/IVelocityControl2.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/IPositionControl2.h>
#include <yarp/dev/ICurrentControl.h>
#include <yarp/dev/IPWMControl.h>
#include <yarp/dev/IPidControl.h>
#include <yarp/dev/IPositionDirect.h>
#include <yarp/dev/IInteractionMode.h>
#include <yarp/dev/IMotorEncoders.h>
#include <yarp/dev/IMotor.h>
#include <yarp/dev/IRemoteVariables.h>

namespace yarp {
    namespace dev {
        class IPidControlRaw;
        class IPidControl;
        class IEncodersRaw;
        class IEncoders;
        class IMotor;
        class IMotorRaw;
        class IMotorEncodersRaw;
        class IMotorEncoders;
        class IAmplifierControlRaw;
        class IAmplifierControl;
        class IControlDebug;
        class IControlLimitsRaw;
        class IControlLimits;
        class IControlCalibrationRaw;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IControlCalibrationRaw instead") IControlCalibrationRaw IControlCalibration2Raw;
#endif
        class IControlCalibration;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IControlCalibration instead") IControlCalibration IControlCalibration2;
#endif
        class IAxisInfo;
        class IAxisInfoRaw;
        struct CalibrationParameters;

        enum JointTypeEnum
        {
            VOCAB_JOINTTYPE_REVOLUTE  = yarp::os::createVocab('a', 't', 'r', 'v'),
            VOCAB_JOINTTYPE_PRISMATIC = yarp::os::createVocab('a', 't', 'p', 'r'),
            VOCAB_JOINTTYPE_UNKNOWN   = yarp::os::createVocab('u', 'n', 'k', 'n')
        };
    }
}

struct YARP_dev_API yarp::dev::CalibrationParameters
{
    unsigned int type;
    double param1;
    double param2;
    double param3;
    double param4;
    double param5;
    double paramZero;
    CalibrationParameters() { type = 0; param1 = 0; param2 = 0; param3 = 0; param4 = 0; param5 = 0; paramZero = 0; }
};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, amplifier commands.
 */
class YARP_dev_API yarp::dev::IAmplifierControl
{
public:
    /**
     * Destructor.
     */
    virtual ~IAmplifierControl() {}

    /** Enable the amplifier on a specific joint. Be careful, check that the output
     * of the controller is appropriate (usually zero), to avoid
     * generating abrupt movements.
     * @return true/false on success/failure
     */
    virtual bool enableAmp(int j)=0;

    /** Disable the amplifier on a specific joint. All computations within the board
     * will be carried out normally, but the output will be disabled.
     * @return true/false on success/failure
     */
    virtual bool disableAmp(int j)=0;

    /* Get the status of the amplifiers, coded in a 32 bits integer for
     * each amplifier (at the moment contains only the fault, it will be
     * expanded in the future).
     * @param st pointer to storage
     * @return true in good luck, false otherwise.
     */
    virtual bool getAmpStatus(int *st)=0;

     /* Get the status of a single amplifier, coded in a 32 bits integer
     * @param j joint number
     * @param st storage for return value
     * @return true/false success failure.
     */
    virtual bool getAmpStatus(int j, int *v)=0;

    /* Read the electric current going to all motors.
     * @param vals pointer to storage for the output values
     * @return hopefully true, false in bad luck.
     */
    virtual bool getCurrents(double *vals)=0;

    /* Read the electric current going to a given motor.
     * @param j motor number
     * @param val pointer to storage for the output value
     * @return probably true, might return false in bad times
     */
    virtual bool getCurrent(int j, double *val)=0;

    /**
    * Returns the maximum electric current allowed for a given motor.
    * Exceeding this value will trigger instantaneous hardware fault.
    * @param j motor number
    * @param v the return value
    * @return probably true, might return false in bad times
    */
    virtual bool getMaxCurrent(int j, double *v)=0;

    /* Set the maximum electric current going to a given motor.
     * Exceeding this value will trigger instantaneous hardware fault.
     * @param j motor number
     * @param v the new value
     * @return probably true, might return false in bad times
     */
    virtual bool setMaxCurrent(int j, double v)=0;

    /* Get the the nominal current which can be kept for an indefinite amount of time
     * without harming the motor. This value is specific for each motor and it is typically
     * found in its datasheet. The units are Ampere.
     * This value and the peak current may be used by the firmware to configure
     * an I2T filter.
     * @param j joint number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool getNominalCurrent(int m, double *val) {return false;};

    /* Set the the nominal current which can be kept for an indefinite amount of time
    * without harming the motor. This value is specific for each motor and it is typically
    * found in its datasheet. The units are Ampere.
    * This value and the peak current may be used by the firmware to configure
    * an I2T filter.
    * @param j joint number
    * @param val storage for return value. [Ampere]
    * @return true/false success failure.
    */
    virtual bool setNominalCurrent(int m, const double val) { return false; };

    /* Get the the peak current which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor datasheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param j joint number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool getPeakCurrent(int m, double *val) {return false;};

    /* Set the the peak current. This value  which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor datasheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param j joint number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool setPeakCurrent(int m, const double val) {return false;};

    /* Get the the current PWM value used to control the motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param j joint number
     * @param val filled with PWM value.
     * @return true/false success failure.
     */
    virtual bool getPWM(int j, double* val) {return false;};

    /* Get the PWM limit fot the given motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param j joint number
     * @param val filled with PWM limit value.
     * @return true/false success failure.
     */
    virtual bool getPWMLimit(int j, double* val) {return false;};

    /* Set the PWM limit fot the given motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param j joint number
     * @param val new value for the PWM limit.
     * @return true/false success failure.
     */
    virtual bool setPWMLimit(int j, const double val) {return false;};

    /* Get the power source voltage for the given motor in Volt.
     * @param j joint number
     * @param val filled with return value.
     * @return true/false success failure.
     */
    virtual bool getPowerSupplyVoltage(int j, double* val) {return false;};
};

/**
 *
 * Interface for control devices, amplifier commands.
 */
class YARP_dev_API yarp::dev::IAmplifierControlRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IAmplifierControlRaw() {}

    /** Enable the amplifier on a specific joint. Be careful, check that the output
     * of the controller is appropriate (usually zero), to avoid
     * generating abrupt movements.
     * @return true/false on success/failure
     */
    virtual bool enableAmpRaw(int j)=0;

    /** Disable the amplifier on a specific joint. All computations within the board
     * will be carried out normally, but the output will be disabled.
     * @return true/false on success/failure
     */
    virtual bool disableAmpRaw(int j)=0;

    /* Get the status of the amplifiers, coded in a 32 bits integer for
     * each amplifier (at the moment contains only the fault, it will be
     * expanded in the future).
     * @param st pointer to storage
     * @return true/false success failure.
     */
    virtual bool getAmpStatusRaw(int *st)=0;

    /* Get the status of a single amplifier, coded in a 32 bits integer
     * @param j joint number
     * @param st storage for return value
     * @return true/false success failure.
     */
    virtual bool getAmpStatusRaw(int j, int *st)=0;

    /* Read the electric current going to all motors.
     * @param vals pointer to storage for the output values
     * @return hopefully true, false in bad luck.
     */
    virtual bool getCurrentsRaw(double *vals)=0;

    /* Read the electric current going to a given motor.
     * @param j motor number
     * @param val pointer to storage for the output value
     * @return probably true, might return false in bad times
     */
    virtual bool getCurrentRaw(int j, double *val)=0;

    /* Set the maximum electric current going to a given motor.
     * Exceeding this value will trigger instantaneous hardware fault.
     * @param j motor number
     * @param v the new value
     * @return probably true, might return false in bad times
     */
    virtual bool setMaxCurrentRaw(int j, double v)=0;

    /**
    * Returns the maximum electric current allowed for a given motor.
    * Exceeding this value will trigger instantaneous hardware fault.
    * @param j motor number
    * @param v the return value
    * @return probably true, might return false in bad times
    */
    virtual bool getMaxCurrentRaw(int j, double *v)=0;

    /* Get the the nominal current which can be kept for an indefinite amount of time
     * without harming the motor. This value is specific for each motor and it is typically
     * found in its datasheet. The units are Ampere.
     * This value and the peak current may be used by the firmware to configure
     * an I2T filter.
     * @param j joint number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool getNominalCurrentRaw(int m, double *val) {return false;};

    /* Set the the nominal current which can be kept for an indefinite amount of time
    * without harming the motor. This value is specific for each motor and it is typically
    * found in its datasheet. The units are Ampere.
    * This value and the peak current may be used by the firmware to configure
    * an I2T filter.
    * @param j joint number
    * @param val storage for return value. [Ampere]
    * @return true/false success failure.
    */
    virtual bool setNominalCurrentRaw(int m, const double val) { return false; };

    /* Get the the peak current which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor datasheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param j joint number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool getPeakCurrentRaw(int m, double *val) {return false;};

    /* Set the the peak current. This value  which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor datasheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param j joint number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool setPeakCurrentRaw(int m, const double val) {return false;};

    /* Get the the current PWM value used to control the motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param j joint number
     * @param val filled with PWM value.
     * @return true/false success failure.
     */
    virtual bool getPWMRaw(int j, double* val) {return false;};

    /* Get the PWM limit fot the given motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param j joint number
     * @param val filled with PWM limit value.
     * @return true/false success failure.
     */
    virtual bool getPWMLimitRaw(int j, double* val) {return false;};

    /* Set the PWM limit fot the given motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param j joint number
     * @param val new value for the PWM limit.
     * @return true/false success failure.
     */
    virtual bool setPWMLimitRaw(int j, const double val) {return false;};

    /* Get the power source voltage for the given motor in Volt.
     * @param j joint number
     * @param val filled with return value. [Volt]
     * @return true/false success failure.
     */
    virtual bool getPowerSupplyVoltageRaw(int j, double* val) {return false;};
};

/**
 *
 * Interface for control devices, calibration commands.
 */
class YARP_dev_API yarp::dev::IControlCalibrationRaw
{
public:
    IControlCalibrationRaw(){}
    /**
     * Destructor.
     */
    virtual ~IControlCalibrationRaw() {}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
    /**
     *  Start calibration, this method is very often platform
     * specific.
     * @deprecated Since YARP 3.0.0
     * @return true/false on success failure
     */
    virtual bool calibrateRaw(int j, double p) { return false; }

    /**
     * Start calibration, this method is very often platform
     * specific.
     * @deprecated Since YARP 3.0.0
     * @return true/false on success failure
     */
    virtual bool calibrate2Raw(int axis, unsigned int type, double p1, double p2, double p3) { return false;}
#endif

    /**
     * Check if the calibration is terminated, on a particular joint.
     * Non blocking.
     * @return true/false
     */
    virtual bool doneRaw(int j)=0;

    /**
     *  Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     */
    virtual bool calibrateRaw(int axis, unsigned int type, double p1, double p2, double p3)=0;

    /**
     *  Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
    */
    virtual bool setCalibrationParametersRaw(int axis, const CalibrationParameters& params) { return false; }

};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, calibration commands.
 */
class YARP_dev_API yarp::dev::IControlCalibration
{
private:
    ICalibrator *calibrator;

public:
    IControlCalibration();
    /**
     * Destructor.
     */
    virtual ~IControlCalibration() {}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
    /**
     *  Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     * @deprecated Since YARP 3.0.0
     */
    YARP_DEPRECATED
    virtual bool calibrate(int j, double p) { return false; }

    /**
     *  Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     * @deprecated Since YARP 3.0.0
     */
    YARP_DEPRECATED_MSG("Use calibrate instead")
    virtual bool calibrate2(int axis, unsigned int type, double p1, double p2, double p3) { return calibrate(axis, type, p1, p2, p3); }
#endif

    /**
     *  Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     */
    virtual bool calibrate(int axis, unsigned int type, double p1, double p2, double p3)=0;

    /**
     *  Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     */
    virtual bool setCalibrationParameters(int axis, const CalibrationParameters& params) { return false; }

    /**
     *  Check if the calibration is terminated, on a particular joint.
     * Non blocking.
     * @return true/false
     */
    virtual bool done(int j)=0;

    /**
     * Set the calibrator object to be used to calibrate the robot.
     * @param c pointer to the calibrator object
     * @return true/false on success failure
     */
    virtual bool setCalibrator(ICalibrator *c);

    /**
     * Calibrate robot by using an external calibrator. The external
     * calibrator must be previously set by calling the setCalibration()
     * method.
     * @return true/false on success failure
     */
    virtual bool calibrate();

    virtual bool park(bool wait=true);

    /* Abort calibration, force the function calibrate() to return.*/
    virtual bool abortCalibration();

    /* Abort parking, force the function park() to return.*/
    virtual bool abortPark();



};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, debug commands.
 */
class YARP_dev_API yarp::dev::IControlDebug
{
public:
    /**
     * Destructor.
     */
    virtual ~IControlDebug() {}

    /* Set the print function, pass here a pointer to your own function
     * to print. This function should implement "printf" like parameters.
     * @param a pointer to the print function
     * @return I don't see good reasons why it should return false.
     */
    virtual bool setPrintFunction(int (*f) (const char *fmt, ...))=0;

    /* Read the content of the board internal memory, this is usually done
     * at boot time, but can be forced by calling this method.
     * @return true/false on success failure
     */
    virtual bool loadBootMemory()=0;

    /* Save the current board configuration to the internal memory,
     * this values are read at boot time or if loadBootMemory() is called.
     * @return true/false on success/failure
     */
    virtual bool saveBootMemory()=0;
};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, commands to get/set position and veloity limits.
 */
class YARP_dev_API yarp::dev::IControlLimits
{
public:
    /**
     * Destructor.
     */
    virtual ~IControlLimits() {}

    /**
     * Set the software limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number (why am I telling you this)
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual bool setLimits(int axis, double min, double max)=0;

    /**
     * Get the software limits for a particular axis.
     * @param axis joint number
     * @param pointer to store the value of the lower limit
     * @param pointer to store the value of the upper limit
     * @return true if everything goes fine, false otherwise.
     */
    virtual bool getLimits(int axis, double *min, double *max)=0;

    /**
     * Set the software speed limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual bool setVelLimits(int axis, double min, double max)=0;

    /**
     * Get the software speed limits for a particular axis.
     * @param axis joint number
     * @param min pointer to store the value of the lower limit
     * @param max pointer to store the value of the upper limit
     * @return true if everything goes fine, false otherwise.
     */
    virtual bool getVelLimits(int axis, double *min, double *max)=0;
};

/**
 * Interface for control devices. Limits commands.
 */
class YARP_dev_API yarp::dev::IControlLimitsRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IControlLimitsRaw() {}

    /**
     *  Set the software limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number (why am I telling you this)
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual bool setLimitsRaw(int axis, double min, double max)=0;

    /**
     * Get the software limits for a particular axis.
     * @param axis joint number
     * @param pointer to store the value of the lower limit
     * @param pointer to store the value of the upper limit
     * @return true if everything goes fine, false otherwise.
     */
    virtual bool getLimitsRaw(int axis, double *min, double *max)=0;

    /**
     * Set the software speed limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual bool setVelLimitsRaw(int axis, double min, double max)=0;

    /**
     * Get the software speed limits for a particular axis.
     * @param axis joint number
     * @param min pointer to store the value of the lower limit
     * @param max pointer to store the value of the upper limit
     * @return true if everything goes fine, false otherwise.
     */
    virtual bool getVelLimitsRaw(int axis, double *min, double *max)=0;
};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for getting information about specific axes, if available.
 */
class YARP_dev_API yarp::dev::IAxisInfo
{
public:
    /**
     * Destructor.
     */
    virtual ~IAxisInfo() {}

    /* Get the name for a particular axis.
    * @param axis joint number
    * @param name the axis name
    * @return true if everything goes fine, false otherwise.
    */
    virtual bool getAxisName(int axis, std::string& name) = 0;

    /* Get the joint type (e.g. revolute/prismatic) for a particular axis.
    * @param axis joint number
    * @param type the joint type
    * @return true if everything goes fine, false otherwise.
    */
    virtual bool getJointType(int axis, yarp::dev::JointTypeEnum& type) { yFatal("getJointType() not implemented on your device, cannot proceed further. Please report the problem on yarp issue tracker"); return false; }
};

/**
* Interface for getting information about specific axes, if available.
*/
class YARP_dev_API yarp::dev::IAxisInfoRaw
{
public:
    /**
    * Destructor.
    */
    virtual ~IAxisInfoRaw() {}

    /* Get the name for a particular axis.
    * @param axis joint number
    * @param name the axis name
    * @return true if everything goes fine, false otherwise.
    */
    virtual bool getAxisNameRaw(int axis, std::string& name) = 0;

    /* Get the joint type (e.g. revolute/prismatic) for a particular axis.
    * @param axis joint number
    * @param type the joint type
    * @return true if everything goes fine, false otherwise.
    */
    virtual bool getJointTypeRaw(int axis, yarp::dev::JointTypeEnum& type)  { yFatal("getJointType() not implemented on your device, cannot proceed further. Please report the problem on yarp issue tracker"); return false; };
};

/* Vocabs representing the above interfaces */

constexpr yarp::conf::vocab32_t VOCAB_CALIBRATE_JOINT        = yarp::os::createVocab('c','a','l','j');
constexpr yarp::conf::vocab32_t VOCAB_CALIBRATE_JOINT_PARAMS = yarp::os::createVocab('c','l','j','p');
constexpr yarp::conf::vocab32_t VOCAB_CALIBRATE              = yarp::os::createVocab('c','a','l');
constexpr yarp::conf::vocab32_t VOCAB_CALIBRATE_CHANNEL      = yarp::os::createVocab('c','a','l','c');
constexpr yarp::conf::vocab32_t VOCAB_ABORTCALIB             = yarp::os::createVocab('a','b','c','a');
constexpr yarp::conf::vocab32_t VOCAB_ABORTPARK              = yarp::os::createVocab('a','b','p','a');
constexpr yarp::conf::vocab32_t VOCAB_CALIBRATE_DONE         = yarp::os::createVocab('c','a','l','d');
constexpr yarp::conf::vocab32_t VOCAB_PARK                   = yarp::os::createVocab('p','a','r','k');

constexpr yarp::conf::vocab32_t VOCAB_MOTION_DONE  = yarp::os::createVocab('d','o','n');
constexpr yarp::conf::vocab32_t VOCAB_MOTION_DONES = yarp::os::createVocab('d','o','n','s');

// interface IPositionControl sets
constexpr yarp::conf::vocab32_t VOCAB_POSITION_MODE  = yarp::os::createVocab('p','o','s','d');
constexpr yarp::conf::vocab32_t VOCAB_POSITION_MOVE  = yarp::os::createVocab('p','o','s');
constexpr yarp::conf::vocab32_t VOCAB_POSITION_MOVES = yarp::os::createVocab('p','o','s','s');
constexpr yarp::conf::vocab32_t VOCAB_RELATIVE_MOVE  = yarp::os::createVocab('r','e','l');

constexpr yarp::conf::vocab32_t VOCAB_RELATIVE_MOVES    = yarp::os::createVocab('r','e','l','s');
constexpr yarp::conf::vocab32_t VOCAB_REF_SPEED         = yarp::os::createVocab('v','e','l');
constexpr yarp::conf::vocab32_t VOCAB_REF_SPEEDS        = yarp::os::createVocab('v','e','l','s');
constexpr yarp::conf::vocab32_t VOCAB_REF_ACCELERATION  = yarp::os::createVocab('a','c','c');
constexpr yarp::conf::vocab32_t VOCAB_REF_ACCELERATIONS = yarp::os::createVocab('a','c','c','s');
constexpr yarp::conf::vocab32_t VOCAB_STOP              = yarp::os::createVocab('s','t','o');
constexpr yarp::conf::vocab32_t VOCAB_STOPS             = yarp::os::createVocab('s','t','o','s');

// interface IVelocityControl sets
constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_MODE  = yarp::os::createVocab('v','e','l','d');
constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_MOVE  = yarp::os::createVocab('v','m','o');
constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_MOVES = yarp::os::createVocab('v','m','o','s');

// interface IAmplifierControl sets/gets
constexpr yarp::conf::vocab32_t VOCAB_AMP_ENABLE            = yarp::os::createVocab('a','e','n');
constexpr yarp::conf::vocab32_t VOCAB_AMP_DISABLE           = yarp::os::createVocab('a','d','i');
constexpr yarp::conf::vocab32_t VOCAB_AMP_STATUS            = yarp::os::createVocab('a','s','t','a');
constexpr yarp::conf::vocab32_t VOCAB_AMP_STATUS_SINGLE     = yarp::os::createVocab('a','s','t','s');
constexpr yarp::conf::vocab32_t VOCAB_AMP_CURRENT           = yarp::os::createVocab('a','c','u');
constexpr yarp::conf::vocab32_t VOCAB_AMP_CURRENTS          = yarp::os::createVocab('a','c','u','s');
constexpr yarp::conf::vocab32_t VOCAB_AMP_MAXCURRENT        = yarp::os::createVocab('m','a','x','c');
constexpr yarp::conf::vocab32_t VOCAB_AMP_NOMINAL_CURRENT   = yarp::os::createVocab('a','c','n','o');
constexpr yarp::conf::vocab32_t VOCAB_AMP_PEAK_CURRENT      = yarp::os::createVocab('a','c','p','k');
constexpr yarp::conf::vocab32_t VOCAB_AMP_PWM               = yarp::os::createVocab('p','w','m');
constexpr yarp::conf::vocab32_t VOCAB_AMP_PWM_LIMIT         = yarp::os::createVocab('p','w','m','l');
constexpr yarp::conf::vocab32_t VOCAB_AMP_VOLTAGE_SUPPLY    = yarp::os::createVocab('a','v','s','u');

// interface IControlLimits sets/gets
constexpr yarp::conf::vocab32_t VOCAB_LIMITS     = yarp::os::createVocab('l','l','i','m');
constexpr yarp::conf::vocab32_t VOCAB_VEL_LIMITS = yarp::os::createVocab('v','l','i','m');


// interface IAxisInfo
constexpr yarp::conf::vocab32_t VOCAB_INFO_NAME            = yarp::os::createVocab('n','a','m','e');
constexpr yarp::conf::vocab32_t VOCAB_INFO_TYPE            = yarp::os::createVocab('t','y','p','e');
constexpr yarp::conf::vocab32_t VOCAB_TIMESTAMP            = yarp::os::createVocab('t','s','t','a');
constexpr yarp::conf::vocab32_t VOCAB_TORQUE               = yarp::os::createVocab('t','o','r','q');
constexpr yarp::conf::vocab32_t VOCAB_TORQUE_MODE          = yarp::os::createVocab('t','r','q','d');
constexpr yarp::conf::vocab32_t VOCAB_TRQS                 = yarp::os::createVocab('t','r','q','s');
constexpr yarp::conf::vocab32_t VOCAB_TRQ                  = yarp::os::createVocab('t','r','q');
constexpr yarp::conf::vocab32_t VOCAB_BEMF                 = yarp::os::createVocab('b','m','f');
constexpr yarp::conf::vocab32_t VOCAB_MOTOR_PARAMS         = yarp::os::createVocab('m','t','p','s');
constexpr yarp::conf::vocab32_t VOCAB_RANGES               = yarp::os::createVocab('r','n','g','s');
constexpr yarp::conf::vocab32_t VOCAB_RANGE                = yarp::os::createVocab('r','n','g');
constexpr yarp::conf::vocab32_t VOCAB_IMP_PARAM            = yarp::os::createVocab('i','p','r');
constexpr yarp::conf::vocab32_t VOCAB_IMP_OFFSET           = yarp::os::createVocab('i','o','f');
constexpr yarp::conf::vocab32_t VOCAB_TORQUES_DIRECTS      = yarp::os::createVocab('d','t','q','s'); //This implements the setRefTorques for the whole part
constexpr yarp::conf::vocab32_t VOCAB_TORQUES_DIRECT       = yarp::os::createVocab('d','t','q'); //This implements the setRefTorque for a single joint
constexpr yarp::conf::vocab32_t VOCAB_TORQUES_DIRECT_GROUP = yarp::os::createVocab('d','t','q','g'); //This implements the setRefTorques with joint list

// protocol version
constexpr yarp::conf::vocab32_t VOCAB_PROTOCOL_VERSION = yarp::os::createVocab('p', 'r', 'o', 't');

#endif // YARP_DEV_CONTROLBOARDINTERFACES_H
