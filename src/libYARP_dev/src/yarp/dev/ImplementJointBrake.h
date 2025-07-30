/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTJOINTBRAKE_H
#define YARP_DEV_IMPLEMENTJOINTBRAKE_H

#include <yarp/dev/IJointBrake.h>
#include <yarp/dev/api.h>

namespace yarp::dev {
class ImplementJointBrake;
}

class YARP_dev_API yarp::dev::ImplementJointBrake: public IJointBrake
{
protected:
    yarp::dev::IJointBrakeRaw *raw;
    void *helper;

    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap);

    /**
     * Clean up internal data and memory.
     * @return true if uninitialization is executed, false otherwise.
     */
    bool uninitialize ();

public:
    /* Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementJointBrake(yarp::dev::IJointBrakeRaw *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementJointBrake();

    virtual yarp::dev::ReturnValue isJointBraked(int j, bool& braked) const override;
    virtual yarp::dev::ReturnValue setManualBrakeActive(int j, bool active) override;
    virtual yarp::dev::ReturnValue setAutoBrakeEnabled(int j, bool enabled) override;
    virtual yarp::dev::ReturnValue getAutoBrakeEnabled(int j, bool& enabled) const override;
};

#endif // YARP_DEV_IMPLEMENTJOINTFAULT_H
