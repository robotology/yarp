/*
 * Copyright: (C) 2013 Istituto Italiano di Tecnologia (IIT)
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_DEV_IPOSITIONCONTROL2IMPL_H
#define YARP_DEV_IPOSITIONCONTROL2IMPL_H


#include <yarp/dev/IPositionControl2.h>

namespace yarp{
    namespace dev {
        class ImplementPositionControl2;
        class StubImplPositionControl2Raw;
    }
}


/**
 * Default implementation of the IPositionControl2 interface. This class can
 * be used to easily provide an implementation of IPositionControl2.
 *
 */
class YARP_dev_API yarp::dev::ImplementPositionControl2 : public IPositionControl2
{
protected:
    IPositionControl2Raw *iPosition2;
    void    *helper;
    int     *temp_int;
    double  *temp_double;
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
    ImplementPositionControl2(yarp::dev::IPositionControl2Raw *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementPositionControl2();


    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     */
    virtual bool getAxes(int *axis) override;

    virtual bool positionMove(int j, double ref) override;
    virtual bool positionMove(const int n_joint, const int *joints, const double *refs) override;
    virtual bool positionMove(const double *refs) override;
    virtual bool relativeMove(int j, double delta) override;
    virtual bool relativeMove(const int n_joint, const int *joints, const double *deltas) override;
    virtual bool relativeMove(const double *deltas) override;
    virtual bool checkMotionDone(bool *flag) override;
    virtual bool checkMotionDone(const int n_joint, const int *joints, bool *flags) override;
    virtual bool checkMotionDone(int j, bool *flag) override;
    virtual bool setRefSpeed(int j, double sp) override;
    virtual bool setRefSpeeds(const int n_joint, const int *joints, const double *spds) override;
    virtual bool setRefSpeeds(const double *spds) override;
    virtual bool setRefAcceleration(int j, double acc) override;
    virtual bool setRefAccelerations(const int n_joint, const int *joints, const double *accs) override;
    virtual bool setRefAccelerations(const double *accs) override;
    virtual bool getRefSpeed(int j, double *ref) override;
    virtual bool getRefSpeeds(const int n_joint, const int *joints, double *spds) override;
    virtual bool getRefSpeeds(double *spds) override;
    virtual bool getRefAcceleration(int j, double *acc) override;
    virtual bool getRefAccelerations(const int n_joint, const int *joints, double *accs) override;
    virtual bool getRefAccelerations(double *accs) override;
    virtual bool stop(int j) override;
    virtual bool stop(const int n_joint, const int *joints) override;
    virtual bool stop() override;
    virtual bool getTargetPosition(const int joint, double *ref) override;
    virtual bool getTargetPositions(double *refs) override;
    virtual bool getTargetPositions(const int n_joint, const int *joints, double *refs) override;
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
class yarp::dev::StubImplPositionControl2Raw: public IPositionControl2Raw
{
private:
    /**
     * Helper for printing error message, see below.
     * Implemented in ControlBoardInterfacesImpl.cpp.
     */
    bool NOT_YET_IMPLEMENTED(const char *func=0);

public:
    virtual ~StubImplPositionControl2Raw(){}

    virtual bool getAxes(int *ax) override
    {return NOT_YET_IMPLEMENTED("getAxes");}

    virtual bool positionMoveRaw(int j, double ref) override
    {return NOT_YET_IMPLEMENTED("positionMoveRaw");}

    virtual bool positionMoveRaw(const int n_joint, const int *joints, const double *refs) override
    {return NOT_YET_IMPLEMENTED("positionMoveRaw");}

    virtual bool positionMoveRaw(const double *refs) override
    {return NOT_YET_IMPLEMENTED("positionMoveRaw");}

    virtual bool relativeMoveRaw(int j, double delta) override
    {return NOT_YET_IMPLEMENTED("relativeMoveRaw");}

    virtual bool relativeMoveRaw(const int n_joint, const int *joints, const double *refs) override
    {return NOT_YET_IMPLEMENTED("relativeMoveRaw");}

    virtual bool relativeMoveRaw(const double *deltas) override
    {return NOT_YET_IMPLEMENTED("relativeMoveRaw");}

    virtual bool checkMotionDoneRaw(int j, bool *flag) override
    {return NOT_YET_IMPLEMENTED("checkMotionDoneRaw");}

    virtual bool checkMotionDoneRaw(const int n_joint, const int *joints, bool *flags) override
    {return NOT_YET_IMPLEMENTED("checkMotionDoneRaw");}

    virtual bool checkMotionDoneRaw(bool *flag) override
    {return NOT_YET_IMPLEMENTED("checkMotionDoneRaw");}

    virtual bool setRefSpeedRaw(int j, double sp) override
    {return NOT_YET_IMPLEMENTED("setRefSpeedRaw");}

    virtual bool setRefSpeedsRaw(const int n_joint, const int *joints, const double *spds) override
    {return NOT_YET_IMPLEMENTED("setRefSpeedsRaw");}

    virtual bool setRefSpeedsRaw(const double *spds) override
    {return NOT_YET_IMPLEMENTED("setRefSpeedsRaw");}

    virtual bool setRefAccelerationRaw(int j, double acc) override
    {return NOT_YET_IMPLEMENTED("setRefAccelerationRaw");}

    virtual bool setRefAccelerationsRaw(const int n_joint, const int *joints, const double *accs) override
    {return NOT_YET_IMPLEMENTED("setRefAccelerationsRaw");}

    virtual bool setRefAccelerationsRaw(const double *accs) override
    {return NOT_YET_IMPLEMENTED("setRefAccelerationsRaw");}

    virtual bool getRefSpeedRaw(int j, double *ref) override
    {return NOT_YET_IMPLEMENTED("getRefSpeedRaw");}

    virtual bool getRefSpeedsRaw(const int n_joint, const int *joints, double *spds) override
    {return NOT_YET_IMPLEMENTED("getRefSpeedsRaw");}

    virtual bool getRefSpeedsRaw(double *spds) override
    {return NOT_YET_IMPLEMENTED("getRefSpeedsRaw");}

    virtual bool getRefAccelerationRaw(int j, double *acc) override
    {return NOT_YET_IMPLEMENTED("getRefAccelerationRaw");}

    virtual bool getRefAccelerationsRaw(const int n_joint, const int *joints, double *accs) override
    {return NOT_YET_IMPLEMENTED("getRefAccelerationsRaw");}

    virtual bool getRefAccelerationsRaw(double *accs) override
    {return NOT_YET_IMPLEMENTED("getRefAccelerationsRaw");}

    virtual bool stopRaw(int j) override
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    virtual bool stopRaw(const int n_joint, const int *joints) override
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    virtual bool stopRaw() override
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    virtual bool getTargetPositionRaw(const int joint, double *ref) override
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    virtual bool getTargetPositionsRaw(double *refs) override
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    virtual bool getTargetPositionsRaw(const int n_joint, const int *joints, double *refs) override
    {return NOT_YET_IMPLEMENTED("stopRaw");}
};

#endif // YARP_DEV_IPOSITIONCONTROL2IMPL_H
