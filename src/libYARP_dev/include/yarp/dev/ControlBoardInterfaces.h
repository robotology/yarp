// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __YARPCONTROLBOARDINTERFACES__
#define __YARPCONTROLBOARDINTERFACES__

#include <yarp/dev/DeviceDriver.h>

/*! \file ControlBoardInterfaces.h define control board standard interfaces*/

namespace yarp{
    namespace dev{
        class IControlPid;
        class IPositionControl;
        class IEncoders;
        class ITorqueControl;
        class IControlAmplifier;
        class Pid;
    }
}

/**
 * Contains the parameters for a PID
 */
class Pid
{
 public:
    double kp;     /**< proportional gain */
    double kd;     /**< derivative gain */
    double ki;	   /**< integrative gain */
    double int_max;  /**< saturation threshold for the integrator */

 public:
    /*! \brief Default Constructor */
    Pid();
    /*! \brief destructor */
    ~Pid();

    /** Constructor
     * @param kp proportional gain
     * @param kd derivative gain
     * @param ki integrative gain
     * @param int_max  integrator max output
     */
    Pid(double kp, double kd, double ki, 
        double int_max);

    /** Set proportional gain 
     * @param p new gain
     * @return true/false upon success/failure
     */
    bool setKp(double p);

    /** Set integrative gain 
     * @param i new gain
     * @return true/false upon success/failure
     */
    bool setKi(double i);

    /** Set derivative gain 
     * @param d new gain
     * @return true/false upon success/failure
     */
    bool setKd(double d);

    /** Set max threshold for the integrative part
     * @param m new max
     * @return true/false upon success/failure
     */
    bool setIntMax(double m);
};

/**
 * Interface for a generic control board device implementing a PID controller.
 */
class yarp::dev::IControlPid: public DeviceDriver
{
 public:
    /** Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    virtual bool setPid(int j, const Pid &pid)=0;

    /** Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setPid(const Pid *pids)=0;

    /** Set the controller reference point for a given axis.
     * Warning this method can result in very large torques 
     * and should be used carefully. If you do not understand
     * this warning you should avoid using this method. 
     * Have a look at other interfaces (e.g. position control).
     * @param j joint number
     * @param ref new reference point
     * @return true/false upon success/failure
     */
    virtual bool setReference(int j, double ref)=0;

    /** Set the controller reference points, multiple axes.
     * Warning this method can result in very large torques 
     * and should be used carefully. If you do not understand
     * this warning you should avoid using this method. 
     * Have a look at other interfaces (e.g. position control).
     * @param refs pointer to the vector that contains the new reference points.
     * @return true/false upon success/failure
     */
    virtual bool setReference(const double *refs)=0;

    /** Set the error limit for the controller on a specifi joint
     * @param j joint number
     * @param limit limit value
     * @return true/false on success/failure
     */
    virtual bool setErrorLimit(int j, double limit)=0;

    /** Get the error limit for the controller on all joints.
     * @param limits pointer to the vector with the new limits
     * @return true/false on success/failure
     */
    virtual bool setErrorLimit(const double *limits)=0;

    /** Set an offset value for the controller. The offset
     * is summed to the output of the pid.
     * @param j joint number
     * @param offset new offset value (impl dependent)
     * @return true/false upon success/failure
     */
    virtual bool setOffset(int j, double offset)=0;

    /** Set an offset value all controllers. The offset
     * is summed to the output of the pid.
     * @param offset pointer to the vector that will store the output values (impl dependent)
     * @return true/false upon success/failure
     */
    virtual bool setOffset(const double *offset)=0;

    /** Set max output of the controller for a single joint. 
     * This could be the value of the pwm or the electric 
     * current going to the motor, depending on the 
     * implementation. The behavior of the board when this limit
     * is reached depends on the implementation as well.
     * @param j joint number
     * @param max maximum output value (impl dependent)
     * @return true/false upon success/failure
     */
    virtual bool setMaxOutput(int j, double max)=0;

    /** Set max output, all joints (see setMaxOutput).
     * @param max pointer to the new output thresholds
     * @return true/false upon success/failure
     */
    virtual bool setMaxOutput(const double *max)=0;

    /** Get the current error for a joint
     * @param j joint number
     * @return error
     */
    virtual double getError(int j)=0;

    /** Get the error of all joints.
     * @param errs pointer to the vector that will store the errors
     */
    virtual void getError(double *errs)=0;

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @return the current output of the controller
     */
    virtual double getOutput(int j)=0;

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     */
    virtual void getOutput(double *outs)=0;

    /** Get current pid value for a specific joint.
     * @param j joint number
     * @return pid value
     */
    virtual Pid getPid(int j)=0;

    /** Get current pid value for a specific joint.
     * @param pids vector that will store the values of the pids.
     */
    virtual void getPid(Pid *pids)=0;

    /** Get the current reference position of the controller for a specific joint.
     * @param j joint number
     * @return reference value
     */
    virtual double getReference(int j)=0;

    /** Get the current reference position of all controllers.
     * @param refs vector that will store the output.
     */
    virtual void getReference(double *refs)=0;

    /** Get the error limit for the controller on a specific joint
     * @param j joint number
     * @return limit value
     */
    virtual double getErrorLimit(int j)=0;

    /** Get the error limit for all controllers
     * @param limits pointer to the array that will store the output
     */
    virtual void getErrorLimit(const double *limits)=0;

    /** Reset all controllers, usually sets the current position
     * as the reference value for each PID, , and resets
     * the integrator.
     */
    virtual void resetPid()=0;

    /** Reset the controller the a given joint, usually sets the 
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     */
    virtual void resetPid(int j)=0;

    /** Disable the pid computation */
    virtual void idle()=0;

    /** Enable the pid computation */
    virtual void enable()=0;
};

/**
 * Interface for a generic control board device implementing position control.
 */
class yarp::dev::IPositionControl: public DeviceDriver
{
 public:
    /** Set position mode. This command
     * is required by control boards implementing different
     * control methods (e.g. velocity/torque), in some cases
     * it can be left empty.
     * return true/false on success failure
     */
    virtual void setPositionMode()=0;

    /** Set new reference point for a single axis.
     * @param j joint number
     * @param ref specifies the new ref point
     * @return true/false on success/failure
     */
    virtual bool positionMove(int j, double ref)=0;

    /** Set new reference point for all axes.
     * @param refs array, new reference points.
     * @return true/false on success/failure
     */
    virtual bool positionMove(const double *refs)=0;

    /** Set relative position. The command is relative to the 
     * current position of the axis.
     * @param j joint axis number
     * @param delta relative command
     * @return true/false on success/failure
     */
    virtual bool positionRelative(int j, double delta)=0;

    /** Set relative position, all joints.
     * @param deltas pointer to the relative commands
     * @return true/false on success/failure
     */
    virtual bool positionRelative(const double *deltas)=0;

    /** Check if the current trajectory is terminated. Non blocking.
     * @return true if the trajectory is terminated, false otherwise
     */
    virtual bool checkMotionDone()=0;

    /** Set reference speed for a joint, this is the speed used during the
     * interpolation of the trajectory.
     * @param j joint number
     * @param sp speed value
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeed(int j, double sp)=0;

    /** Set reference speed on all joints. These values are used during the
     * interpolation of the trajectory.
     * @param vel pointer to the array of speed values.
     * @return true/false upon success/failure
     */
    virtual bool setRefSpeed(const double *sps)=0;

    /** Set reference acceleration for a joint. This value is used during the
     * trajectory generation.
     * @param j joint number
     * @param acc acceleration value
     * @return true/false upon success/failure
     */
    virtual bool setRefAcceleration(int j, double acc)=0;

    /** Set reference acceleration on all joints. This is the valure that is
     * used during the generation of the trajectory.
     * @param acc pointer to the array of acceleration values
     * @return true/false upon success/failure
     */
    virtual bool setRefAcceleration(const double *accs)=0;

    /** Get reference speed for a joint. Returns the speed used to 
     * generate the trajectory profile.
     * @param j joint number
     * @return the speed value
     */
    virtual double getRefSpeed(int j)=0;

    /** Get reference speed of all joints. These are the  values used during the
     * interpolation of the trajectory.
     * @param spds pointer to the array that will store the speed values.
     */
    virtual void setRefSpeed(double *sps)=0;

    /** Get reference acceleration for a joint. Returns the acceleration used to 
     * generate the trajectory profile.
     * @param j joint number
     * @return the acceleration value
     */
    virtual double getRefAcceleration(int j)=0;

    /** Get reference acceleration of all joints. These are the values used during the
     * interpolation of the trajectory.
     * @param accs pointer to the array that will store the acceleration values.
     */
    virtual void getRefAcceleration(double *accs)=0;

    /** Check if the current trajectory is terminated. Single joint version.
     * Never blocks.
     * @param j joint number
     * @return true if the trajectory is terminated, false otherwise
     */
    virtual bool checkMotionDone(int j)=0;

    /** Wait for the current trajectory of all joints to be terminated. Blocks. */
    virtual void waitForMotionDone()=0;

    /** Wait for the trajectory of a specific joint to be terminated. Blocks. 
     * @param j joint number
     */
    virtual void waitForMotionDone(int j)=0;

    /** Stop motion, single joint
     * @param j joint number
     */
    virtual void stop(int j)=0;

    /** Stop motion, multiple joints */
    virtual void stop()=0;
};

/**
 * Interface for control boards implementig velocity control.
 */
class yarp::dev::IVelocityControl: public DeviceDriver
{
public:
    /**
     * Set position mode. This command
     * is required by control boards implementing different
     * control methods (e.g. velocity/torque), in some cases
     * it can be left empty.
     * @return true/false on success failure
     */
    virtual bool setVelocityMode()=0;

    /**
     * Start motion at a given speed, single joint.
     * @param j joint number
     * @param sp speed value
     * @return bool/false upone success/failure
     */
    virtual bool velocityMove(int j, double sp)=0;

    /**
     * Start motion at a given speed, multiple joints.
     * @param sp pointer to the array containing the new speed values
     * @return true/false upon success/failure
     */
    virtual bool velocityMove(const double *sp)=0;

    /** Stop motion, single joint
     * @param j joint number
     */
    virtual void stop(int j)=0;

    /** Stop motion, multiple joints */
    virtual void stop()=0;
};

/**
 * Interface for control boards implementig torque control.
 */
class yarp::dev::ITorqueControl: public DeviceDriver
{
public:
    /**
     * Set torque control mode. This command
     * is required by control boards implementing different
     * control methods (e.g. velocity/torque), in some cases
     * it can be left empty.
     * @return true/false on success failure
     */
    virtual bool setTorqueMode()=0;

    /** Get the value of the torque on a given joint.
     * @param j joint number
     * @return torque value
     */
    virtual double getTorque(int j)=0;

    /** Get the value of the torque for all joints.
     * @param t pointer to the array that will store the output
     */
    virtual void getTorque(double *t)=0;

    /** Set the reference value of the torque for all joints.
     * @param t pointer to the array of torque values
     * @return true/false
     */
    virtual bool setTorque(const double *t)=0;

    /** Set the reference value of the torque for a given joint.
     * @param j joint number
     * @param t new value
     */
    virtual void setTorque(int j, double *t)=0;
};

/**
 * Control board, encoder interface.
 */
class yarp::dev::IEncoders: public DeviceDriver
{
public:

    /* Reset encoder, single joint. Set the encoder value to zero 
    * @param j encoder number
    * @return true/false
    */
    bool encoderReset(int j)=0;

    /* Reset encoders. Set the encoders value to zero 
    * @return true/false
    */
    bool encoderReset()=0;

    /* Set the value of the encoder for a given joint. 
    * @param j encoder number
    * @param val new value
    * @return true/false
    */
    bool encoderSet(int j, double val)=0;

    /* Read the value of an encoder.
     * @param j encoder number
     * @return the value of the encoder
     */
    double encoderRead(int j)=0;

    /* Read the istantaneous speed of an axis.
     * @param j axis number
     * @return speed value
     */
    double encoderGetSpeed(int j)=0;

    /* Read the istantaneous acceleration of an axis.
     * @param j axis number
     * @return acceleration value
     */
    double encoderGetAcceleration(int j)=0;
    
    /* Read the position of all axes.
     * @param encs pointer to the array that will contain the output
     */
    void encoderGet(double *encs)=0;

    /* Read the istantaneous speed of all axes.
     * @param spds pointer to the array that will contain the output
     */
    void encoderGetSpeed(double *spds)=0;

    /* Read the istantaneous acceleration of all axes.
     * @param accs pointer to the array that will contain the output
     */
    void encoderGetAcceleration(double *accs)=0;

    /* Set the value of all encoders.
    * @param vals pointer to the new values
    * @return true/false
    */
    bool encoderSet(const double *vals)=0;
};


/**
 * Interface for control devices. Amplifier commands.
 */
class yarp::dev::IControlAmplifier: public DeviceDriver
{
 public:
    /** Enable the amplifier. Be careful, check that the output
     * of the controller is appropriate (usually zero), to avoid 
     * generating abrupt movements.
     * @return true/false on success/failure
     */
    bool enable();

    /** Disable the amplifier. All computations withing the board
     * will be carried out normally, but the output will be disabled.
     * @return true/false on success/failure
     */
    bool disable();
};

/** 
 * Interface for control devices. Calibration commands.
 */
class IControlCalibration
{
public:
    /* Start calibration of a single joint (homing procedure).
     * @param j joint number
     * @return true/false on success failure
     */
    bool startCalibration(int j)=0;
};

#endif
//
