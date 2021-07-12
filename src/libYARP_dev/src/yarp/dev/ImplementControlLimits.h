/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ICONTROLLIMITSIMPL_H
#define YARP_DEV_ICONTROLLIMITSIMPL_H


#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/api.h>
#include <yarp/os/Log.h>

namespace yarp {
namespace dev {

/**
 * @ingroup dev_iface_motor
 *        class ImplementControlLimits;
          class StubImplControlLimitsRaw;


 * Interface for control devices, commands to get/set position and veloity limits
 */
class YARP_dev_API ImplementControlLimits: public IControlLimits
{
protected:
    IControlLimitsRaw *iLimits2;
    void    *helper;

    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @param enc is an array containing the encoder to angles conversion factors.
     * @param zos is an array containing the zeros of the encoders.
     *  respect to the control/output values of the driver.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap, const double *enc, const double *zos);

    /**
     * Clean up internal data and memory.
     * @return true if uninitialization is executed, false otherwise.
     */
    bool uninitialize ();


public:
    /**
     * Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementControlLimits(yarp::dev::IControlLimitsRaw *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementControlLimits();
    bool setLimits(int axis, double min, double max) override;
    bool getLimits(int axis, double *min, double *max) override;
    bool setVelLimits(int axis, double min, double max) override;
    bool getVelLimits(int axis, double *min, double *max) override;
};


/**
 * Stub implementation of IControlLimitRaw interface.
 * Inherit from this class if you want a stub implementation
 * of methods in IControlLimit2Raw. This class allows to
 * gradually implement an interface; you just have to implement
 * functions that are useful for the underlying device.
 * Another way to see this class is as a means to convert
 * compile time errors in runtime errors.
 *
 * If you use this class please be aware that the device
 * you are wrapping might not function properly because you
 * missed to implement useful functionalities.
 *
 */
class StubImplControlLimitsRaw: public yarp::dev::IControlLimitsRaw
{
private:
    /**
     * Helper for printing error message, see below.
     * Implemented in ControlBoardInterfacesImpl.cpp.
     */
    bool NOT_YET_IMPLEMENTED(const char *func=0)
    {
        if (func)
            yError("%s: not yet implemented\n", func);
        else
            yError("Function not yet implemented\n");

        return false;
    }


public:
    /**
     * Destructor.
     */
    virtual ~StubImplControlLimitsRaw() {}

    bool setLimitsRaw(int axis, double min, double max) override
    {
        return NOT_YET_IMPLEMENTED("setLimitsRaw");
    }

    bool getLimitsRaw(int axis, double *min, double *max) override
    {
        return NOT_YET_IMPLEMENTED("getLimitsRaw");
    }

    bool setVelLimitsRaw(int axis, double min, double max) override
    {
        return NOT_YET_IMPLEMENTED("setVelLimitsRaw");
    }

    bool getVelLimitsRaw(int axis, double *min, double *max) override
    {
        return NOT_YET_IMPLEMENTED("getVelLimitsRaw");
    }

};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_ICONTROLLIMITSIMPL_H
