/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IIMPEDANCECONTROL_H
#define YARP_DEV_IIMPEDANCECONTROL_H

#include <yarp/dev/api.h>
#include <yarp/os/Vocab.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {
class IImpedanceControlRaw;
class IImpedanceControl;
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control boards implementing impedance control.
 */
class YARP_dev_API yarp::dev::IImpedanceControlRaw
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
    virtual yarp::dev::ReturnValue getAxes(int *ax) = 0;

    /** Get current impedance parameters (stiffness,damping,offset) for a specific joint.
     * @return success/failure
     */
    virtual yarp::dev::ReturnValue getImpedanceRaw(int j, double *stiffness, double *damping)=0;

    /** Set current impedance parameters (stiffness,damping) for a specific joint.
     * @return success/failure
     */
    virtual yarp::dev::ReturnValue setImpedanceRaw(int j, double stiffness, double damping)=0;

    /** Set current force Offset for a specific joint.
     * @return success/failure
     */
    virtual yarp::dev::ReturnValue setImpedanceOffsetRaw(int j, double offset)=0;

    /** Get current force Offset for a specific joint.
     * @return success/failure
     */
    virtual yarp::dev::ReturnValue getImpedanceOffsetRaw(int j, double* offset)=0;

    /** Get the current impedance limits for a specific joint.
     * @return success/failure
     */
    virtual yarp::dev::ReturnValue getCurrentImpedanceLimitRaw(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp)=0;
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
    virtual yarp::dev::ReturnValue getAxes(int *ax) = 0;

    /** Get current impedance gains (stiffness,damping,offset) for a specific joint.
     * @return success/failure
     */
    virtual yarp::dev::ReturnValue getImpedance(int j, double *stiffness, double *damping)=0;

    /** Set current impedance gains (stiffness,damping) for a specific joint.
     * @return success/failure
     */
    virtual yarp::dev::ReturnValue setImpedance(int j, double stiffness, double damping)=0;

    /** Set current force Offset for a specific joint.
     * @return success/failure
     */
    virtual yarp::dev::ReturnValue setImpedanceOffset(int j, double offset)=0;

    /** Get current force Offset for a specific joint.
     * @return success/failure
     */
    virtual yarp::dev::ReturnValue getImpedanceOffset(int j, double* offset)=0;

    /** Get the current impedance limits for a specific joint.
     * @return success/failure
     */
    virtual yarp::dev::ReturnValue getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp)=0;

};

//interface
constexpr yarp::conf::vocab32_t VOCAB_IMPEDANCE    = yarp::os::createVocab32('i','i','m','p');

constexpr yarp::conf::vocab32_t VOCAB_POSITION     = yarp::os::createVocab32('p','o','s');
constexpr yarp::conf::vocab32_t VOCAB_VELOCITY     = yarp::os::createVocab32('v','e','l');

#endif // YARP_DEV_IIMPEDANCECONTROL_H
