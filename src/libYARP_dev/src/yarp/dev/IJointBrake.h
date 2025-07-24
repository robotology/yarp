/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IJOINTBRAKE_H
#define YARP_DEV_IJOINTBRAKE_H

#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {
class IJointBrakeRaw;
class IJointBrake;
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for controlling a joint equipped with brakes (hardware or simulated).
 */
class YARP_dev_API yarp::dev::IJointBrake
{
public:
    virtual ~IJointBrake(){}

    /**
    * Check is the joint brake is currently active.
    * @param j joint number
    * @param braked is true if the joint is currently braked
    * @return: true/false success failure.
    */
    virtual yarp::dev::ReturnValue isJointBraked(int j, bool& braked) const =0;

    /**
    * Enables/Disables manually the joint brake.
    * @param j joint number
    * @param active true to enable the joint brake
    * @return: true/false success failure.
    */
    virtual yarp::dev::ReturnValue setManualBrakeActive(int j, bool active) =0;

    /**
    * Enables/Disables the automatic joint brake. If enabled, the firmware
    * can automatically choose when to enable the joint brake (for example when
    * the joint is in hardware_fault mode).
    * @param j joint number
    * @param enabled true to enable the automatic joint brake
    * @return: true/false success failure.
    */
    virtual yarp::dev::ReturnValue setAutoBrakeEnabled(int j, bool enabled) =0;

    /**
    * checks if the automatic joint brake mode is enabled or disabled.
    * @param j joint number
    * @param enabled is true the automatic joint brake mode is enabled
    * @return: true/false success failure.
    */
    virtual yarp::dev::ReturnValue getAutoBrakeEnabled(int j, bool& enabled) const =0;
};


/**
 * @ingroup dev_iface_motor_raw
 *
 * Interface for controlling a joint equipped with brakes (hardware or simulated).
 * See IJointBrake for more documentation.
 */
class YARP_dev_API yarp::dev::IJointBrakeRaw
{
public:
    virtual ~IJointBrakeRaw(){}
    virtual yarp::dev::ReturnValue isJointBrakedRaw(int j, bool& braked) const =0;
    virtual yarp::dev::ReturnValue setManualBrakeActiveRaw(int j, bool active) =0;
    virtual yarp::dev::ReturnValue setAutoBrakeEnabledRaw(int j, bool enabled) =0;
    virtual yarp::dev::ReturnValue getAutoBrakeEnabledRaw(int j, bool& enabled) const =0;
};

#endif // YARP_DEV_IJOINTBRAKE_H
