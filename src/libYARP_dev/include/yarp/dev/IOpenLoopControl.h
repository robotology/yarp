// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2008, 2009 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARPOPENLOOP__
#define __YARPOPENLOOP__

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
    virtual bool setOutputRaw(int j, double v)=0;

    /**
     * Command direct output value to all joints.
     */
    virtual bool setOutputsRaw(const double *v)=0;

    /**
     * Get the controller current output values.
     */ 
    virtual bool getOutputsRaw(double *v)=0;

    /**
     * Get the controller current output for joint j.
     */
    virtual bool getOutputRaw(int j, double *v)=0;
    
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
    virtual bool setOutput(int j, double v)=0;

    /**
     * Command direct output value to all joints.
     */
    virtual bool setOutputs(const double *v)=0;

    /**
     * Get current output command for joint j.
     */
    virtual bool getOutput(int j, double *v)=0;

    /**
     * Get current output command, all joints.
     */
    virtual bool getOutputs(double *v)=0;

    /**
     * Enable open loop mode.
     */
    virtual bool setOpenLoopMode()=0;
};

#endif


