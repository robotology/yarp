/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IAMPLIFIERCONTROL_H
#define YARP_DEV_IAMPLIFIERCONTROL_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>

/*! \file IAmplifierControl.h define control board standard interfaces*/

namespace yarp
{
    namespace dev
    {
        class IAmplifierControlRaw;
        class IAmplifierControl;
    }
}

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

    /* Get the nominal current which can be kept for an indefinite amount of time
     * without harming the motor. This value is specific for each motor and it is typically
     * found in its data-sheet. The units are Ampere.
     * This value and the peak current may be used by the firmware to configure
     * an I2T filter.
     * @param j joint number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool getNominalCurrent(int m, double *val) {return false;};

    /* Set the nominal current which can be kept for an indefinite amount of time
    * without harming the motor. This value is specific for each motor and it is typically
    * found in its data-sheet. The units are Ampere.
    * This value and the peak current may be used by the firmware to configure
    * an I2T filter.
    * @param j joint number
    * @param val storage for return value. [Ampere]
    * @return true/false success failure.
    */
    virtual bool setNominalCurrent(int m, const double val) { return false; };

    /* Get the peak current which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor data-sheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param j joint number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool getPeakCurrent(int m, double *val) {return false;};

    /* Set the peak current. This value  which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor data-sheet, units are Ampere.
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

    /* Get the PWM limit for the given motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param j joint number
     * @param val filled with PWM limit value.
     * @return true/false success failure.
     */
    virtual bool getPWMLimit(int j, double* val) {return false;};

    /* Set the PWM limit for the given motor.
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

    /* Get the nominal current which can be kept for an indefinite amount of time
     * without harming the motor. This value is specific for each motor and it is typically
     * found in its data-sheet. The units are Ampere.
     * This value and the peak current may be used by the firmware to configure
     * an I2T filter.
     * @param j joint number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool getNominalCurrentRaw(int m, double *val) {return false;};

    /* Set the nominal current which can be kept for an indefinite amount of time
    * without harming the motor. This value is specific for each motor and it is typically
    * found in its data-sheet. The units are Ampere.
    * This value and the peak current may be used by the firmware to configure
    * an I2T filter.
    * @param j joint number
    * @param val storage for return value. [Ampere]
    * @return true/false success failure.
    */
    virtual bool setNominalCurrentRaw(int m, const double val) { return false; };

    /* Get the peak current which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor data-sheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param j joint number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool getPeakCurrentRaw(int m, double *val) {return false;};

    /* Set the peak current. This value  which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor data-sheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param j joint number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool setPeakCurrentRaw(int m, const double val) {return false;};

    /* Get the current PWM value used to control the motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param j joint number
     * @param val filled with PWM value.
     * @return true/false success failure.
     */
    virtual bool getPWMRaw(int j, double* val) {return false;};

    /* Get the PWM limit for the given motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param j joint number
     * @param val filled with PWM limit value.
     * @return true/false success failure.
     */
    virtual bool getPWMLimitRaw(int j, double* val) {return false;};

    /* Set the PWM limit for the given motor.
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

// interface IAmplifierControl sets/gets
constexpr yarp::conf::vocab32_t VOCAB_AMP_ENABLE            = yarp::os::createVocab32('a','e','n');
constexpr yarp::conf::vocab32_t VOCAB_AMP_DISABLE           = yarp::os::createVocab32('a','d','i');
constexpr yarp::conf::vocab32_t VOCAB_AMP_STATUS            = yarp::os::createVocab32('a','s','t','a');
constexpr yarp::conf::vocab32_t VOCAB_AMP_STATUS_SINGLE     = yarp::os::createVocab32('a','s','t','s');
constexpr yarp::conf::vocab32_t VOCAB_AMP_CURRENT           = yarp::os::createVocab32('a','c','u');
constexpr yarp::conf::vocab32_t VOCAB_AMP_CURRENTS          = yarp::os::createVocab32('a','c','u','s');
constexpr yarp::conf::vocab32_t VOCAB_AMP_MAXCURRENT        = yarp::os::createVocab32('m','a','x','c');
constexpr yarp::conf::vocab32_t VOCAB_AMP_NOMINAL_CURRENT   = yarp::os::createVocab32('a','c','n','o');
constexpr yarp::conf::vocab32_t VOCAB_AMP_PEAK_CURRENT      = yarp::os::createVocab32('a','c','p','k');
constexpr yarp::conf::vocab32_t VOCAB_AMP_PWM               = yarp::os::createVocab32('p','w','m');
constexpr yarp::conf::vocab32_t VOCAB_AMP_PWM_LIMIT         = yarp::os::createVocab32('p','w','m','l');
constexpr yarp::conf::vocab32_t VOCAB_AMP_VOLTAGE_SUPPLY    = yarp::os::createVocab32('a','v','s','u');

#endif // YARP_DEV_IAMPLIFIERCONTROL_H
