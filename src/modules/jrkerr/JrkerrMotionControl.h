// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


/*
 * Copyright (C) 2006 Alexandre Bernardino, Julio Gomes
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


/***********************************************************************

  YARP2 JrkerrDeviceDriver

  Windows Implementation Jrkerr Motion Control Boards
  Authors: Julio Gomes, Alexandre Bernardino, VisLab, ISR-IST

  Contact: jgomes(a)isr.ist.utl.pt, alex(a)isr.ist.utl.pt

************************************************************************/

#ifndef __JrkerrMotionControlh__
#define __JrkerrMotionControlh__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Thread.h>

namespace yarp{
    namespace dev{
        class JrkerrMotionControl;
        class JrkerrMotionControlParameters;
    }
}


// servo clock of the jrkerr boards
#define SERVOTICKTIME 0.000512

/**
 * \file JrkerrMotionControl.h 
 * class for interfacing with the JRKERR control boards.
 */

/**
 * The open parameter class containing the initialization values.
 */
class yarp::dev::JrkerrMotionControlParameters
{
private:
    JrkerrMotionControlParameters (const JrkerrMotionControlParameters&);
    void operator= (const JrkerrMotionControlParameters&);

public:
	/**
	 * Constructor (please make sure you use the constructor to allocate
     * memory).
     * @param nj is the number of controlled joints/axes.
	 */
	JrkerrMotionControlParameters (int nj);

    /**
     * Destructor, with memory deallocation.
     */
    ~JrkerrMotionControlParameters ();
	
	int (*_p) (const char *fmt, ...);			/** printf-like function for spying messages */

	int _comPort;								/** RS485 COM Port address */
	int _baudRate;								/** Serial Transmission Rate */
	int _groupAddr;                             /** For Broadcasting Commands */ 
	int _njoints;								/** number of joints/axes/controlled motors */
	
    int *_axisMap;                              /** axis remapping lookup-table */
    double *_angleToEncoder;                    /** angle to encoder conversion factors */
    double *_zeros;                             /** encoder zeros */

	double *_limitsMin;                          /** joint limits, max*/
    double *_limitsMax;                         /** joint limits, min*/
    double *_currentLimits;                     /** current limits */
	double *_velocityLimits;					/** velocity limits (deg/s)          */
	double *_accelerationLimits;				/** acceleration limits (deg/s^2)    */
    double *_velocityDefaults;					/** velocity default value (deg/s)          */
	double *_accelerationDefaults;				/** acceleration default value (deg/s^2)    */
	double *_errorLimits;						/** position error limits (encoder)  */

    Pid *_pids;                                  /** initial gains */

	int *_servoRate;
	int *_deadBand;
	int *_countperturn;

};

/**
 * @ingroup dev_impl_motor
 *
 * The Jrkerr motion controller device driver.
 * Contains a thread that takes care of polling the can bus for incoming messages.
 */
class yarp::dev::JrkerrMotionControl: 
    public DeviceDriver,
    public os::Thread, 
    public IPidControlRaw, 
    public IPositionControlRaw, 
    public IVelocityControlRaw, 
    public IEncodersRaw, 
    public IAmplifierControlRaw,
    public IControlCalibrationRaw,
    public IControlDebug,
    public IControlLimitsRaw,
    public ImplementPositionControl<JrkerrMotionControl, IPositionControl>,
    public ImplementVelocityControl<JrkerrMotionControl, IVelocityControl>,
    public ImplementPidControl<JrkerrMotionControl, IPidControl>,
    public ImplementEncoders<JrkerrMotionControl, IEncoders>,
    public ImplementControlCalibration<JrkerrMotionControl, IControlCalibration>,
    public ImplementAmplifierControl<JrkerrMotionControl, IAmplifierControl>,
    public ImplementControlLimits<JrkerrMotionControl, IControlLimits>

{
private:
    JrkerrMotionControl(const JrkerrMotionControl&);
	void operator=(const JrkerrMotionControl&);

public:
	/**
	 * Default constructor. Construction is done in two stages, first build the
     * object and then open the device driver.
	 */
    JrkerrMotionControl();

	/**
	 * Destructor.
	 */
	virtual ~JrkerrMotionControl();

	/**
	 * Open the device driver.
	 * @param par is the parameter structure 
	 * @return true/false on success/failure.
	 */ 
	virtual bool open(const JrkerrMotionControlParameters &par);

    /**
     * Open the device driver and start communication with the hardware.
     * @param config is a Searchable object containing the list of parameters.
     * @return true on success/failure.
     */
    virtual bool open(yarp::os::Searchable& config);

	/**
	 * Closes the device driver.
	 * @return true on success.
	 */
	virtual bool close(void);

    ///////////// PID INTERFACE
    //
    virtual bool setPidRaw(int j, const Pid &pid);
    virtual bool setPidsRaw(const Pid *pids);
    virtual bool setReferenceRaw(int j, double ref);
    virtual bool setReferencesRaw(const double *refs);
    virtual bool setErrorLimitRaw(int j, double limit);
    virtual bool setErrorLimitsRaw(const double *limits);
    virtual bool getErrorRaw(int j, double *err);
    virtual bool getErrorsRaw(double *errs);
    virtual bool getOutputRaw(int j, double *out);
    virtual bool getOutputsRaw(double *outs);
    virtual bool getPidRaw(int j, Pid *pid);
    virtual bool getPidsRaw(Pid *pids);
    virtual bool getReferenceRaw(int j, double *ref);
    virtual bool getReferencesRaw(double *refs);
    virtual bool getErrorLimitRaw(int j, double *limit);
    virtual bool getErrorLimitsRaw(double *limits);
    virtual bool resetPidRaw(int j);
    virtual bool disablePidRaw(int j);
    virtual bool enablePidRaw(int j);
	virtual bool setOffsetRaw(int j, double v);
    //
    /////////////////////////////// END PID INTERFACE

    //
    /// POSITION CONTROL INTERFACE RAW
    virtual bool getAxes(int *ax);
    virtual bool setPositionMode();
    virtual bool positionMoveRaw(int j, double ref);
    virtual bool positionMoveRaw(const double *refs);
    virtual bool relativeMoveRaw(int j, double delta);
    virtual bool relativeMoveRaw(const double *deltas);
    virtual bool checkMotionDoneRaw(bool *flag);
    virtual bool checkMotionDoneRaw(int j, bool *flag);
    virtual bool setRefSpeedRaw(int j, double sp);
    virtual bool setRefSpeedsRaw(const double *spds);
    virtual bool setRefAccelerationRaw(int j, double acc);
    virtual bool setRefAccelerationsRaw(const double *accs);
    virtual bool getRefSpeedRaw(int j, double *ref);
    virtual bool getRefSpeedsRaw(double *spds);
    virtual bool getRefAccelerationRaw(int j, double *acc);
    virtual bool getRefAccelerationsRaw(double *accs);
    virtual bool stopRaw(int j);
    virtual bool stopRaw();
    //
    /////////////////////////////// END Position Control INTERFACE

    ///////////// BEGIN Velocity control interface     ///
    virtual bool setVelocityMode();
    virtual bool velocityMoveRaw(int j, double sp);
    virtual bool velocityMoveRaw(const double *sp);
    //
    /////////////////////////////// END Velocity Control INTERFACE

    //////////////////////// BEGIN EncoderInterface
    //
    virtual bool resetEncoderRaw(int j);
    virtual bool resetEncodersRaw();
    virtual bool setEncoderRaw(int j, double val);
    virtual bool setEncodersRaw(const double *vals);
    virtual bool getEncoderRaw(int j, double *v);
    virtual bool getEncodersRaw(double *encs);
    virtual bool getEncoderSpeedRaw(int j, double *sp);
    virtual bool getEncoderSpeedsRaw(double *spds);
    virtual bool getEncoderAccelerationRaw(int j, double *spds);
    virtual bool getEncoderAccelerationsRaw(double *accs);
    //
    ///////////////////////// END Encoder Interface

    ////// Amplifier interface
    //
    virtual bool enableAmpRaw(int j);
    virtual bool disableAmpRaw(int j);
    virtual bool getCurrentsRaw(double *vals);
    virtual bool getCurrentRaw(int j, double *val);
    virtual bool getMaxCurrentRaw(int j, double *val);
    virtual bool setMaxCurrentRaw(int j, double val);
    virtual bool getAmpStatusRaw(int *st);
    virtual bool getAmpStatusRaw(int k, int *st);
    //
    /////////////// END AMPLIFIER INTERFACE

    ////// calibration
    virtual bool calibrateRaw(int j, double p);
    virtual bool doneRaw(int j);

    /// IControlDebug Interface
    virtual bool setPrintFunction(int (*f) (const char *fmt, ...));
    virtual bool loadBootMemory();
    virtual bool saveBootMemory();

    /////// Limits
    virtual bool setLimitsRaw(int axis, double min, double max);
    virtual bool getLimitsRaw(int axis, double *min, double *max);

protected:
   	bool setBCastMessages (int axis, double v);

protected:
	void *system_resources;
    yarp::os::Semaphore _mutex;
    yarp::os::Semaphore _done;

	bool _writerequested;
	bool _noreply;
	
	/**
	 * pointer to the function printing the device debug information.
	 */
	int (*_p) (const char *fmt, ...);

	/**
	 * helper function to check whether the enabled flag is on or off.
	 * @param axis is the axis to check for.
	 * @return true if the axis is enabled and processing of the message
	 * can in fact continue.
	 */
	inline bool ENABLED (int axis);

	virtual void run(void);


	// internal stuff.
	double *_ref_speeds;            //encoder pulses per second
	double *_ref_accs;              //encoder pulses per secong square
	double *_ref_positions;         //encoder pulses
    double *_velocity_limits;        //encoder pulses per second
	double *_acceleration_limits;	//encoder pulses per second square
    double *_velocity_defaults;      //encoder pulses per second
    double *_acceleration_defaults;  //encoder pulses per second square

	int *_kp;		/** controller proportional gain */
 	int *_kd;		/** controller derivative gain */
	int *_ki;		/** controller integral gain */
	int *_il;		/** controller integration limit */
	int *_ol;		/** controller output limit */
	int *_cl;		/** controller current limit */
	int *_el;		/** position error limit */
	int *_sr;		/** servo rate divider */
	int *_dc;		/** dead band compensation */
	int *_pl;		/** positive position limit */
	int *_nl;		/** negative position limit */
	int *_ct;       /** encoder pulses per turn */
	int *_en;		/** amplifier enable flags */
	int *_mode;     /** control modes: 0 = position, 1 = velocity **/
	// Modes are implemented via jrkerr trapezoidal control profiles
	// Default is positions

	
};



/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_jrkerr jrkerr

 The Jrkerr motion controller, see yarp::dev::JrkerrMotionControl.

*/


#endif
