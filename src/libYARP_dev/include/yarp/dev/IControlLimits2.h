/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_ICONTROLLIMITS2_H
#define YARP_DEV_ICONTROLLIMITS2_H

#include <yarp/dev/ControlBoardInterfaces.h>

namespace yarp {
    namespace dev {
        class IControlLimits2;
        class IControlLimits2Raw;
    }
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, commands to get/set position and veloity limits
 */
class YARP_dev_API yarp::dev::IControlLimits2: public yarp::dev::IControlLimits
{
public:
    /**
     * Destructor.
     */
    virtual ~IControlLimits2() {}

    using IControlLimits::setLimits;
    using IControlLimits::getLimits;

    /**
     * Set the software speed limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual bool setVelLimits(int axis, double min, double max)=0;

    /**
     * Get the software speed limits for a particular axis.
     * @param axis joint number
     * @param min pointer to store the value of the lower limit
     * @param max pointer to store the value of the upper limit
     * @return true if everything goes fine, false otherwise.
     */
    virtual bool getVelLimits(int axis, double *min, double *max)=0;
};


/**
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, commands to get/set position and veloity limits
 * in encoder coordinates
 */
class yarp::dev::IControlLimits2Raw: public yarp::dev::IControlLimitsRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IControlLimits2Raw() {}

    using IControlLimitsRaw::setLimitsRaw;
    using IControlLimitsRaw::getLimitsRaw;

    /**
     * Set the software speed limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual bool setVelLimitsRaw(int axis, double min, double max)=0;

    /**
     * Get the software speed limits for a particular axis.
     * @param axis joint number
     * @param min pointer to store the value of the lower limit
     * @param max pointer to store the value of the upper limit
     * @return true if everything goes fine, false otherwise.
     */
    virtual bool getVelLimitsRaw(int axis, double *min, double *max)=0;

};

#define VOCAB_VEL_LIMITS VOCAB4('v','l','i','m')

#endif // YARP_DEV_ICONTROLLIMITS2_H
