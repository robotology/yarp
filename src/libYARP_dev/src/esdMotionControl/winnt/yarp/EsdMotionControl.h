// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

///
/// $Id: EsdMotionControl.h,v 1.18 2006-08-01 23:20:30 gmetta Exp $
///
///

#ifndef __EsdMotionControlh__
#define __EsdMotionControlh__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Thread.h>

namespace yarp{
    namespace dev{
        class EsdMotionControl;
        class EsdMotionControlParameters;
    }
}

/**
 * \file EsdMotionControl.h 
 * class for interfacing with the ESD can device driver.
 */

/**
 * \include UserDoc_dev_motorcontrol.dox
 */

/**
 * The open parameter class containing the initialization values.
 */
class yarp::dev::EsdMotionControlParameters
{
private:
    EsdMotionControlParameters (const EsdMotionControlParameters&);
    void operator= (const EsdMotionControlParameters&);

public:
	/**
	 * Constructor (please make sure you use the constructor to allocate
     * memory).
     * @param nj is the number of controlled joints/axes.
	 */
	EsdMotionControlParameters (int nj);

    /**
     * Destructor, with memory deallocation.
     */
    ~EsdMotionControlParameters ();
	
	long int _txQueueSize;
	long int _rxQueueSize;
	long int _txTimeout;
	long int _rxTimeout;

	int _networkN;								/** network number */
	int _njoints;								/** number of joints/axes/controlled motors */
	unsigned char *_destinations;       		/** destination addresses */
	unsigned char _my_address;					/** my address */
	int _polling_interval;						/** thread polling interval [ms] */
	int _timeout;								/** number of cycles before timing out */
	int (*_p) (const char *fmt, ...);			/** printf-like function for spying messages */

    int *_axisMap;                              /** axis remapping lookup-table */
    double *_angleToEncoder;                    /** angle to encoder conversion factors */
    double *_zeros;                             /** encoder zeros */
    Pid *_pids;                                  /** initial gains */
    double *_limitsMin;                          /** joint limits, max*/
    double *_limitsMax;                         /** joint limits, min*/
    double *_currentLimits;                     /** current limits */
};

/**
 * @ingroup dev_impl
 *
 * The ESD motion controller device driver.
 * Contains a thread that takes care of polling the can bus for incoming messages.
 */
class yarp::dev::EsdMotionControl: 
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
    public ImplementPositionControl<EsdMotionControl, IPositionControl>,
    public ImplementVelocityControl<EsdMotionControl, IVelocityControl>,
    public ImplementPidControl<EsdMotionControl, IPidControl>,
    public ImplementEncoders<EsdMotionControl, IEncoders>,
    public ImplementControlCalibration<EsdMotionControl, IControlCalibration>,
    public ImplementAmplifierControl<EsdMotionControl, IAmplifierControl>,
    public ImplementControlLimits<EsdMotionControl, IControlLimits>
{
private:
    EsdMotionControl(const EsdMotionControl&);
	void operator=(const EsdMotionControl&);

public:
	/**
	 * Default constructor. Construction is done in two stages, first build the
     * object and then open the device driver.
	 */
    EsdMotionControl();

	/**
	 * Destructor.
	 */
	virtual ~EsdMotionControl();

	/**
	 * Open the device driver.
	 * @param d is the parameter structure 
	 * @return true/false on success/failure.
	 */ 
	virtual bool open(const EsdMotionControlParameters &par);

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

    ///////////// Velocity control interface raw
    ///
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
    virtual bool setMaxCurrentRaw(int j, double val);
    virtual bool getAmpStatusRaw(int *st);
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
	 * filter for recurrent messages.
	 */
	int _filter;

	/**
	 * helper function to check whether the enabled flag is on or off.
	 * @param axis is the axis to check for.
	 * @return true if the axis is enabled and processing of the message
	 * can in fact continue.
	 */
	inline bool ENABLED (int axis);

	virtual void run(void);

	// helper functions
	bool _writeWord16 (int msg, int axis, short s);
	bool _writeWord16Ex (int msg, int axis, short s1, short s2);
	bool _readWord16 (int msg, int axis, short& value);
	bool _readWord16Array (int msg, double *out);
	bool _readDWord (int msg, int axis, int& value);
	bool _readDWordArray (int msg, double *out);
	bool _writeDWord (int msg, int axis, int value);
	bool _writeNone (int msg, int axis);

	// internal stuff.
	double *_ref_speeds;		// used for position control.
	double *_command_speeds;	// used for velocity control.
	double *_ref_accs;			// for velocity control, in position min jerk eq is used.
	double *_ref_positions;		// for position control.

	enum { MAX_SHORT = 32767, MIN_SHORT = -32768, MAX_INT = 0x7fffffff, MIN_INT = 0x80000000 };
	enum { ESD_CAN_SKIP_ADDR = 0x80 };

	inline short S_16(double x) const 
	{
		if (x <= double(-(MAX_SHORT))-1)
			return MIN_SHORT;
		else
		if (x >= double(MAX_SHORT))
			return MAX_SHORT;
		else
			return short(x + .5);
	}

	inline int S_32(double x) const
	{
		if (x <= double(-(MAX_INT))-1.0)
			return MIN_INT;
		else
		if (x >= double(MAX_INT))
			return MAX_INT;
		else
			return int(x + .5);
	}
};



#endif
