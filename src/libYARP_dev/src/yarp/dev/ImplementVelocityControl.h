/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IVELOCITYCONTROLIMPL_H
#define YARP_DEV_IVELOCITYCONTROLIMPL_H


#include <yarp/dev/IVelocityControl.h>
#include <yarp/os/Log.h>

namespace yarp::dev {
class ImplementVelocityControl;
class StubImplVelocityControlRaw;
}

namespace yarp::dev::impl {

template <typename T>
class FixedSizeBuffersManager;

} // namespace yarp::dev::impl

class YARP_dev_API yarp::dev::ImplementVelocityControl : public IVelocityControl
{
protected:
    IVelocityControlRaw *iVelocity;
    void    *helper;
    yarp::dev::impl::FixedSizeBuffersManager<int> *intBuffManager;
    yarp::dev::impl::FixedSizeBuffersManager<double> *doubleBuffManager;

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
    ImplementVelocityControl(yarp::dev::IVelocityControlRaw *y);

    /**
     * Destructor.
     */
    virtual ~ImplementVelocityControl();

    yarp::dev::ReturnValue getAxes(int *axes) override;
    yarp::dev::ReturnValue velocityMove(int j, double sp) override;
    yarp::dev::ReturnValue velocityMove(const double *sp) override;
    yarp::dev::ReturnValue setTrajAcceleration(int j, double acc) override;
    yarp::dev::ReturnValue setTrajAccelerations(const double *accs) override;
    yarp::dev::ReturnValue getTrajAcceleration(int j, double *acc) override;
    yarp::dev::ReturnValue getTrajAccelerations(double *accs) override;
    yarp::dev::ReturnValue stop(int j) override;
    yarp::dev::ReturnValue stop() override;


    // specific of IVelocityControl
    yarp::dev::ReturnValue velocityMove(const int n_joint, const int *joints, const double *spds) override;
    yarp::dev::ReturnValue getTargetVelocity(const int joint, double *vel) override;
    yarp::dev::ReturnValue getTargetVelocities(double *vels) override;
    yarp::dev::ReturnValue getTargetVelocities(const int n_joint, const int *joints, double *vels) override;
    yarp::dev::ReturnValue setTrajAccelerations(const int n_joint, const int *joints, const double *accs) override;
    yarp::dev::ReturnValue getTrajAccelerations(const int n_joint, const int *joints, double *accs) override;
    yarp::dev::ReturnValue stop(const int n_joint, const int *joints) override;
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
class yarp::dev::StubImplVelocityControlRaw: public IVelocityControlRaw
{
public:
    yarp::dev::ReturnValue getAxes(int *axes) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue velocityMoveRaw(int j, double sp) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue velocityMoveRaw(const double *sp) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue setTrajAccelerationRaw(int j, double acc) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue setTrajAccelerationsRaw(const double *accs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTrajAccelerationRaw(int j, double *acc) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTrajAccelerationsRaw(double *accs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue stopRaw(int j) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue stopRaw() override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue velocityMoveRaw(const int n_joint, const int *joints, const double *spds) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTargetVelocityRaw(const int joint, double *vel) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTargetVelocitiesRaw(double *vels) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTargetVelocitiesRaw(const int n_joint, const int *joints, double *vels) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue setTrajAccelerationsRaw(const int n_joint, const int *joints, const double *accs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTrajAccelerationsRaw(const int n_joint, const int *joints, double *accs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue stopRaw(const int n_joint, const int *joints) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

};
#endif // YARP_DEV_IVELOCITYCONTROLIMPL_H
