// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __CONTROLBOARDIMPL__
#define __CONTROLBOARDIMPL__

#include <yarp/dev/ControlBoardInterfaces.h>

namespace yarp{
    namespace dev {
        template <class DERIVED, class IMPLEMENT> class ImplementPositionControl;
        template <class DERIVED, class IMPLEMENT> class ImplementVelocityControl;
        template <class DERIVED, class IMPLEMENT> class ImplementPidControl;
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
        bool initialize (int size, const int *amap, const double *enc);
          
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

template <class DERIVED, class IMPLEMENT> class yarp::dev::ImplementPidControl : public IMPLEMENT
{
    protected:
        IPidControlRaw *iPid;
        Pid *tmpPids;
        void *helper;
        double *temp;
        
    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
     bool initialize (int size, int *amap, int *enc, int *zeros);
          
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
    ImplementPidControl(DERIVED *y);
        
    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementPidControl();
        
    /** Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    virtual bool setPid(int j, const Pid &pid);

    /** Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setPids(const Pid *pids);

    /** Set the controller reference point for a given axis.
     * Warning this method can result in very large torques 
     * and should be used carefully. If you do not understand
     * this warning you should avoid using this method. 
     * Have a look at other interfaces (e.g. position control).
     * @param j joint number
     * @param ref new reference point
     * @return true/false upon success/failure
     */
    virtual bool setReference(int j, double ref);

    /** Set the controller reference points, multiple axes.
     * Warning this method can result in very large torques 
     * and should be used carefully. If you do not understand
     * this warning you should avoid using this method. 
     * Have a look at other interfaces (e.g. position control).
     * @param refs pointer to the vector that contains the new reference points.
     * @return true/false upon success/failure
     */
    virtual bool setReferences(const double *refs);

    /** Set the error limit for the controller on a specifi joint
     * @param j joint number
     * @param limit limit value
     * @return true/false on success/failure
     */
    virtual bool setErrorLimit(int j, double limit);

    /** Get the error limit for the controller on all joints.
     * @param limits pointer to the vector with the new limits
     * @return true/false on success/failure
     */
    virtual bool setErrorLimits(const double *limits);

    /** Get the current error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getError(int j, double *err);

    /** Get the error of all joints.
     * @param errs pointer to the vector that will store the errors
     */
    virtual bool getErrors(double *errs);

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getOutput(int j, double *out);

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     */
    virtual bool getOutputs(double *outs);

    /** Get current pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return success/failure
     */
    virtual bool getPid(int j, Pid *pid);

    /** Get current pid value for a specific joint.
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     */
    virtual bool getPids(Pid *pids);

    /** Get the current reference position of the controller for a specific joint.
     * @param j joint number
     * @param ref pointer to storage for return value
     * @return reference value 
     */
    virtual bool getReference(int j, double *ref);

    /** Get the current reference position of all controllers.
     * @param refs vector that will store the output.
     */
    virtual bool getReferences(double *refs);

    /** Get the error limit for the controller on a specific joint
     * @param j joint number
     * @param limit pointer to storage
     * @return success/failure
     */
    virtual bool getErrorLimit(int j, double *limit);

    /** Get the error limit for all controllers
     * @param limits pointer to the array that will store the output
     * @return success or failure
     */
    virtual bool getErrorLimits(double *limits);

    /** Reset the controller of a given joint, usually sets the 
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     * @return true on success, false on failure.
     */
    virtual bool resetPid(int j);

    /** Disable the pid computation for a joint*/
    virtual bool disablePid(int j);

    /** Enable the pid computation for a joint*/
    virtual bool enablePid(int j);
};

#endif