// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Robotics Brain and Cognitive Sciences Department, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo and Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARP_IMPEDANCE_CONTROLINTERFACE__
#define __YARP_IMPEDANCE_CONTROLINTERFACE__

#include <yarp/dev/api.h>

namespace yarp{
    namespace dev {
        class IImpedanceControlRaw;
        class IImpedanceControl;
    }
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control boards implementing impedance control.
 */
class yarp::dev::IImpedanceControlRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IImpedanceControlRaw() {}

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     */
    virtual bool getAxes(int *ax) = 0;

    /** Get current impedance parameters (stiffness,damping,offset) for a specific joint.
     * @return success/failure
     */
    virtual bool getImpedanceRaw(int j, double *stiffness, double *damping)=0;

    /** Set current impedance parameters (stiffness,damping) for a specific joint.
     * @return success/failure
     */
    virtual bool setImpedanceRaw(int j, double stiffness, double damping)=0;

    /** Set current force Offset for a specific joint.
     * @return success/failure
     */
    virtual bool setImpedanceOffsetRaw(int j, double offset)=0;

    /** Get current force Offset for a specific joint.
     * @return success/failure
     */
    virtual bool getImpedanceOffsetRaw(int j, double* offset)=0;

    /** Get the current impedandance limits for a specific joint.
     * @return success/failure
     */
    virtual bool getCurrentImpedanceLimitRaw(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp)=0;
};


/**
 * @ingroup dev_iface_motor
 *
 * Interface for control boards implementing impedance control.
 */
class YARP_dev_API yarp::dev::IImpedanceControl
{
public:
    /**
     * Destructor.
     */
    virtual ~IImpedanceControl() {}

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     */
    virtual bool getAxes(int *ax) = 0;

    /** Get current impedance gains (stiffness,damping,offset) for a specific joint.
     * @return success/failure
     */
    virtual bool getImpedance(int j, double *stiffness, double *damping)=0;

    /** Set current impedance gains (stiffness,damping) for a specific joint.
     * @return success/failure
     */
    virtual bool setImpedance(int j, double stiffness, double damping)=0;

    /** Set current force Offset for a specific joint.
     * @return success/failure
     */
    virtual bool setImpedanceOffset(int j, double offset)=0;

    /** Get current force Offset for a specific joint.
     * @return success/failure
     */
    virtual bool getImpedanceOffset(int j, double* offset)=0;

    /** Get the current impedandance limits for a specific joint.
     * @return success/failure
     */
    virtual bool getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp)=0;

};

//interface
#define VOCAB_IMPEDANCE VOCAB4('i','i','m','p')
#define VOCAB_ICONTROLMODE VOCAB4('i','c','m','d')
#define VOCAB_POSITION VOCAB3('p','o','s')
#define VOCAB_VELOCITY VOCAB3('v','e','l')
#define VOCAB_IOPENLOOP VOCAB3('o','l','o')

#endif
