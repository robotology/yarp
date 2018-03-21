/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IPWMCONTROL_H
#define YARP_DEV_IPWMCONTROL_H

#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
        class IPWMControlRaw;
        class IPWMControl;
    }
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
    virtual bool getNumberOfMotors(int *number) = 0;

    /**
     * Sets the reference dutycycle to a single motor.
     * @param m motor number
     * @param ref the dutycycle, expressed as percentage (-100% +100%)
     * @return true/false on success/failure
     */
    virtual bool setRefDutyCycle(int m, double ref) = 0;

    /**
     * Sets the reference dutycycle for all the motors.
     * @param refs the dutycycle, expressed as percentage (-100% +100%)
     * @return true/false on success/failure
     */
    virtual bool setRefDutyCycles(const double *refs) = 0;

    /**
     * Gets the last reference sent using the setRefDutyCycle function.
     * @param m motor number
     * @param ref pointer to storage for return value, expressed as percentage (-100% +100%)
     * @return true/false on success/failure
     */
    virtual bool getRefDutyCycle(int m, double *ref) = 0;

    /**
     * Gets the last reference sent using the setRefDutyCycles function.
     * @param refs pointer to the vector that will store the values, expressed as percentage (-100% +100%)
     * @return true/false on success/failure
     */
    virtual bool getRefDutyCycles(double *refs) = 0;

    /** Gets the current dutycycle of the output of the amplifier (i.e. pwm value sent to the motor)
    * @param m motor number
    * @param val pointer to storage for return value, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual bool getDutyCycle(int m, double *val) = 0;

    /** Gets the current dutycycle of the output of the amplifier (i.e. pwm values sent to all motors)
    * @param vals pointer to the vector that will store the values, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual bool getDutyCycles(double *vals) = 0;
};

/**
*
* Interface for controlling an axis, by sending directly a PWM reference signal to a motor.
*/
class yarp::dev::IPWMControlRaw
{
public:
    virtual ~IPWMControlRaw(){}

    /**
    * Retrieves the number of controlled motors from the current physical interface.
    * @param number returns the number of controlled motors.
    * @return true/false on success/failure
    */
    virtual bool getNumberOfMotorsRaw(int *number) = 0;

    /**
    * Sets the reference dutycycle of a single motor.
    * @param m motor number
    * @param ref the dutycycle, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual bool setRefDutyCycleRaw(int m, double ref) = 0;

    /**
    * Sets the reference dutycycle for all motors.
    * @param refs the dutycycle, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual bool setRefDutyCyclesRaw(const double *refs) = 0;

    /**
    * Gets the last reference sent using the setRefDutyCycleRaw function.
    * @param m motor number
    * @param ref pointer to storage for return value, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual bool getRefDutyCycleRaw(int m, double *ref) = 0;

    /**
    * Gets the last reference sent using the setRefDutyCyclesRaw function.
    * @param refs pointer to the vector that will store the values, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual bool getRefDutyCyclesRaw(double *refs) = 0;

    /** Gets the current dutycycle of the output of the amplifier (i.e. pwm value sent to the motor)
    * @param m motor number
    * @param val pointer to storage for return value, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual bool getDutyCycleRaw(int m, double *val) = 0;

    /** Gets the current dutycycle of the output of the amplifier (i.e. pwm values sent to all motors)
    * @param vals pointer to the vector that will store the values, expressed as percentage (-100% +100%)
    * @return true/false on success/failure
    */
    virtual bool getDutyCyclesRaw(double *vals) = 0;
};

// Interface name
#define VOCAB_PWMCONTROL_INTERFACE   VOCAB4('i','p','w','m')
// methods names
#define VOCAB_PWMCONTROL_REF_PWM     VOCAB3('r','e','f')
#define VOCAB_PWMCONTROL_REF_PWMS    VOCAB4('r','e','f','s')
#define VOCAB_PWMCONTROL_PWM_OUTPUT  VOCAB3('p','w','m')
#define VOCAB_PWMCONTROL_PWM_OUTPUTS VOCAB4('p','w','m','s')

#endif // YARP_DEV_IPWMCONTROL_H
