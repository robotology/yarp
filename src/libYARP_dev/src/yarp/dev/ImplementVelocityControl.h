/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IVELOCITYCONTROLIMPL_H
#define YARP_DEV_IVELOCITYCONTROLIMPL_H


#include <yarp/dev/IVelocityControl.h>
#include <yarp/os/Log.h>

namespace yarp {
    namespace dev {
        class ImplementVelocityControl;
        class StubImplVelocityControlRaw;
    }
}

namespace yarp {
namespace dev {
namespace impl {

template <typename T>
class FixedSizeBuffersManager;

} // namespace impl
} // namespace dev
} // namespace yarp

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

    bool getAxes(int *axes) override;
    bool velocityMove(int j, double sp) override;
    bool velocityMove(const double *sp) override;
    bool setRefAcceleration(int j, double acc) override;
    bool setRefAccelerations(const double *accs) override;
    bool getRefAcceleration(int j, double *acc) override;
    bool getRefAccelerations(double *accs) override;
    bool stop(int j) override;
    bool stop() override;


    // specific of IVelocityControl
    bool velocityMove(const int n_joint, const int *joints, const double *spds) override;
    bool getRefVelocity(const int joint, double *vel) override;
    bool getRefVelocities(double *vels) override;
    bool getRefVelocities(const int n_joint, const int *joints, double *vels) override;
    bool setRefAccelerations(const int n_joint, const int *joints, const double *accs) override;
    bool getRefAccelerations(const int n_joint, const int *joints, double *accs) override;
    bool stop(const int n_joint, const int *joints) override;
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
private:
    /**
     * Helper for printing error message, see below.
     * Implemented in ControlBoardInterfacesImpl.cpp.
     */
    bool NOT_YET_IMPLEMENTED(const char *func = 0)
    {
        if (func) {
            yError("%s: not yet implemented\n", func);
        } else {
            yError("Function not yet implemented\n");
        }

        return false;
    }

public:
    bool getAxes(int *axes) override
    {return NOT_YET_IMPLEMENTED("getAxesRaw");}

    bool velocityMoveRaw(int j, double sp) override
    {return NOT_YET_IMPLEMENTED("velocityMoveRaw");}

    bool velocityMoveRaw(const double *sp) override
    {return NOT_YET_IMPLEMENTED("velocityMoveRaw");}

    bool setRefAccelerationRaw(int j, double acc) override
    {return NOT_YET_IMPLEMENTED("setRefAccelerationRaw");}

    bool setRefAccelerationsRaw(const double *accs) override
    {return NOT_YET_IMPLEMENTED("setRefAccelerationsRaw");}

    bool getRefAccelerationRaw(int j, double *acc) override
    {return NOT_YET_IMPLEMENTED("getRefAccelerationRaw");}

    bool getRefAccelerationsRaw(double *accs) override
    {return NOT_YET_IMPLEMENTED("getRefAccelerationsRaw");}

    bool stopRaw(int j) override
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    bool stopRaw() override
    {return NOT_YET_IMPLEMENTED("stopRaw");}


    // specific of IVelocityControl
    bool velocityMoveRaw(const int n_joint, const int *joints, const double *spds) override
    {return NOT_YET_IMPLEMENTED("velocityMoveRaw");}

    bool getRefVelocityRaw(const int joint, double *vel) override
    {return NOT_YET_IMPLEMENTED("getRefVelocityRaw");}

    bool getRefVelocitiesRaw(double *vels) override
    {return NOT_YET_IMPLEMENTED("getRefVelocitiesRaw");}

    bool getRefVelocitiesRaw(const int n_joint, const int *joints, double *vels) override
    {return NOT_YET_IMPLEMENTED("getRefVelocitiesRaw");}

    bool setRefAccelerationsRaw(const int n_joint, const int *joints, const double *accs) override
    {return NOT_YET_IMPLEMENTED("setRefAccelerationsRaw");}

    bool getRefAccelerationsRaw(const int n_joint, const int *joints, double *accs) override
    {return NOT_YET_IMPLEMENTED("getRefAccelerationsRaw");}

    bool stopRaw(const int n_joint, const int *joints) override
    {return NOT_YET_IMPLEMENTED("stopRaw");}

};
#endif // YARP_DEV_IVELOCITYCONTROLIMPL_H
