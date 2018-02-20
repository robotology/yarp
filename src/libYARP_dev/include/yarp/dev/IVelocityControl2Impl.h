/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IVELOCITYCONTROL2IMPL_H
#define YARP_DEV_IVELOCITYCONTROL2IMPL_H


#include <yarp/dev/IVelocityControl2.h>
#include <yarp/os/Log.h>

namespace yarp {
    namespace dev {
        class ImplementVelocityControl2;
        class StubImplVelocityControl2Raw;
    }
}


class YARP_dev_API yarp::dev::ImplementVelocityControl2 : public IVelocityControl2
{
protected:
    IVelocityControl2Raw *iVelocity2;
    void    *helper;
    int     *temp_int;
    double  *temp_double;
    Pid     *tempPids;

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
    bool uninitialize();

public:
    /**
     * Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementVelocityControl2(yarp::dev::IVelocityControl2Raw *y);

    /**
     * Destructor.
     */
    virtual ~ImplementVelocityControl2();

    virtual bool getAxes(int *axes) override;
    virtual bool velocityMove(int j, double sp) override;
    virtual bool velocityMove(const double *sp) override;
    virtual bool setRefAcceleration(int j, double acc) override;
    virtual bool setRefAccelerations(const double *accs) override;
    virtual bool getRefAcceleration(int j, double *acc) override;
    virtual bool getRefAccelerations(double *accs) override;
    virtual bool stop(int j) override;
    virtual bool stop() override;


    // specific of IVelocityControl2
    virtual bool velocityMove(const int n_joint, const int *joints, const double *spds) override;
    virtual bool getRefVelocity(const int joint, double *vel) override;
    virtual bool getRefVelocities(double *vels) override;
    virtual bool getRefVelocities(const int n_joint, const int *joints, double *vels) override;
    virtual bool setRefAccelerations(const int n_joint, const int *joints, const double *accs) override;
    virtual bool getRefAccelerations(const int n_joint, const int *joints, double *accs) override;
    virtual bool stop(const int n_joint, const int *joints) override;
};


/**
 * Stub implementation of IPositionControl2Raw interface.
 * Inherit from this class if you want a stub implementation
 * of methods in IPositionControlRaw. This class allows to
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
class yarp::dev::StubImplVelocityControl2Raw: public IVelocityControl2Raw
{
private:
    /**
     * Helper for printing error message, see below.
     * Implemented in ControlBoardInterfacesImpl.cpp.
     */
    bool NOT_YET_IMPLEMENTED(const char *func = 0)
    {
        if (func)
            yError("%s: not yet implemented\n", func);
        else
            yError("Function not yet implemented\n");

        return false;
    }

public:
    virtual bool getAxes(int *axes) override
    {return NOT_YET_IMPLEMENTED("getAxesRaw");}

    virtual bool velocityMoveRaw(int j, double sp) override
    {return NOT_YET_IMPLEMENTED("velocityMoveRaw");}

    virtual bool velocityMoveRaw(const double *sp) override
    {return NOT_YET_IMPLEMENTED("velocityMoveRaw");}

    virtual bool setRefAccelerationRaw(int j, double acc) override
    {return NOT_YET_IMPLEMENTED("setRefAccelerationRaw");}

    virtual bool setRefAccelerationsRaw(const double *accs) override
    {return NOT_YET_IMPLEMENTED("setRefAccelerationsRaw");}

    virtual bool getRefAccelerationRaw(int j, double *acc) override
    {return NOT_YET_IMPLEMENTED("getRefAccelerationRaw");}

    virtual bool getRefAccelerationsRaw(double *accs) override
    {return NOT_YET_IMPLEMENTED("getRefAccelerationsRaw");}

    virtual bool stopRaw(int j) override
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    virtual bool stopRaw() override
    {return NOT_YET_IMPLEMENTED("stopRaw");}


    // specific of IVelocityControl2
    virtual bool velocityMoveRaw(const int n_joint, const int *joints, const double *spds) override
    {return NOT_YET_IMPLEMENTED("velocityMoveRaw");}

    virtual bool getRefVelocityRaw(const int joint, double *vel) override
    {return NOT_YET_IMPLEMENTED("getRefVelocityRaw");}

    virtual bool getRefVelocitiesRaw(double *vels) override
    {return NOT_YET_IMPLEMENTED("getRefVelocitiesRaw");}

    virtual bool getRefVelocitiesRaw(const int n_joint, const int *joints, double *vels) override
    {return NOT_YET_IMPLEMENTED("getRefVelocitiesRaw");}

    virtual bool setRefAccelerationsRaw(const int n_joint, const int *joints, const double *accs) override
    {return NOT_YET_IMPLEMENTED("setRefAccelerationsRaw");}

    virtual bool getRefAccelerationsRaw(const int n_joint, const int *joints, double *accs) override
    {return NOT_YET_IMPLEMENTED("getRefAccelerationsRaw");}

    virtual bool stopRaw(const int n_joint, const int *joints) override
    {return NOT_YET_IMPLEMENTED("stopRaw");}

};
#endif // YARP_DEV_IVELOCITYCONTROL2IMPL_H
