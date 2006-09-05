// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __CONTROLBOARDIMPL__
#define __CONTROLBOARDIMPL__

#include <yarp/dev/ControlBoardInterfaces.h>

namespace yarp{
    namespace dev {
        template <class DERIVED, class IMPLEMENT> class ImplementPositionControl;
        template <class DERIVED, class IMPLEMENT> class ImplementVelocityControl;
        template <class DERIVED, class IMPLEMENT> class ImplementPidControl;
        template <class DERIVED, class IMPLEMENT> class ImplementEncoders;
        template <class DERIVED, class IMPLEMENT> class ImplementAmplifierControl;
        template <class DERIVED, class IMPLEMENT> class ImplementControlCalibration;
        template <class DERIVED, class IMPLEMENT> class ImplementControlLimits;
    }
}

/** 
* Default implementation of the IPositionControl interface. This template class can
* be used to easily provide an implementation of IPositionControl. It takes two
* arguments, the class it is derived from and the class it is implementing, typically
* IPositionControl (which should probably be removed from the template arguments).
* "<IMPLEMENT>" makes only explicit that the class is implementing IPositionControl and
* appears in the inheritance list of the derived class.
*/
template <class DERIVED, class IMPLEMENT> 
class yarp::dev::ImplementPositionControl : public IMPLEMENT
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
* "<IMPLEMENT>" makes only explicit that the class is implementing IVelocityControl and
* appears in the inheritance list of the derived class.
*/
template <class DERIVED, class IMPLEMENT> 
class yarp::dev::ImplementVelocityControl : public IMPLEMENT
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
        * @param zos is an array containing the zeros of the encoders.
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

template <class DERIVED, class IMPLEMENT> 
class yarp::dev::ImplementPidControl : public IMPLEMENT
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
     * @param enc is an array containing the encoder to angles conversion factors.
     * @param zos is an array containing the zeros of the encoders.
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

    /** Enable the pid computation for a joint*/
    virtual bool setOffset(int j, double v);

};

template <class DERIVED, class IMPLEMENT> 
class yarp::dev::ImplementEncoders : public IMPLEMENT
{
    protected:
        IEncodersRaw *iEncoders;
        void *helper;
        double *temp;

     /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @param enc is an array containing the encoder to angles conversion factors.
     * @param zos is an array containing the zeros of the encoders.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
     bool initialize (int size, const int *amap, const double *enc, const double *zos);
          
     /**
      * Clean up internal data and memory.
      * @return true if uninitialization is executed, false otherwise.
      */
     bool uninitialize ();

    public:
    /* Constructor.
    * @param y is the pointer to the class instance inheriting from this 
    *  implementation.
    */
    ImplementEncoders(DERIVED *y);
        
    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementEncoders();


    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     */
    virtual bool getAxes(int *ax);

    /**
     * Reset encoder, single joint. Set the encoder value to zero 
     * @param j encoder number
     * @return true/false
     */
    virtual bool resetEncoder(int j);

    /**
     * Reset encoders. Set the encoders value to zero 
     * @return true/false
     */
    virtual bool resetEncoders();

    /**
     * Set the value of the encoder for a given joint. 
     * @param j encoder number
     * @param val new value
     * @return true/false
     */
    virtual bool setEncoder(int j, double val);

    /**
     * Set the value of all encoders.
     * @param vals pointer to the new values
     * @return true/false
     */
    virtual bool setEncoders(const double *vals);

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    virtual bool getEncoder(int j, double *v);

    /**
     * Read the position of all axes.
     * @param encs pointer to the array that will contain the output
     * @return true/false on success/failure
     */
    virtual bool getEncoders(double *encs);

    /**
     * Read the instantaneous speed of an axis.
     * @param j axis number
     * @param spds pointer to storage for the output
     * @return true if successful, false ... otherwise.
     */
    virtual bool getEncoderSpeed(int j, double *spds);

    /**
     * Read the instantaneous speed of all axes.
     * @param spds pointer to storage for the output values
     * @return guess what? (true/false on success or failure).
     */
    virtual bool getEncoderSpeeds(double *spds);
    
    /**
     * Read the instantaneous acceleration of an axis.
     * @param j axis number
     * @param spds pointer to the array that will contain the output
     */
    virtual bool getEncoderAcceleration(int j, double *spds);

    /**
     * Read the instantaneous acceleration of all axes.
     * @param accs pointer to the array that will contain the output
     * @return true if all goes well, false if anything bad happens. 
     */
    virtual bool getEncoderAccelerations(double *accs);
};

template <class DERIVED, class IMPLEMENT> 
class yarp::dev::ImplementControlCalibration: public IMPLEMENT
{
    protected:
        IControlCalibrationRaw *iCalibrate;
        void *helper;
        double *temp;

     /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @param enc is an array containing the encoder to angles conversion factors.
     * @param zos is an array containing the zeros of the encoders.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
     bool initialize (int size, const int *amap, const double *enc, const double *zos);
          
     /**
      * Clean up internal data and memory.
      * @return true if uninitialization is executed, false otherwise.
      */
     bool uninitialize ();

    public:
    /* Constructor.
    * @param y is the pointer to the class instance inheriting from this 
    *  implementation.
    */
    ImplementControlCalibration(DERIVED *y);
        
    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementControlCalibration();

    virtual bool calibrate(int j, double p);

    virtual bool done(int j);
};

template <class DERIVED, class IMPLEMENT> 
class yarp::dev::ImplementControlLimits: public IMPLEMENT
{
protected:
     IControlLimitsRaw *iLimits;
     void *helper;
     double *temp;

     /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @param enc is an array containing the encoder to angles conversion factors.
     * @param zos is an array containing the zeros of the encoders.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
     bool initialize (int size, const int *amap, const double *enc, const double *zos);
          
     /**
      * Clean up internal data and memory.
      * @return true if uninitialization is executed, false otherwise.
      */
     bool uninitialize ();

public:
    /* Constructor.
    * @param y is the pointer to the class instance inheriting from this 
    *  implementation.
    */
    ImplementControlLimits(DERIVED *y);
        
    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementControlLimits();

    /* Set the software limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number (why am I telling you this)
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual bool setLimits(int axis, double min, double max);
    
    /* Get the software limits for a particular axis.
     * @param axis joint number (again... why am I telling you this)
     * @param pointer to store the value of the lower limit
     * @param pointer to store the value of the upper limit
     * @return true if everything goes fine, false if something bad happens (yes, sometimes life is tough)
     */
    virtual bool getLimits(int axis, double *min, double *max);
};


template <class DERIVED, class IMPLEMENT> 
class yarp::dev::ImplementAmplifierControl: public IMPLEMENT
{
protected:
       IAmplifierControlRaw *iAmplifier;
       void *helper;
       double *dTemp;
       int *iTemp;

     /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @param enc is an array containing the encoder to angles conversion factors.
     * @param zos is an array containing the zeros of the encoders.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
     bool initialize (int size, const int *amap, const double *enc, const double *zos);
          
     /**
      * Clean up internal data and memory.
      * @return true if uninitialization is executed, false otherwise.
      */
     bool uninitialize ();

public:
    /* Constructor.
    * @param y is the pointer to the class instance inheriting from this 
    *  implementation.
    */
    ImplementAmplifierControl(DERIVED *y);
        
    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementAmplifierControl();

    /** Enable the amplifier on a specific joint. Be careful, check that the output
     * of the controller is appropriate (usually zero), to avoid 
     * generating abrupt movements.
     * @return true/false on success/failure
     */
    virtual bool enableAmp(int j);

    /** Disable the amplifier on a specific joint. All computations within the board
     * will be carried out normally, but the output will be disabled.
     * @return true/false on success/failure
     */
    virtual bool disableAmp(int j);

    /* Read the electric current going to all motors.
     * @param vals pointer to storage for the output values
     * @return hopefully true, false in bad luck.
     */
    virtual bool getCurrents(double *vals);

    /* Read the electric current going to a given motor.
     * @param j motor number
     * @param val pointer to storage for the output value
     * @return probably true, might return false in bad times
     */
    virtual bool getCurrent(int j, double *val);

    /* Set the maximum electric current going to a given motor. The behavior 
     * of the board/amplifier when this limit is reached depends on the
     * implementation.
     * @param j motor number
     * @param v the new value
     * @return probably true, might return false in bad times
     */
    virtual bool setMaxCurrent(int j, double v);

    /* Get the status of the amplifiers, coded in a 32 bits integer for
     * each amplifier (at the moment contains only the fault, it will be 
     * expanded in the future).
     * @param st pointer to storage
     * @return true in good luck, false otherwise.
     */
    virtual bool getAmpStatus(int *st);
};
#endif
