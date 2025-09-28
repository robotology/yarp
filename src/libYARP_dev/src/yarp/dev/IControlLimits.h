/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ICONTROLLIMITS_H
#define YARP_DEV_ICONTROLLIMITS_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>

/*! \file IControlLimits.h define control board standard interfaces*/

namespace yarp::dev {
class IControlLimitsRaw;
class IControlLimits;
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, commands to get/set position and velocity limits.
 */
class YARP_dev_API yarp::dev::IControlLimits
{
public:
    /**
     * Destructor.
     */
    virtual ~IControlLimits() {}

    /**
     * Set the software limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number (why am I telling you this)
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual yarp::dev::ReturnValue setPosLimits(int axis, double min, double max)=0;

    /**
     * Get the software limits for a particular axis.
     * @param axis joint number
     * @param pointer to store the value of the lower limit
     * @param pointer to store the value of the upper limit
     * @return true if everything goes fine, false otherwise.
     */
    virtual yarp::dev::ReturnValue getPosLimits(int axis, double *min, double *max)=0;

    /**
     * Set the software speed limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual yarp::dev::ReturnValue setVelLimits(int axis, double min, double max)=0;

    /**
     * Get the software speed limits for a particular axis.
     * @param axis joint number
     * @param min pointer to store the value of the lower limit
     * @param max pointer to store the value of the upper limit
     * @return true if everything goes fine, false otherwise.
     */
    virtual yarp::dev::ReturnValue getVelLimits(int axis, double *min, double *max)=0;
};

/**
 * @ingroup dev_iface_motor_raw
 *
 * Interface for control devices. Limits commands.
 */
class YARP_dev_API yarp::dev::IControlLimitsRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IControlLimitsRaw() {}

    /**
     *  Set the software limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number (why am I telling you this)
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual yarp::dev::ReturnValue setPosLimitsRaw(int axis, double min, double max)=0;

    /**
     * Get the software limits for a particular axis.
     * @param axis joint number
     * @param pointer to store the value of the lower limit
     * @param pointer to store the value of the upper limit
     * @return true if everything goes fine, false otherwise.
     */
    virtual yarp::dev::ReturnValue getPosLimitsRaw(int axis, double *min, double *max)=0;

    /**
     * Set the software speed limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual yarp::dev::ReturnValue setVelLimitsRaw(int axis, double min, double max)=0;

    /**
     * Get the software speed limits for a particular axis.
     * @param axis joint number
     * @param min pointer to store the value of the lower limit
     * @param max pointer to store the value of the upper limit
     * @return true if everything goes fine, false otherwise.
     */
    virtual yarp::dev::ReturnValue getVelLimitsRaw(int axis, double *min, double *max)=0;
};

#endif // YARP_DEV_ICONTROLLIMITS_H
