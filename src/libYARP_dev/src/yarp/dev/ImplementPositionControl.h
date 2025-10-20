/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IPOSITIONCONTROLIMPL_H
#define YARP_DEV_IPOSITIONCONTROLIMPL_H


#include <yarp/dev/IPositionControl.h>

namespace yarp::dev {
class ImplementPositionControl;
class StubImplPositionControlRaw;
}

namespace yarp::dev::impl {

template <typename T>
class FixedSizeBuffersManager;

} // namespace yarp::dev::impl

/**
 * Default implementation of the IPositionControl interface. This class can
 * be used to easily provide an implementation of IPositionControl.
 *
 */
class YARP_dev_API yarp::dev::ImplementPositionControl : public IPositionControl
{
protected:
    IPositionControlRaw *iPosition;
    void    *helper;
    yarp::dev::impl::FixedSizeBuffersManager<int> *intBuffManager;
    yarp::dev::impl::FixedSizeBuffersManager<double> *doubleBuffManager;
    yarp::dev::impl::FixedSizeBuffersManager<bool> *boolBuffManager;

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
    ImplementPositionControl(yarp::dev::IPositionControlRaw *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementPositionControl();


    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     */
    yarp::dev::ReturnValue getAxes(int *axis) override;

    yarp::dev::ReturnValue positionMove(int j, double ref) override;
    yarp::dev::ReturnValue positionMove(const int n_joint, const int *joints, const double *refs) override;
    yarp::dev::ReturnValue positionMove(const double *refs) override;
    yarp::dev::ReturnValue relativeMove(int j, double delta) override;
    yarp::dev::ReturnValue relativeMove(const int n_joint, const int *joints, const double *deltas) override;
    yarp::dev::ReturnValue relativeMove(const double *deltas) override;
    yarp::dev::ReturnValue checkMotionDone(bool *flag) override;
    yarp::dev::ReturnValue checkMotionDone(const int n_joint, const int *joints, bool *flags) override;
    yarp::dev::ReturnValue checkMotionDone(int j, bool *flag) override;
    yarp::dev::ReturnValue setTrajSpeed(int j, double sp) override;
    yarp::dev::ReturnValue setTrajSpeeds(const int n_joint, const int *joints, const double *spds) override;
    yarp::dev::ReturnValue setTrajSpeeds(const double *spds) override;
    yarp::dev::ReturnValue setTrajAcceleration(int j, double acc) override;
    yarp::dev::ReturnValue setTrajAccelerations(const int n_joint, const int *joints, const double *accs) override;
    yarp::dev::ReturnValue setTrajAccelerations(const double *accs) override;
    yarp::dev::ReturnValue getTrajSpeed(int j, double *ref) override;
    yarp::dev::ReturnValue getTrajSpeeds(const int n_joint, const int *joints, double *spds) override;
    yarp::dev::ReturnValue getTrajSpeeds(double *spds) override;
    yarp::dev::ReturnValue getTrajAcceleration(int j, double *acc) override;
    yarp::dev::ReturnValue getTrajAccelerations(const int n_joint, const int *joints, double *accs) override;
    yarp::dev::ReturnValue getTrajAccelerations(double *accs) override;
    yarp::dev::ReturnValue stop(int j) override;
    yarp::dev::ReturnValue stop(const int n_joint, const int *joints) override;
    yarp::dev::ReturnValue stop() override;
    yarp::dev::ReturnValue getTargetPosition(const int joint, double *ref) override;
    yarp::dev::ReturnValue getTargetPositions(double *refs) override;
    yarp::dev::ReturnValue getTargetPositions(const int n_joint, const int *joints, double *refs) override;
};

/**
 * Stub implementation of IPositionControlRaw interface.
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
class YARP_dev_API yarp::dev::StubImplPositionControlRaw: public IPositionControlRaw
{
private:
    /**
     * Helper for printing error message, see below.
     * Implemented in ControlBoardInterfacesImpl.cpp.
     */
    bool NOT_YET_IMPLEMENTED(const char *func=0);

public:
    virtual ~StubImplPositionControlRaw(){}

    yarp::dev::ReturnValue getAxes(int *ax) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue positionMoveRaw(int j, double ref) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue positionMoveRaw(const int n_joint, const int *joints, const double *refs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue positionMoveRaw(const double *refs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue relativeMoveRaw(int j, double delta) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue relativeMoveRaw(const int n_joint, const int *joints, const double *refs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue relativeMoveRaw(const double *deltas) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue checkMotionDoneRaw(int j, bool *flag) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue checkMotionDoneRaw(const int n_joint, const int *joints, bool *flags) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue checkMotionDoneRaw(bool *flag) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue setTrajSpeedRaw(int j, double sp) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue setTrajSpeedsRaw(const int n_joint, const int *joints, const double *spds) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue setTrajSpeedsRaw(const double *spds) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue setTrajAccelerationRaw(int j, double acc) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue setTrajAccelerationsRaw(const int n_joint, const int *joints, const double *accs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue setTrajAccelerationsRaw(const double *accs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTrajSpeedRaw(int j, double *ref) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTrajSpeedsRaw(const int n_joint, const int *joints, double *spds) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTrajSpeedsRaw(double *spds) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTrajAccelerationRaw(int j, double *acc) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTrajAccelerationsRaw(const int n_joint, const int *joints, double *accs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTrajAccelerationsRaw(double *accs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue stopRaw(int j) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue stopRaw(const int n_joint, const int *joints) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue stopRaw() override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTargetPositionRaw(const int joint, double *ref) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTargetPositionsRaw(double *refs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getTargetPositionsRaw(const int n_joint, const int *joints, double *refs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}
};

#endif // YARP_DEV_IPOSITIONCONTROLIMPL_H
