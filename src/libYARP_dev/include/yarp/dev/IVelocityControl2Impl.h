/*
 * Copyright: 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef IVELOCITYCONTROLIMPL_V2_H_
#define IVELOCITYCONTROLIMPL_V2_H_


#include <yarp/dev/IVelocityControl2.h>
#include <yarp/os/Log.h>

namespace yarp {
    namespace dev {
        class ImplementVelocityControl2;
        class StubImplVelocityControl2Raw;
    }
}

#if defined(_MSC_VER) && !defined(YARP_NO_DEPRECATED) // since YARP 2.3.65
// A class implementing setVelocityMode() causes a warning on MSVC
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
#endif && !defined(YARP_NO_DEPRECATED)

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

    virtual bool getAxes(int *axes);
    virtual bool velocityMove(int j, double sp);
    virtual bool velocityMove(const double *sp);
    virtual bool setRefAcceleration(int j, double acc);
    virtual bool setRefAccelerations(const double *accs);
    virtual bool getRefAcceleration(int j, double *acc);
    virtual bool getRefAccelerations(double *accs);
    virtual bool stop(int j);
    virtual bool stop();


    // specific of IVelocityControl2
    virtual bool velocityMove(const int n_joint, const int *joints, const double *spds);
    virtual bool getRefVelocity(const int joint, double *vel);
    virtual bool getRefVelocities(double *vels);
    virtual bool getRefVelocities(const int n_joint, const int *joints, double *vels);
    virtual bool setRefAccelerations(const int n_joint, const int *joints, const double *accs);
    virtual bool getRefAccelerations(const int n_joint, const int *joints, double *accs);
    virtual bool stop(const int n_joint, const int *joints);
    virtual bool setVelPid(int j, const Pid &pid);
    virtual bool setVelPids(const Pid *pids);
    virtual bool getVelPid(int j, Pid *pid);
    virtual bool getVelPids(Pid *pids);

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.65
    YARP_DEPRECATED virtual bool setVelocityMode();
#endif // YARP_NO_DEPRECATED
};

#if defined(_MSC_VER) && !defined(YARP_NO_DEPRECATED) // since YARP 2.3.65
YARP_WARNING_POP
#endif

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
    virtual bool getAxes(int *axes)
    {return NOT_YET_IMPLEMENTED("getAxesRaw");}

    virtual bool velocityMoveRaw(int j, double sp)
    {return NOT_YET_IMPLEMENTED("velocityMoveRaw");}

    virtual bool velocityMoveRaw(const double *sp)
    {return NOT_YET_IMPLEMENTED("velocityMoveRaw");}

    virtual bool setRefAccelerationRaw(int j, double acc)
    {return NOT_YET_IMPLEMENTED("setRefAccelerationRaw");}

    virtual bool setRefAccelerationsRaw(const double *accs)
    {return NOT_YET_IMPLEMENTED("setRefAccelerationsRaw");}

    virtual bool getRefAccelerationRaw(int j, double *acc)
    {return NOT_YET_IMPLEMENTED("getRefAccelerationRaw");}

    virtual bool getRefAccelerationsRaw(double *accs)
    {return NOT_YET_IMPLEMENTED("getRefAccelerationsRaw");}

    virtual bool stopRaw(int j)
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    virtual bool stopRaw()
    {return NOT_YET_IMPLEMENTED("stopRaw");}


    // specific of IVelocityControl2
    virtual bool velocityMoveRaw(const int n_joint, const int *joints, const double *spds)
    {return NOT_YET_IMPLEMENTED("velocityMoveRaw");}

    virtual bool getRefVelocityRaw(const int joint, double *vel)
    {return NOT_YET_IMPLEMENTED("getRefVelocityRaw");}

    virtual bool getRefVelocitiesRaw(double *vels)
    {return NOT_YET_IMPLEMENTED("getRefVelocitiesRaw");}

    virtual bool getRefVelocitiesRaw(const int n_joint, const int *joints, double *vels)
    {return NOT_YET_IMPLEMENTED("getRefVelocitiesRaw");}

    virtual bool setRefAccelerationsRaw(const int n_joint, const int *joints, const double *accs)
    {return NOT_YET_IMPLEMENTED("setRefAccelerationsRaw");}

    virtual bool getRefAccelerationsRaw(const int n_joint, const int *joints, double *accs)
    {return NOT_YET_IMPLEMENTED("getRefAccelerationsRaw");}

    virtual bool stopRaw(const int n_joint, const int *joints)
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    virtual bool setVelPidRaw(int j, const Pid &pid)
    {return NOT_YET_IMPLEMENTED("setVelPidRaw");}

    virtual bool setVelPidsRaw(const Pid *pids)
    {return NOT_YET_IMPLEMENTED("setVelPidsRaw");}

    virtual bool getVelPidRaw(int j, Pid *pid)
    {return NOT_YET_IMPLEMENTED("getVelPidRaw");}

    virtual bool getVelPidsRaw(Pid *pids)
    {return NOT_YET_IMPLEMENTED("getVelPidsRaw");}

};
#endif /* IVELOCITYCONTROLIMPL_V2_H_ */
