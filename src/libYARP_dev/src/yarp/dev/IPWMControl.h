/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IPWMCONTROL_H
#define YARP_DEV_IPWMCONTROL_H

#include <yarp/dev/api.h>
#include <yarp/os/Vocab.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {
class IPWMControlRaw;
class IPWMControl;
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for controlling an axis, by sending directly a PWM reference signal to a motor.
 */
class YARP_dev_API yarp::dev::IPWMControl
{
public:
    virtual ~IPWMControl(){}

    /**
    * Retrieves the number of controlled motors from the current physical interface.
    * @param number returns the number of controlled motors.
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue getNumberOfMotors(int *number) = 0;

    /**
     * Sets the reference dutycycle to a single motor.
     * @param m motor number
     * @param ref the dutycycle, expressed as percentage (-100% +100%)
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue setRefDutyCycle(int m, double ref) = 0;

    /**
     * Sets the reference dutycycle for all the motors.
     * @param refs the dutycycle, expressed as percentage (-100% +100%)
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue setRefDutyCycles(const double *refs) = 0;

    /**
     * Gets the last reference sent using the setRefDutyCycle function.
     * @param m motor number
     * @param ref pointer to storage for return value, expressed as percentage (-100% +100%)
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getRefDutyCycle(int m, double *ref) = 0;

    /**
     * Gets the last reference sent using the setRefDutyCycles function.
     * @param refs pointer to the vector that will store the values, expressed as percentage (-100% +100%)
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getRefDutyCycles(double *refs) = 0;

    /** Gets the current dutycycle of the output of the amplifier (i.e. pwm value sent to the motor)
    * @param m motor number
    * @param val pointer to storage for return value, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue getDutyCycle(int m, double *val) = 0;

    /** Gets the current dutycycle of the output of the amplifier (i.e. pwm values sent to all motors)
    * @param vals pointer to the vector that will store the values, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue getDutyCycles(double *vals) = 0;
};

/**
 * @ingroup dev_iface_motor_raw
 *
 * Interface for controlling an axis, by sending directly a PWM reference signal to a motor.
 */
class YARP_dev_API yarp::dev::IPWMControlRaw
{
public:
    virtual ~IPWMControlRaw(){}

    /**
    * Retrieves the number of controlled motors from the current physical interface.
    * @param number returns the number of controlled motors.
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue getNumberOfMotorsRaw(int *number) = 0;

    /**
    * Sets the reference dutycycle of a single motor.
    * @param m motor number
    * @param ref the dutycycle, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue setRefDutyCycleRaw(int m, double ref) = 0;

    /**
    * Sets the reference dutycycle for all motors.
    * @param refs the dutycycle, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue setRefDutyCyclesRaw(const double *refs) = 0;

    /**
    * Gets the last reference sent using the setRefDutyCycleRaw function.
    * @param m motor number
    * @param ref pointer to storage for return value, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue getRefDutyCycleRaw(int m, double *ref) = 0;

    /**
    * Gets the last reference sent using the setRefDutyCyclesRaw function.
    * @param refs pointer to the vector that will store the values, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue getRefDutyCyclesRaw(double *refs) = 0;

    /** Gets the current dutycycle of the output of the amplifier (i.e. pwm value sent to the motor)
    * @param m motor number
    * @param val pointer to storage for return value, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue getDutyCycleRaw(int m, double *val) = 0;

    /** Gets the current dutycycle of the output of the amplifier (i.e. pwm values sent to all motors)
    * @param vals pointer to the vector that will store the values, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual yarp::dev::ReturnValue getDutyCyclesRaw(double *vals) = 0;
};

// Interface name
constexpr yarp::conf::vocab32_t VOCAB_PWMCONTROL_INTERFACE   = yarp::os::createVocab32('i','p','w','m');
// methods names
constexpr yarp::conf::vocab32_t VOCAB_PWMCONTROL_REF_PWM     = yarp::os::createVocab32('r','e','f');
constexpr yarp::conf::vocab32_t VOCAB_PWMCONTROL_REF_PWMS    = yarp::os::createVocab32('r','e','f','s');
constexpr yarp::conf::vocab32_t VOCAB_PWMCONTROL_PWM_OUTPUT  = yarp::os::createVocab32('p','w','m');
constexpr yarp::conf::vocab32_t VOCAB_PWMCONTROL_PWM_OUTPUTS = yarp::os::createVocab32('p','w','m','s');

#endif // YARP_DEV_IPWMCONTROL_H
