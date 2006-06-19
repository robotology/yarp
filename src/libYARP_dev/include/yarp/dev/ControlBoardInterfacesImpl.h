// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __CONTROLBOARDIMPL__
#define __CONTROLBOARDIMPL__

#include <yarp/dev/ControlBoardInterfaces.h>

namespace yarp{
    namespace dev {
        template <class DERIVED, class IMPLEMENT> class ImplementPositionControl;
        template <class DERIVED, class IMPLEMENT> class ImplementVelocityControl;
    }
}

/** 
* Default implementation of the IPositionControl interface. This template class can
* be used to easily provide an implementation of IPositionControl. It takes two
* arguments, the class it is derived from and the class it is implementing, typically
* IPositionControl (which should probably be removed from the template arguments).
* <IMPLEMENT> makes only explicit that the class is implementing IPositionControl and
* appears in the inheritance list of the derived class.
*/
template <class DERIVED, class IMPLEMENT> class yarp::dev::ImplementPositionControl : public IMPLEMENT
{
    protected:
        IPositionControlRaw *iPosition;
        void *helper;
        double *temp;
        
        /**
        * Initialize the internal data and alloc memory.
        * @param size is the number of controlled axes the driver deals with.
        * @param amap is a lookup table mapping axes onto physical drivers.
        * @param enc is an array containing the encoder to angles conversion factors.
        * @param zos is an array containing the zeros of the encoders.
        *  respect to the control/output values of the driver.
        * @return true if initialized succeeded, false if it wasn't executed, or assert.
        */
        bool initialize (int size, int *amap, double *enc, double *zos);
        
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
        ImplementPositionControl(DERIVED *y);
        
        /**
        * Destructor. Perform uninitialize if needed.
        */
        virtual ~ImplementPositionControl();
        
        
        /**
        * Get the number of controlled axes. This command asks the number of controlled
        * axes for the current physical interface.
        * @return the number of controlled axes.
        */
        virtual bool getAxes(int *axis);
        
        virtual bool setPositionMode();
        virtual bool positionMove(int j, double ref);
        virtual bool positionMove(const double *refs);
        virtual bool relativeMove(int j, double delta);
        virtual bool relativeMove(const double *deltas);
        virtual bool checkMotionDone(bool *flag);
        virtual bool checkMotionDone(int j, bool *flag);
        virtual bool setRefSpeed(int j, double sp);
        virtual bool setRefSpeeds(const double *spds);
        virtual bool setRefAcceleration(int j, double acc);
        virtual bool setRefAccelerations(const double *accs);
        virtual bool getRefSpeed(int j, double *ref);
        virtual bool getRefSpeeds(double *spds);
        virtual bool getRefAcceleration(int j, double *acc);
        virtual bool getRefAccelerations(double *accs);
        virtual bool stop(int j);
        virtual bool stop();
};  

/** 
* Default implementation of the IVelocityControl interface. This template class can
* be used to easily provide an implementation of IVelocityControl. It takes two
* arguments, the class it is derived from and the class it is implementing, typically
* IVelocityControl (which should probably be removed from the template arguments).
* <IMPLEMENT> makes only explicit that the class is implementing IVelocityControl and
* appears in the inheritance list of the derived class.
*/
template <class DERIVED, class IMPLEMENT> class yarp::dev::ImplementVelocityControl : public IMPLEMENT
{
    protected:
        IVelocityControlRaw *iVelocity;
        void *helper;
        double *temp;
        
    /**
        * Initialize the internal data and alloc memory.
        * @param size is the number of controlled axes the driver deals with.
        * @param amap is a lookup table mapping axes onto physical drivers.
        * @param enc is an array containing the encoder to angles conversion factors.
        * @param sgns is an array containing the signs of the encoder readings with
        *  respect to the control/output values of the driver.
        * @return true if initialized succeeded, false if it wasn't executed, or assert.
        */
        bool initialize (int size, int *amap, double *enc);
          
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
        ImplementVelocityControl(DERIVED *y);
        
        /**
        * Destructor. Perform uninitialize if needed.
        */
        virtual ~ImplementVelocityControl();
        
        virtual bool getAxes(int *axes);
        
        virtual bool setVelocityMode();
        
        virtual bool velocityMove(int j, double v);
        virtual bool velocityMove(const double *v);
        virtual bool setRefAcceleration(int j, double acc);
        virtual bool setRefAccelerations(const double *accs);
        virtual bool getRefAcceleration(int j, double *acc);
        virtual bool getRefAccelerations(double *accs);
        virtual bool stop(int j);
        virtual bool stop();
};

#endif