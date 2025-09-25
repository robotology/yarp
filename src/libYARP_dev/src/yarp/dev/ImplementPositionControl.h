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
    bool getAxes(int *axis) override;

    bool positionMove(int j, double ref) override;
    bool positionMove(const int n_joint, const int *joints, const double *refs) override;
    bool positionMove(const double *refs) override;
    bool relativeMove(int j, double delta) override;
    bool relativeMove(const int n_joint, const int *joints, const double *deltas) override;
    bool relativeMove(const double *deltas) override;
    bool checkMotionDone(bool *flag) override;
    bool checkMotionDone(const int n_joint, const int *joints, bool *flags) override;
    bool checkMotionDone(int j, bool *flag) override;
    bool setTrajSpeed(int j, double sp) override;
    bool setTrajSpeeds(const int n_joint, const int *joints, const double *spds) override;
    bool setTrajSpeeds(const double *spds) override;
    bool setTrajAcceleration(int j, double acc) override;
    bool setTrajAccelerations(const int n_joint, const int *joints, const double *accs) override;
    bool setTrajAccelerations(const double *accs) override;
    bool getTrajSpeed(int j, double *ref) override;
    bool getTrajSpeeds(const int n_joint, const int *joints, double *spds) override;
    bool getTrajSpeeds(double *spds) override;
    bool getTrajAcceleration(int j, double *acc) override;
    bool getTrajAccelerations(const int n_joint, const int *joints, double *accs) override;
    bool getTrajAccelerations(double *accs) override;
    bool stop(int j) override;
    bool stop(const int n_joint, const int *joints) override;
    bool stop() override;
    bool getTargetPosition(const int joint, double *ref) override;
    bool getTargetPositions(double *refs) override;
    bool getTargetPositions(const int n_joint, const int *joints, double *refs) override;
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

    bool getAxes(int *ax) override
    {return NOT_YET_IMPLEMENTED("getAxes");}

    bool positionMoveRaw(int j, double ref) override
    {return NOT_YET_IMPLEMENTED("positionMoveRaw");}

    bool positionMoveRaw(const int n_joint, const int *joints, const double *refs) override
    {return NOT_YET_IMPLEMENTED("positionMoveRaw");}

    bool positionMoveRaw(const double *refs) override
    {return NOT_YET_IMPLEMENTED("positionMoveRaw");}

    bool relativeMoveRaw(int j, double delta) override
    {return NOT_YET_IMPLEMENTED("relativeMoveRaw");}

    bool relativeMoveRaw(const int n_joint, const int *joints, const double *refs) override
    {return NOT_YET_IMPLEMENTED("relativeMoveRaw");}

    bool relativeMoveRaw(const double *deltas) override
    {return NOT_YET_IMPLEMENTED("relativeMoveRaw");}

    bool checkMotionDoneRaw(int j, bool *flag) override
    {return NOT_YET_IMPLEMENTED("checkMotionDoneRaw");}

    bool checkMotionDoneRaw(const int n_joint, const int *joints, bool *flags) override
    {return NOT_YET_IMPLEMENTED("checkMotionDoneRaw");}

    bool checkMotionDoneRaw(bool *flag) override
    {return NOT_YET_IMPLEMENTED("checkMotionDoneRaw");}

    bool setTrajSpeedRaw(int j, double sp) override
    {return NOT_YET_IMPLEMENTED("setTrajSpeedRaw");}

    bool setTrajSpeedsRaw(const int n_joint, const int *joints, const double *spds) override
    {return NOT_YET_IMPLEMENTED("setTrajSpeedsRaw");}

    bool setTrajSpeedsRaw(const double *spds) override
    {return NOT_YET_IMPLEMENTED("setTrajSpeedsRaw");}

    bool setTrajAccelerationRaw(int j, double acc) override
    {return NOT_YET_IMPLEMENTED("setTrajAccelerationRaw");}

    bool setTrajAccelerationsRaw(const int n_joint, const int *joints, const double *accs) override
    {return NOT_YET_IMPLEMENTED("setTrajAccelerationsRaw");}

    bool setTrajAccelerationsRaw(const double *accs) override
    {return NOT_YET_IMPLEMENTED("setTrajAccelerationsRaw");}

    bool getTrajSpeedRaw(int j, double *ref) override
    {return NOT_YET_IMPLEMENTED("getTrajSpeedRaw");}

    bool getTrajSpeedsRaw(const int n_joint, const int *joints, double *spds) override
    {return NOT_YET_IMPLEMENTED("getTrajSpeedsRaw");}

    bool getTrajSpeedsRaw(double *spds) override
    {return NOT_YET_IMPLEMENTED("getTrajSpeedsRaw");}

    bool getTrajAccelerationRaw(int j, double *acc) override
    {return NOT_YET_IMPLEMENTED("getTrajAccelerationRaw");}

    bool getTrajAccelerationsRaw(const int n_joint, const int *joints, double *accs) override
    {return NOT_YET_IMPLEMENTED("getTrajAccelerationsRaw");}

    bool getTrajAccelerationsRaw(double *accs) override
    {return NOT_YET_IMPLEMENTED("getTrajAccelerationsRaw");}

    bool stopRaw(int j) override
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    bool stopRaw(const int n_joint, const int *joints) override
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    bool stopRaw() override
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    bool getTargetPositionRaw(const int joint, double *ref) override
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    bool getTargetPositionsRaw(double *refs) override
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    bool getTargetPositionsRaw(const int n_joint, const int *joints, double *refs) override
    {return NOT_YET_IMPLEMENTED("stopRaw");}
};

#endif // YARP_DEV_IPOSITIONCONTROLIMPL_H
