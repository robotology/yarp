// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef ICONTROL_LIMIT2_IMPL_H_
#define ICONTROL_LIMIT2_IMPL_H_


#include <yarp/dev/IControlLimits2.h>
#include <yarp/dev/api.h>
#include <yarp/os/Log.h>

namespace yarp {
    namespace dev {
        class ImplementControlLimits2;
        class StubImplControlLimits2Raw;
    }
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, commands to get/set position and veloity limits
 */
class YARP_dev_API yarp::dev::ImplementControlLimits2: public IControlLimits2
{
protected:
    IControlLimits2Raw *iLimits2;
    void    *helper;
    // those data are used as a support, DO NOT RELY on them!
    int     *temp_int;
    double  *temp_max;
    double  *temp_min;

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
    ImplementControlLimits2(yarp::dev::IControlLimits2Raw *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementControlLimits2();
    virtual bool setLimits(int axis, double min, double max);
    virtual bool getLimits(int axis, double *min, double *max);
    virtual bool setVelLimits(int axis, double min, double max);
    virtual bool getVelLimits(int axis, double *min, double *max);
};


/**
 * Stub implementation of IControlLimit2Raw interface.
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
class yarp::dev::StubImplControlLimits2Raw: public yarp::dev::IControlLimits2Raw
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
    virtual ~StubImplControlLimits2Raw() {};

    virtual bool setLimitsRaw(int axis, double min, double max)
    {
        return NOT_YET_IMPLEMENTED("setLimitsRaw");
    }

    virtual bool getLimitsRaw(int axis, double *min, double *max)
    {
        return NOT_YET_IMPLEMENTED("getLimitsRaw");
    }

    virtual bool setVelLimitsRaw(int axis, double min, double max)
    {
        return NOT_YET_IMPLEMENTED("setVelLimitsRaw");
    }

    virtual bool getVelLimitsRaw(int axis, double *min, double *max)
    {
        return NOT_YET_IMPLEMENTED("getVelLimitsRaw");
    }

};

#endif /* ICONTROL_LIMIT2_IMPL_H_ */


