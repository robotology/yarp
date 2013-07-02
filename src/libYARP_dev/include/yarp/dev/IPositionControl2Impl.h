/*
* Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
* Author:  Alberto Cardellino
* email:   alberto.cardellino@iit.it
* website: www.robotcub.org
* Permission is granted to copy, distribute, and/or modify this program
* under the terms of the GNU General Public License, version 2 or any
* later version published by the Free Software Foundation.
*
* A copy of the license can be found at
* http://www.robotcub.org/icub/license/gpl.txt
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details
*/

#ifndef IMPLEMENTPOSITIONCONTROL_V2_H_
#define IMPLEMENTPOSITIONCONTROL_V2_H_


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
class yarp::dev::ImplementPositionControl2 : public IPositionControl2
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
    virtual bool getAxes(int *axis);

    virtual bool setPositionMode();
    virtual bool positionMove(int j, double ref);
    virtual bool positionMove(const int n_joint, const int *joints, const double *refs);
    virtual bool positionMove(const double *refs);
    virtual bool relativeMove(int j, double delta);
    virtual bool relativeMove(const int n_joint, const int *joints, const double *deltas);
    virtual bool relativeMove(const double *deltas);
    virtual bool checkMotionDone(bool *flag);
    virtual bool checkMotionDone(const int n_joint, const int *joints, bool *flags);
    virtual bool checkMotionDone(int j, bool *flag);
    virtual bool setRefSpeed(int j, double sp);
    virtual bool setRefSpeeds(const int n_joint, const int *joints, const double *spds);
    virtual bool setRefSpeeds(const double *spds);
    virtual bool setRefAcceleration(int j, double acc);
    virtual bool setRefAccelerations(const int n_joint, const int *joints, const double *accs);
    virtual bool setRefAccelerations(const double *accs);
    virtual bool getRefSpeed(int j, double *ref);
    virtual bool getRefSpeeds(const int n_joint, const int *joints, double *spds);
    virtual bool getRefSpeeds(double *spds);
    virtual bool getRefAcceleration(int j, double *acc);
    virtual bool getRefAccelerations(const int n_joint, const int *joints, double *accs);
    virtual bool getRefAccelerations(double *accs);
    virtual bool stop(int j);
    virtual bool stop(const int n_joint, const int *joints);
    virtual bool stop();
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

    virtual bool getAxes(int *ax)
    {return NOT_YET_IMPLEMENTED("getAxes");}

    virtual bool setPositionModeRaw()
    {return NOT_YET_IMPLEMENTED("setPositionModeRaw");}

    virtual bool positionMoveRaw(int j, double ref)
    {return NOT_YET_IMPLEMENTED("positionMoveRaw");}

    virtual bool positionMoveRaw(const int n_joint, const int *joints, const double *refs)
    {return NOT_YET_IMPLEMENTED("positionMoveRaw");}

    virtual bool positionMoveRaw(const double *refs)
    {return NOT_YET_IMPLEMENTED("positionMoveRaw");}

    virtual bool relativeMoveRaw(int j, double delta)
    {return NOT_YET_IMPLEMENTED("relativeMoveRaw");}

    virtual bool relativeMoveRaw(const int n_joint, const int *joints, const double *refs)
    {return NOT_YET_IMPLEMENTED("relativeMoveRaw");}

    virtual bool relativeMoveRaw(const double *deltas)
    {return NOT_YET_IMPLEMENTED("relativeMoveRaw");}

    virtual bool checkMotionDoneRaw(int j, bool *flag)
    {return NOT_YET_IMPLEMENTED("checkMotionDoneRaw");}

    virtual bool checkMotionDoneRaw(const int n_joint, const int *joints, bool *flags)
    {return NOT_YET_IMPLEMENTED("checkMotionDoneRaw");}

    virtual bool checkMotionDoneRaw(bool *flag)
    {return NOT_YET_IMPLEMENTED("checkMotionDoneRaw");}

    virtual bool setRefSpeedRaw(int j, double sp)
    {return NOT_YET_IMPLEMENTED("setRefSpeedRaw");}

    virtual bool setRefSpeedsRaw(const int n_joint, const int *joints, const double *spds)
    {return NOT_YET_IMPLEMENTED("setRefSpeedsRaw");}

    virtual bool setRefSpeedsRaw(const double *spds)
    {return NOT_YET_IMPLEMENTED("setRefSpeedsRaw");}

    virtual bool setRefAccelerationRaw(int j, double acc)
    {return NOT_YET_IMPLEMENTED("setRefAccelerationRaw");}

    virtual bool setRefAccelerationsRaw(const int n_joint, const int *joints, double *accs)
    {return NOT_YET_IMPLEMENTED("setRefAccelerationsRaw");}

    virtual bool setRefAccelerationsRaw(const double *accs)
    {return NOT_YET_IMPLEMENTED("setRefAccelerationsRaw");}

    virtual bool getRefSpeedRaw(int j, double *ref)
    {return NOT_YET_IMPLEMENTED("getRefSpeedRaw");}

    virtual bool getRefSpeedsRaw(const int n_joint, const int *joints, double *spds)
    {return NOT_YET_IMPLEMENTED("getRefSpeedsRaw");}

    virtual bool getRefSpeedsRaw(double *spds)
    {return NOT_YET_IMPLEMENTED("getRefSpeedsRaw");}

    virtual bool getRefAccelerationRaw(int j, double *acc)
    {return NOT_YET_IMPLEMENTED("getRefAccelerationRaw");}

    virtual bool getRefAccelerationsRaw(const int n_joint, const int *joints, double *accs)
    {return NOT_YET_IMPLEMENTED("getRefAccelerationsRaw");}

    virtual bool getRefAccelerationsRaw(double *accs)
    {return NOT_YET_IMPLEMENTED("getRefAccelerationsRaw");}

    virtual bool stopRaw(int j)
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    virtual bool stopRaw(const int n_joint, const int *joints)
    {return NOT_YET_IMPLEMENTED("stopRaw");}

    virtual bool stopRaw()
    {return NOT_YET_IMPLEMENTED("stopRaw");}
};

#endif /* IMPLEMENTPOSITIONCONTROL2_H_ */
