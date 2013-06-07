// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Lorenzo Natale, Giorgio Metta
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARPCONTROLBOARDINTERFACES__
#define __YARPCONTROLBOARDINTERFACES__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardPid.h>
#include <yarp/dev/CalibratorInterfaces.h>

/*! \file ControlBoardInterfaces.h define control board standard interfaces*/

#include <yarp/dev/IEncoders.h>
#include <yarp/dev/IEncodersTimed.h>
#include <yarp/dev/ITorqueControl.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IImpedanceControl.h>
#include <yarp/dev/IVelocityControl.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/IPositionControl2.h>
#include <yarp/dev/IOpenLoopControl.h>
//#include <yarp/dev/IControlLimits2.h>

namespace yarp {
    namespace dev {
        class IPidControlRaw;
        class IPidControl;
        class IEncodersRaw;
        class IEncoders;
        class IAmplifierControlRaw;
        class IAmplifierControl;
        class IControlDebug;
        class IControlLimitsRaw;
        class IControlLimits;
        class IControlCalibrationRaw;
        class IControlCalibration2Raw;
        class IControlCalibration;
        class IControlCalibration2;
        class IAxisInfo;
    }
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for a generic control board device implementing a PID controller.
 */
class yarp::dev::IPidControlRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IPidControlRaw() {}

    /** Set new pid value for a joint axis.
     * @param j joint number
     * @param pid new pid value
     * @return true/false on success/failure
     */
    virtual bool setPidRaw(int j, const Pid &pid)=0;

    /** Set new pid value on multiple axes.
     * @param pids pointer to a vector of pids
     * @return true/false upon success/failure
     */
    virtual bool setPidsRaw(const Pid *pids)=0;

    /** Set the controller reference point for a given axis.
     * Warning this method can result in very large torques 
     * and should be used carefully. If you do not understand
     * this warning you should avoid using this method. 
     * Have a look at other interfaces (e.g. position control).
     * @param j joint number
     * @param ref new reference point
     * @return true/false upon success/failure
     */
    virtual bool setReferenceRaw(int j, double ref)=0;

    /** Set the controller reference points, multiple axes.
     * Warning this method can result in very large torques 
     * and should be used carefully. If you do not understand
     * this warning you should avoid using this method. 
     * Have a look at other interfaces (e.g. position control).
     * @param refs pointer to the vector that contains the new reference points.
     * @return true/false upon success/failure
     */
    virtual bool setReferencesRaw(const double *refs)=0;

    /** Set the error limit for the controller on a specific joint
     * @param j joint number
     * @param limit limit value
     * @return true/false on success/failure
     */
    virtual bool setErrorLimitRaw(int j, double limit)=0;

    /** Get the error limit for the controller on all joints.
     * @param limits pointer to the vector with the new limits
     * @return true/false on success/failure
     */
    virtual bool setErrorLimitsRaw(const double *limits)=0;

    /** Get the current error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getErrorRaw(int j, double *err)=0;

    /** Get the error of all joints.
     * @param errs pointer to the vector that will store the errors
     */
    virtual bool getErrorsRaw(double *errs)=0;

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getOutputRaw(int j, double *out)=0;

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     */
    virtual bool getOutputsRaw(double *outs)=0;

    /** Get current pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return success/failure
     */
    virtual bool getPidRaw(int j, Pid *pid)=0;

    /** Get current pid value for a specific joint.
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     */
    virtual bool getPidsRaw(Pid *pids)=0;

    /** Get the current reference position of the controller for a specific joint.
     * @param j joint number
     * @param ref pointer to storage for return value
     * @return reference value 
     */
    virtual bool getReferenceRaw(int j, double *ref)=0;

    /** Get the current reference position of all controllers.
     * @param refs vector that will store the output.
     */
    virtual bool getReferencesRaw(double *refs)=0;

    /** Get the error limit for the controller on a specific joint
     * @param j joint number
     * @param limit pointer to storage
     * @return success/failure
     */
    virtual bool getErrorLimitRaw(int j, double *limit)=0;

    /** Get the error limit for all controllers
     * @param limits pointer to the array that will store the output
     * @return success or failure
     */
    virtual bool getErrorLimitsRaw(double *limits)=0;

    /** Reset the controller of a given joint, usually sets the 
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     * @return true on success, false on failure.
     */
    virtual bool resetPidRaw(int j)=0;

    /** Disable the pid computation for a joint*/
    virtual bool disablePidRaw(int j)=0;

    /** Enable the pid computation for a joint*/
    virtual bool enablePidRaw(int j)=0;

	/** Set offset value for a given pid*/
    virtual bool setOffsetRaw(int j, double v)=0;
};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for a generic control board device implementing a PID controller,
 * with scaled arguments.
 */
class yarp::dev::IPidControl
{
public:
    /**
     * Destructor.
     */
    virtual ~IPidControl() {}

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
    virtual bool setPids(const Pid *pids)=0;

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
    virtual bool setReferences(const double *refs)=0;

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
    virtual bool setErrorLimits(const double *limits)=0;

    /** Get the current error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getError(int j, double *err)=0;

    /** Get the error of all joints.
     * @param errs pointer to the vector that will store the errors
     */
    virtual bool getErrors(double *errs)=0;

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getOutput(int j, double *out)=0;

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     */
    virtual bool getOutputs(double *outs)=0;

    /** Get current pid value for a specific joint.
     * @param j joint number
     * @param pid pointer to storage for the return value.
     * @return success/failure
     */
    virtual bool getPid(int j, Pid *pid)=0;

    /** Get current pid value for a specific joint.
     * @param pids vector that will store the values of the pids.
     * @return success/failure
     */
    virtual bool getPids(Pid *pids)=0;

    /** Get the current reference position of the controller for a specific joint.
     * @param j joint number
     * @param ref pointer to storage for return value
     * @return reference value 
     */
    virtual bool getReference(int j, double *ref)=0;

    /** Get the current reference position of all controllers.
     * @param refs vector that will store the output.
     */
    virtual bool getReferences(double *refs)=0;

    /** Get the error limit for the controller on a specific joint
     * @param j joint number
     * @param limit pointer to storage
     * @return success/failure
     */
    virtual bool getErrorLimit(int j, double *limit)=0;

    /** Get the error limit for all controllers
     * @param limits pointer to the array that will store the output
     * @return success or failure
     */
    virtual bool getErrorLimits(double *limits)=0;

    /** Reset the controller of a given joint, usually sets the 
     * current position of the joint as the reference value for the PID, and resets
     * the integrator.
     * @param j joint number
     * @return true on success, false on failure.
     */
    virtual bool resetPid(int j)=0;

    /** Disable the pid computation for a joint*/
    virtual bool disablePid(int j)=0;

    /** Enable the pid computation for a joint*/
    virtual bool enablePid(int j)=0;

	/** Set offset value for a given controller*/
    virtual bool setOffset(int j, double v)=0;
};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, amplifier commands.
 */
class yarp::dev::IAmplifierControl
{
public:
    /**
     * Destructor.
     */
    virtual ~IAmplifierControl() {}

    /** Enable the amplifier on a specific joint. Be careful, check that the output
     * of the controller is appropriate (usually zero), to avoid 
     * generating abrupt movements.
     * @return true/false on success/failure
     */
    virtual bool enableAmp(int j)=0;

    /** Disable the amplifier on a specific joint. All computations within the board
     * will be carried out normally, but the output will be disabled.
     * @return true/false on success/failure
     */
    virtual bool disableAmp(int j)=0;

    /* Read the electric current going to all motors.
     * @param vals pointer to storage for the output values
     * @return hopefully true, false in bad luck.
     */
    virtual bool getCurrents(double *vals)=0;

    /* Read the electric current going to a given motor.
     * @param j motor number
     * @param val pointer to storage for the output value
     * @return probably true, might return false in bad times
     */
    virtual bool getCurrent(int j, double *val)=0;

    /* Set the maximum electric current going to a given motor. The behavior 
     * of the board/amplifier when this limit is reached depends on the
     * implementation.
     * @param j motor number
     * @param v the new value
     * @return probably true, might return false in bad times
     */
    virtual bool setMaxCurrent(int j, double v)=0;

    /* Get the status of the amplifiers, coded in a 32 bits integer for
     * each amplifier (at the moment contains only the fault, it will be 
     * expanded in the future).
     * @param st pointer to storage
     * @return true in good luck, false otherwise.
     */
    virtual bool getAmpStatus(int *st)=0;

     /* Get the status of a single amplifier, coded in a 32 bits integer
     * @param j joint number
     * @param st storage for return value
     * @return true/false success failure.
     */
    virtual bool getAmpStatus(int j, int *v)=0;
};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, amplifier commands.
 */
class yarp::dev::IAmplifierControlRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IAmplifierControlRaw() {}

    /** Enable the amplifier on a specific joint. Be careful, check that the output
     * of the controller is appropriate (usually zero), to avoid 
     * generating abrupt movements.
     * @return true/false on success/failure
     */
    virtual bool enableAmpRaw(int j)=0;

    /** Disable the amplifier on a specific joint. All computations within the board
     * will be carried out normally, but the output will be disabled.
     * @return true/false on success/failure
     */
    virtual bool disableAmpRaw(int j)=0;

    /* Read the electric current going to all motors.
     * @param vals pointer to storage for the output values
     * @return hopefully true, false in bad luck.
     */
    virtual bool getCurrentsRaw(double *vals)=0;

    /* Read the electric current going to a given motor.
     * @param j motor number
     * @param val pointer to storage for the output value
     * @return probably true, might return false in bad times
     */
    virtual bool getCurrentRaw(int j, double *val)=0;

    /* Set the maximum electric current going to a given motor. The behavior 
     * of the board/amplifier when this limit is reached depends on the
     * implementation.
     * @param j motor number
     * @param v the new value
     * @return probably true, might return false in bad times
     */
    virtual bool setMaxCurrentRaw(int j, double v)=0;

    /* Get the status of the amplifiers, coded in a 32 bits integer for
     * each amplifier (at the moment contains only the fault, it will be 
     * expanded in the future).
     * @param st pointer to storage
     * @return true/false success failure.
     */
    virtual bool getAmpStatusRaw(int *st)=0;

    /* Get the status of a single amplifier, coded in a 32 bits integer
     * @param j joint number
     * @param st storage for return value
     * @return true/false success failure.
     */
    virtual bool getAmpStatusRaw(int j, int *st)=0;
};

/** 
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, calibration commands.
 */
class yarp::dev::IControlCalibrationRaw
{
public:
    IControlCalibrationRaw(){};
    /**
     * Destructor.
     */
    virtual ~IControlCalibrationRaw() {}

    /* Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     */
    virtual bool calibrateRaw(int j, double p)=0;

    /* Check if the calibration is terminated, on a particular joint.
     * Non blocking.
     * @return true/false 
     */
    virtual bool doneRaw(int j)=0;

};

/** 
 * @ingroup dev_iface_motor
 *
 * New interface for control devices, calibration commands.
 */
class yarp::dev::IControlCalibration2Raw
{
public:
    IControlCalibration2Raw(){};
    /**
     * Destructor.
     */
    virtual ~IControlCalibration2Raw() {}

    /* Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     */
    virtual bool calibrate2Raw(int axis, unsigned int type, double p1, double p2, double p3)=0;

    /* Check if the calibration is terminated, on a particular joint.
     * Non blocking.
     * @return true/false 
     */
    virtual bool doneRaw(int j)=0;

};

/** 
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, calibration commands.
 */
class YARP_dev_API yarp::dev::IControlCalibration
{
private:
    ICalibrator *calibrator;

public:
    IControlCalibration();
    /**
     * Destructor.
     */
    virtual ~IControlCalibration() {}

    /* Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     */
    virtual bool calibrate(int j, double p)=0;

    /* Check if the calibration is terminated, on a particular joint.
     * Non blocking.
     * @return true/false 
     */
    virtual bool done(int j)=0;

    /* Set the calibrator object to be used to calibrate the robot.
     * @param c pointer to the calibrator object
     * @return true/false on success failure
     */
    virtual bool setCalibrator(ICalibrator *c);

    /* Calibrate robot by using an external calibrator. The external 
     * calibrator must be previously set by calling the setCalibration()
     * method.
     * @return true/false on success failure
     */
    virtual bool calibrate();

    virtual bool park(bool wait=true);

};

/** 
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, calibration commands.
 */
class YARP_dev_API yarp::dev::IControlCalibration2
{
private:
    ICalibrator *calibrator;

public:
    IControlCalibration2();
    /**
     * Destructor.
     */
    virtual ~IControlCalibration2() {}

    /* Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     */
    virtual bool calibrate2(int axis, unsigned int type, double p1, double p2, double p3)=0;

    /* Check if the calibration is terminated, on a particular joint.
     * Non blocking.
     * @return true/false 
     */
    virtual bool done(int j)=0;

    /* Set the calibrator object to be used to calibrate the robot.
     * @param c pointer to the calibrator object
     * @return true/false on success failure
     */
    virtual bool setCalibrator(ICalibrator *c);

    /* Calibrate robot by using an external calibrator. The external 
     * calibrator must be previously set by calling the setCalibration()
     * method.
     * @return true/false on success failure
     */
    virtual bool calibrate();

    virtual bool park(bool wait=true);

    /* Abort calibration, force the function calibrate() to return.*/
    virtual bool abortCalibration();

    /* Abort parking, force the function park() to return.*/
    virtual bool abortPark();
};

/** 
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, debug commands.
 */
class yarp::dev::IControlDebug
{
public:
    /**
     * Destructor.
     */
    virtual ~IControlDebug() {}

    /* Set the print function, pass here a pointer to your own function 
     * to print. This function should implement "printf" like parameters. 
     * @param a pointer to the print function
     * @return I don't see good reasons why it should return false.
     */
    virtual bool setPrintFunction(int (*f) (const char *fmt, ...))=0;

    /* Read the content of the board internal memory, this is usually done
     * at boot time, but can be forced by calling this method. 
     * @return true/false on success failure
     */
    virtual bool loadBootMemory()=0;
    
    /* Save the current board configuration to the internal memory,
     * this values are read at boot time or if loadBootMemory() is called.
     * @return true/false on success/failure
     */
    virtual bool saveBootMemory()=0;
};

/** 
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, limits commands.
 */
class yarp::dev::IControlLimits
{
public:
    /**
     * Destructor.
     */
    virtual ~IControlLimits() {}

    /* Set the software limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number (why am I telling you this)
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual bool setLimits(int axis, double min, double max)=0;
    
    /* Get the software limits for a particular axis.
     * @param axis joint number (again... why am I telling you this)
     * @param pointer to store the value of the lower limit
     * @param pointer to store the value of the upper limit
     * @return true if everything goes fine, false otherwise.
     */
    virtual bool getLimits(int axis, double *min, double *max)=0;
};

/** 
 * Interface for control devices. Limits commands.
 */
class yarp::dev::IControlLimitsRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IControlLimitsRaw() {}

    /* Set the software limits for a particular axis, the behavior of the
     * control card when these limits are exceeded, depends on the implementation.
     * @param axis joint number (why am I telling you this)
     * @param min the value of the lower limit
     * @param max the value of the upper limit
     * @return true or false on success or failure
     */
    virtual bool setLimitsRaw(int axis, double min, double max)=0;
    
    /* Get the software limits for a particular axis.
     * @param axis joint number (again... why am I telling you this)
     * @param pointer to store the value of the lower limit
     * @param pointer to store the value of the upper limit
     * @return true if everything goes fine, false otherwise.
     */
    virtual bool getLimitsRaw(int axis, double *min, double *max)=0;
};

/** 
 * Interface for getting information about specific axes, if available.
 */
class yarp::dev::IAxisInfo
{
public:
    /**
     * Destructor.
     */
    virtual ~IAxisInfo() {}

    virtual bool getAxisName(int axis, yarp::os::ConstString& name) = 0;
};

/* Vocabs representing the above interfaces */

#define VOCAB_CALIBRATE_JOINT VOCAB4('c','a','l','j')
#define VOCAB_CALIBRATE VOCAB3('c','a','l')
#define VOCAB_ABORTCALIB VOCAB4('a','b','c','a')
#define VOCAB_ABORTPARK VOCAB4('a','b','p','a')
#define VOCAB_CALIBRATE_DONE VOCAB4('c','a','l','d')
#define VOCAB_PARK VOCAB4('p','a','r','k')
#define VOCAB_SET VOCAB3('s','e','t')
#define VOCAB_GET VOCAB3('g','e','t')
#define VOCAB_IS VOCAB2('i','s')
#define VOCAB_FAILED VOCAB4('f','a','i','l')
#define VOCAB_OK VOCAB2('o','k')

#define VOCAB_OFFSET VOCAB3('o', 'f', 'f')

// interface IPidControl sets.
#define VOCAB_PID VOCAB3('p','i','d')
#define VOCAB_PIDS VOCAB4('p','i','d','s')
#define VOCAB_REF VOCAB3('r','e','f')
#define VOCAB_REFS VOCAB4('r','e','f','s')
#define VOCAB_LIM VOCAB3('l','i','m')
#define VOCAB_LIMS VOCAB4('l','i','m','s')
#define VOCAB_RESET VOCAB3('r','e','s')
#define VOCAB_DISABLE VOCAB3('d','i','s')
#define VOCAB_ENABLE VOCAB3('e','n','a')

// interface IPidControl gets.
#define VOCAB_ERR VOCAB3('e','r','r')
#define VOCAB_ERRS VOCAB4('e','r','r','s')
#define VOCAB_OUTPUT VOCAB3('o','u','t')
#define VOCAB_OUTPUTS VOCAB4('o','u','t','s')
#define VOCAB_REFERENCE VOCAB3('r','e','f')
#define VOCAB_REFERENCES VOCAB4('r','e','f','s')

// interface IPositionControl gets
#define VOCAB_AXES VOCAB4('a','x','e','s')
#define VOCAB_MOTION_DONE VOCAB3('d','o','n')
#define VOCAB_MOTION_DONES VOCAB4('d','o','n','s')

// interface IPositionControl sets
#define VOCAB_POSITION_MODE VOCAB4('p','o','s','d')
#define VOCAB_POSITION_MOVE VOCAB3('p','o','s')
#define VOCAB_POSITION_MOVES VOCAB4('p','o','s','s')
#define VOCAB_RELATIVE_MOVE VOCAB3('r','e','l')
#define VOCAB_RELATIVE_MOVES VOCAB4('r','e','l','s')
#define VOCAB_REF_SPEED VOCAB3('v','e','l')
#define VOCAB_REF_SPEEDS VOCAB4('v','e','l','s')
#define VOCAB_REF_ACCELERATION VOCAB3('a','c','c')
#define VOCAB_REF_ACCELERATIONS VOCAB4('a','c','c','s')
#define VOCAB_STOP VOCAB3('s','t','o')
#define VOCAB_STOPS VOCAB4('s','t','o','s')

// interface IVelocityControl sets
#define VOCAB_VELOCITY_MODE VOCAB4('v','e','l','d')
#define VOCAB_VELOCITY_MOVE VOCAB3('v','m','o')
#define VOCAB_VELOCITY_MOVES VOCAB4('v','m','o','s')

// interface IAmplifierControl sets/gets
#define VOCAB_AMP_ENABLE VOCAB3('a','e','n')
#define VOCAB_AMP_DISABLE VOCAB3('a','d','i')
#define VOCAB_AMP_CURRENT VOCAB3('a','c','u')
#define VOCAB_AMP_CURRENTS VOCAB4('a','c','u','s')
#define VOCAB_AMP_MAXCURRENT VOCAB4('m','a','x','c')
#define VOCAB_AMP_STATUS VOCAB4('a','s','t','a')
#define VOCAB_AMP_STATUS_SINGLE VOCAB4('a','s','t','s')

// interface IControlLimits sets/gets
#define VOCAB_LIMITS VOCAB4('l','l','i','m')

// interface OpenLoop Mode
#define VOCAB_OPENLOOP_MODE VOCAB4('o','l','p','d')

// interface IAxisInfo
#define VOCAB_INFO_NAME VOCAB4('n','a','m','e')

#define VOCAB_TIMESTAMP VOCAB4('t','s','t','a')
#define VOCAB_TORQUE VOCAB4('t','o','r','q')
#define VOCAB_TORQUE_MODE VOCAB4('t','r','q','d')
#define VOCAB_TRQS VOCAB4('t','r','q','s')
#define VOCAB_TRQ  VOCAB3('t','r','q')
#define VOCAB_RANGES VOCAB4('r','n','g','s')
#define VOCAB_RANGE  VOCAB3('r','n','g')
#define VOCAB_IMP_PARAM   VOCAB3('i','p','r')
#define VOCAB_IMP_OFFSET  VOCAB3('i','o','f')

#endif

