/*
* Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
* Authors: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
 * Interface for controlling the output of a motor control device.
 */
class yarp::dev::IPWMControlRaw
{
public:
    virtual ~IPWMControlRaw(){}

    /**
     * Command direct output value to joint j.
     */
    virtual bool setRefDutyCycleRaw(int j, double v) = 0;

    /**
     * Command direct output value to all joints.
     */
    virtual bool setRefDutyCyclesRaw(const double *v) = 0;

    /**
     * Get the last reference sent using the setRefOutput function
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getRefDutyCycleRaw(int j, double *v) = 0;

    /**
     * Get the last reference sent using the setRefOutputs function
     * @param outs pinter to the vector that will store the output values
     * @return true/false on success/failure
     */
    virtual bool getRefDutyCyclesRaw(double *v) = 0;

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getDutyCycleRaw(int j, double *v) = 0;

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     */
    virtual bool getDutyCyclesRaw(double *v) = 0;
};


/**
 * @ingroup dev_iface_motor
 *
 * Interface for controlling the output of a motor control device.
 */
class YARP_dev_API yarp::dev::IPWMControl
{
public:
    virtual ~IPWMControl(){}

    /**
     * Command direct output value to joint j.
     */
    virtual bool setRefDutyCycle(int j, double v) = 0;

    /**
     * Command direct output value to all joints.
     */
    virtual bool setRefDutyCycles(const double *v) = 0;

    /**
     * Get the last reference sent using the setRefOutput function
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getRefDutyCycle(int j, double *v) = 0;

    /**
     * Get the last reference sent using the setRefOutputs function
     * @param outs pinter to the vector that will store the output values
     * @return true/false on success/failure
     */
    virtual bool getRefDutyCycles(double *v) = 0;

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getDutyCycle(int j, double *v) = 0;

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     */
    virtual bool getDutyCycles(double *v) = 0;

};

// all sets in streaming

// Interface name
#define VOCAB_PWMCONTROL_INTERFACE VOCAB4('i','p','w','m')
// methods names
#define VOCAB_PWMCONTROL_REF_PWM  VOCAB3('r','e','f')
#define VOCAB_PWMCONTROL_REF_PWMS VOCAB4('r','e','f','s')
#define VOCAB_PWMCONTROL_PWM_OUTPUT  VOCAB3('p','w','m')
#define VOCAB_PWMCONTROL_PWM_OUTPUTS VOCAB4('p','w','m','s')

#endif // YARP_DEV_IPWMCONTROL_H
