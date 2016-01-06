// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2008, 2009 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPOPENLOOP
#define YARPOPENLOOP

#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
        class IOpenLoopControlRaw;
        class IOpenLoopControl;
    }
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for controlling the output of a motor control device.
 */
class yarp::dev::IOpenLoopControlRaw
{
public:
    virtual ~IOpenLoopControlRaw(){}

    /**
     * Command direct output value to joint j.
     */
    virtual bool setRefOutputRaw(int j, double v)=0;

    /**
     * Command direct output value to all joints.
     */
    virtual bool setRefOutputsRaw(const double *v)=0;

    /**
     * Get the last reference sent using the setRefOutput function
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getRefOutputRaw(int j, double *v)=0;

    /**
     * Get the last reference sent using the setRefOutputs function
     * @param outs pinter to the vector that will store the output values
     * @return true/false on success/failure
     */
    virtual bool getRefOutputsRaw(double *v)=0;

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getOutputRaw(int j, double *v)=0;

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     */
    virtual bool getOutputsRaw(double *v)=0;

    /**
     * Enable open loop mode.
     */
    virtual bool setOpenLoopModeRaw()=0;
};


/**
 * @ingroup dev_iface_motor
 *
 * Interface for controlling the output of a motor control device.
 */
class YARP_dev_API yarp::dev::IOpenLoopControl
{
public:
    virtual ~IOpenLoopControl(){}

    /**
     * Command direct output value to joint j.
     */
    virtual bool setRefOutput(int j, double v)=0;

    /**
     * Command direct output value to all joints.
     */
    virtual bool setRefOutputs(const double *v)=0;

    /**
     * Get the last reference sent using the setRefOutput function
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getRefOutput(int j, double *v)=0;

    /**
     * Get the last reference sent using the setRefOutputs function
     * @param outs pinter to the vector that will store the output values
     * @return true/false on success/failure
     */
    virtual bool getRefOutputs(double *v)=0;

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getOutput(int j, double *v)=0;

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     */
    virtual bool getOutputs(double *v)=0;

    /**
     * Enable open loop mode.
     */
    virtual bool setOpenLoopMode()=0;
};

// all sets in streaming

// Interface name
#define VOCAB_OPENLOOP_INTERFACE VOCAB4('i','o','p','l')
// methods names
#define VOCAB_OPENLOOP_REF_OUTPUT  VOCAB3('r','e','f')
#define VOCAB_OPENLOOP_REF_OUTPUTS VOCAB4('r','e','f','s')
#define VOCAB_OPENLOOP_PWM_OUTPUT  VOCAB3('p','w','m')
#define VOCAB_OPENLOOP_PWM_OUTPUTS VOCAB4('p','w','m','s')
#endif
