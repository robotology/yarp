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
        class IControlCalibration2Raw;
        class IControlCalibration;
        class IControlCalibration2;
        class IAxisInfo;
        class IAxisInfoRaw;
        struct CalibrationParameters;

        enum JointTypeEnum
        {
            VOCAB_JOINTTYPE_REVOLUTE  = VOCAB4('a', 't', 'r', 'v'),
            VOCAB_JOINTTYPE_PRISMATIC = VOCAB4('a', 't', 'p', 'r'),
            VOCAB_JOINTTYPE_UNKNOWN   = VOCAB4('u', 'n', 'k', 'n')
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
    IControlCalibrationRaw(){};
    /**
     * Destructor.
     */
    virtual ~IControlCalibrationRaw() {}

    /* Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     */
    virtual bool calibrateRaw(int j, double p)=0;

    /* Check if the calibration is terminated, on a particular joint.
     * Non blocking.
     * @return true/false
     */
    virtual bool doneRaw(int j)=0;

};

/**
 *
 * New interface for control devices, calibration commands.
 */
class YARP_dev_API yarp::dev::IControlCalibration2Raw
{
public:
    IControlCalibration2Raw(){};
    /**
     * Destructor.
     */
    virtual ~IControlCalibration2Raw() {}

    /* Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     */
    virtual bool calibrate2Raw(int axis, unsigned int type, double p1, double p2, double p3)=0;

    /* Start calibration, this method is very often platform
    * specific.
    * @return true/false on success failure
    */
    virtual bool setCalibrationParametersRaw(int axis, const CalibrationParameters& params) {return false;}

    /* Check if the calibration is terminated, on a particular joint.
     * Non blocking.
     * @return true/false
     */
    virtual bool doneRaw(int j)=0;

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

    /* Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     */
    virtual bool calibrate(int j, double p)=0;

    /* Check if the calibration is terminated, on a particular joint.
     * Non blocking.
     * @return true/false
     */
    virtual bool done(int j)=0;

    /* Set the calibrator object to be used to calibrate the robot.
     * @param c pointer to the calibrator object
     * @return true/false on success failure
     */
    virtual bool setCalibrator(ICalibrator *c);

    /* Calibrate robot by using an external calibrator. The external
     * calibrator must be previously set by calling the setCalibration()
     * method.
     * @return true/false on success failure
     */
    virtual bool calibrate();

    virtual bool park(bool wait=true);

};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, calibration commands.
 */
class YARP_dev_API yarp::dev::IControlCalibration2
{
private:
    ICalibrator *calibrator;

public:
    IControlCalibration2();
    /**
     * Destructor.
     */
    virtual ~IControlCalibration2() {}

    /* Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     */
    virtual bool calibrate2(int axis, unsigned int type, double p1, double p2, double p3)=0;

    /* Start calibration, this method is very often platform
    * specific.
    * @return true/false on success failure
    */
    virtual bool setCalibrationParameters(int axis, const CalibrationParameters& params) { return false; }

    /* Check if the calibration is terminated, on a particular joint.
     * Non blocking.
     * @return true/false
     */
    virtual bool done(int j)=0;

    /* Set the calibrator object to be used to calibrate the robot.
     * @param c pointer to the calibrator object
     * @return true/false on success failure
     */
    virtual bool setCalibrator(ICalibrator *c);

    /* Calibrate robot by using an external calibrator. The external
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
 * Interface for control devices, limits commands.
 */
class YARP_dev_API yarp::dev::IControlLimits
{
public:
    /**
     * Destructor.
     */
    virtual ~IControlLimits() {}

    /* Set the software limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number (why am I telling you this)
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual bool setLimits(int axis, double min, double max)=0;

    /* Get the software limits for a particular axis.
     * @param axis joint number
     * @param pointer to store the value of the lower limit
     * @param pointer to store the value of the upper limit
     * @return true if everything goes fine, false otherwise.
     */
    virtual bool getLimits(int axis, double *min, double *max)=0;
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

    /* Set the software limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number (why am I telling you this)
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual bool setLimitsRaw(int axis, double min, double max)=0;

    /* Get the software limits for a particular axis.
     * @param axis joint number
     * @param pointer to store the value of the lower limit
     * @param pointer to store the value of the upper limit
     * @return true if everything goes fine, false otherwise.
     */
    virtual bool getLimitsRaw(int axis, double *min, double *max)=0;
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
    virtual bool getAxisName(int axis, yarp::os::ConstString& name) = 0;

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
    virtual bool getAxisNameRaw(int axis, yarp::os::ConstString& name) = 0;

    /* Get the joint type (e.g. revolute/prismatic) for a particular axis.
    * @param axis joint number
    * @param type the joint type
    * @return true if everything goes fine, false otherwise.
    */
    virtual bool getJointTypeRaw(int axis, yarp::dev::JointTypeEnum& type)  { yFatal("getJointType() not implemented on your device, cannot proceed further. Please report the problem on yarp issue tracker"); return false; };
};

/* Vocabs representing the above interfaces */

#define VOCAB_CALIBRATE_JOINT VOCAB4('c','a','l','j')
#define VOCAB_CALIBRATE_JOINT_PARAMS VOCAB4('c','l','j','p')
#define VOCAB_CALIBRATE VOCAB3('c','a','l')
#define VOCAB_ABORTCALIB VOCAB4('a','b','c','a')
#define VOCAB_ABORTPARK VOCAB4('a','b','p','a')
#define VOCAB_CALIBRATE_DONE VOCAB4('c','a','l','d')
#define VOCAB_PARK VOCAB4('p','a','r','k')
#define VOCAB_SET VOCAB3('s','e','t')
#define VOCAB_GET VOCAB3('g','e','t')
#define VOCAB_IS VOCAB2('i','s')
#define VOCAB_FAILED VOCAB4('f','a','i','l')
#define VOCAB_OK VOCAB2('o','k')

#define VOCAB_OFFSET VOCAB3('o', 'f', 'f')

// interface IPidControl sets.
#define VOCAB_PID VOCAB3('p','i','d')
#define VOCAB_PIDS VOCAB4('p','i','d','s')
#define VOCAB_REF VOCAB3('r','e','f')
#define VOCAB_REFS VOCAB4('r','e','f','s')
#define VOCAB_REFG VOCAB4('r','e','f','g')
#define VOCAB_LIM VOCAB3('l','i','m')
#define VOCAB_LIMS VOCAB4('l','i','m','s')
#define VOCAB_RESET VOCAB3('r','e','s')
#define VOCAB_DISABLE VOCAB3('d','i','s')
#define VOCAB_ENABLE VOCAB3('e','n','a')

// interface IPidControl gets.
#define VOCAB_ERR VOCAB3('e','r','r')
#define VOCAB_ERRS VOCAB4('e','r','r','s')
#define VOCAB_OUTPUT VOCAB3('o','u','t')
#define VOCAB_OUTPUTS VOCAB4('o','u','t','s')
#define VOCAB_REFERENCE VOCAB3('r','e','f')
#define VOCAB_REFERENCES VOCAB4('r','e','f','s')

// interface IPositionControl gets
#define VOCAB_AXES VOCAB4('a','x','e','s')
#define VOCAB_MOTION_DONE VOCAB3('d','o','n')
#define VOCAB_MOTION_DONES VOCAB4('d','o','n','s')

// interface IPositionControl sets
#define VOCAB_POSITION_MODE VOCAB4('p','o','s','d')
#define VOCAB_POSITION_MOVE VOCAB3('p','o','s')
#define VOCAB_POSITION_MOVES VOCAB4('p','o','s','s')
#define VOCAB_RELATIVE_MOVE VOCAB3('r','e','l')

#define VOCAB_RELATIVE_MOVES VOCAB4('r','e','l','s')
#define VOCAB_REF_SPEED VOCAB3('v','e','l')
#define VOCAB_REF_SPEEDS VOCAB4('v','e','l','s')
#define VOCAB_REF_ACCELERATION VOCAB3('a','c','c')
#define VOCAB_REF_ACCELERATIONS VOCAB4('a','c','c','s')
#define VOCAB_STOP VOCAB3('s','t','o')
#define VOCAB_STOPS VOCAB4('s','t','o','s')

// interface IVelocityControl sets
#define VOCAB_VELOCITY_MODE VOCAB4('v','e','l','d')
#define VOCAB_VELOCITY_MOVE VOCAB3('v','m','o')
#define VOCAB_VELOCITY_MOVES VOCAB4('v','m','o','s')

// interface IAmplifierControl sets/gets
#define VOCAB_AMP_ENABLE            VOCAB3('a','e','n')
#define VOCAB_AMP_DISABLE           VOCAB3('a','d','i')
#define VOCAB_AMP_STATUS            VOCAB4('a','s','t','a')
#define VOCAB_AMP_STATUS_SINGLE     VOCAB4('a','s','t','s')
#define VOCAB_AMP_CURRENT           VOCAB3('a','c','u')
#define VOCAB_AMP_CURRENTS          VOCAB4('a','c','u','s')
#define VOCAB_AMP_MAXCURRENT        VOCAB4('m','a','x','c')
#define VOCAB_AMP_NOMINAL_CURRENT   VOCAB4('a','c','n','o')
#define VOCAB_AMP_PEAK_CURRENT      VOCAB4('a','c','p','k')
#define VOCAB_AMP_PWM               VOCAB3('p','w','m')
#define VOCAB_AMP_PWM_LIMIT         VOCAB4('p','w','m','l')
#define VOCAB_AMP_VOLTAGE_SUPPLY    VOCAB4('a','v','s','u')

// interface IControlLimits sets/gets
#define VOCAB_LIMITS VOCAB4('l','l','i','m')

// interface IAxisInfo
#define VOCAB_INFO_NAME VOCAB4('n','a','m','e')
#define VOCAB_INFO_TYPE VOCAB4('t','y','p','e')

#define VOCAB_TIMESTAMP VOCAB4('t','s','t','a')
#define VOCAB_TORQUE VOCAB4('t','o','r','q')
#define VOCAB_TORQUE_MODE VOCAB4('t','r','q','d')
#define VOCAB_TRQS VOCAB4('t','r','q','s')
#define VOCAB_TRQ  VOCAB3('t','r','q')
#define VOCAB_BEMF VOCAB3('b','m','f')
#define VOCAB_MOTOR_PARAMS  VOCAB4('m','t','p','s')
#define VOCAB_RANGES VOCAB4('r','n','g','s')
#define VOCAB_RANGE  VOCAB3('r','n','g')
#define VOCAB_IMP_PARAM   VOCAB3('i','p','r')
#define VOCAB_IMP_OFFSET  VOCAB3('i','o','f')

#define VOCAB_TORQUES_DIRECTS VOCAB4('d','t','q','s') //This implements the setRefTorques for the whole part
#define VOCAB_TORQUES_DIRECT VOCAB3('d','t','q') //This implements the setRefTorque for a single joint
#define VOCAB_TORQUES_DIRECT_GROUP VOCAB4('d','t','q','g') //This implements the setRefTorques with joint list

// protocol version
#define VOCAB_PROTOCOL_VERSION VOCAB('p', 'r', 'o', 't')

#endif // YARP_DEV_CONTROLBOARDINTERFACES_H
