// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARPMOTOR__
#define __YARPMOTOR__

#include <yarp/os/Vocab.h>

namespace yarp {
    namespace dev {
        class IMotorRaw;
        class IMotor;
      }
}

/**
 * @ingroup dev_iface_motor
 *
 * Control board, encoder interface.
 */
class yarp::dev::IMotorRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IMotorRaw() {}

    /**
     * Get the number of available motors.
     * @param num retrieved number of available motors
     * @return true/false
     */
    virtual bool getNumberOfMotorsRaw(int *num) = 0;

    /**
     * Get temperature of a motor. 
     * @param m motor number
     * @param val retrieved motor temperature
     * @return true/false
     */
    virtual bool getTemperatureRaw(int m, double* val)=0;

    /**
     * Get temperature of all the motors.
     * @param vals pointer to an array containing all motor temperatures
     * @return true/false
     */
    virtual bool getTemperaturesRaw(double *vals)=0;

    /**
     * Retreives the current temperature limit for a specific motor.
     * The specific behavior of the motor when the temperature limit is exceeded depends on the implementation (power off recommeneded)
     * @param m motor number
     * @param temp the current temperature limit.
     * @return true/false
     */
    virtual bool getTemperatureLimitRaw(int m, double *temp)=0;

    /**
     * Set the temperature limit for a specific motor.
     * The specific behavior of the motor when the temperature limit is exceeded depends on the implementation (power off recommeneded)
     * @param m motor number
     * @param temp the temperature limit to be set
     * @return true/false
     */
    virtual bool setTemperatureLimitRaw(int m, const double temp)=0;

    /**
     * Get the output limit for a specific motor
     * The specific behavior of the motor when the output limit is exceeded depends on the implementation (saturation recommended)
     * 
     * @param m motor number
     * @param limit retrieved output limit
     * @return true/false
     */
    virtual bool getMotorOutputLimitRaw(int m, double *limit)=0;

    /**
     * Set the output limit for a specific motor
     * The specific behavior of the motor when the output limit is exceeded depends on the implementation (saturation recommended)
     * @param m motor number
     * @param limit output limit to be set
     * @return true/false
     */
    virtual bool setMotorOutputLimitRaw(int m, const double limit)=0;
};

/**
 * @ingroup dev_iface_motor
 *
 * Control board, encoder interface.
 */
class YARP_dev_API yarp::dev::IMotor
{
public:
    /**
     * Destructor.
     */
    virtual ~IMotor() {}

    /**
     * Get the number of available motors.
     * @param num retrieved number of available motors
     * @return true/false
     */
    virtual bool getNumberOfMotors(int *num) = 0;

    /**
     * Get temperature of a motor. 
     * @param m motor number
     * @param val retrieved motor temperature
     * @return true/false
     */
    virtual bool getTemperature(int m, double *val)=0;

    /**
     * Get temperature of all the motors.
     * @param vals pointer to an array containing all motor temperatures
     * @return true/false
     */
    virtual bool getTemperatures(double *vals)=0;

    /**
     * Retreives the current temperature limit for a specific motor.
     * The specific behavior of the motor when the temperature limit is exceeded depends on the implementation (power off recommeneded)
     * @param m motor number
     * @param temp the current temperature limit.
     * @return true/false
     */
    virtual bool getTemperatureLimit(int m, double *temp)=0;

    /**
     * Set the temperature limit for a specific motor.
     * The specific behavior of the motor when the temperature limit is exceeded depends on the implementation (power off recommeneded)
     * @param m motor number
     * @param temp the temperature limit to be set
     * @return true/false
     */
    virtual bool setTemperatureLimit(int m, const double temp)=0;

   /**
     * Get the output limit for a specific motor
     * The specific behavior of the motor when the output limit is exceeded depends on the implementation (saturation recommended)
     * 
     * @param m motor number
     * @param limit retrieved output limit
     * @return true/false
     */
    virtual bool getMotorOutputLimit(int m, double *limit)=0;

    /**
     * Set the output limit for a specific motor
     * The specific behavior of the motor when the output limit is exceeded depends on the implementation (saturation recommended)
     * @param m motor number
     * @param limit output limit to be set
     * @return true/false
     */
    virtual bool setMotorOutputLimit(int m, const double limit)=0;
};

// interface IMotorEncoders gets
#define VOCAB_MOTORS_NUMBER        VOCAB4('m','t','n','m')
#define VOCAB_TEMPERATURE          VOCAB3('t','m','p')
#define VOCAB_TEMPERATURES         VOCAB4('t','m','p','s')
#define VOCAB_TEMPERATURE_LIMIT    VOCAB4('t','m','p','l')
#define VOCAB_MOTOR_OUTPUT_LIMIT   VOCAB4('o','u','t','l')


#endif


