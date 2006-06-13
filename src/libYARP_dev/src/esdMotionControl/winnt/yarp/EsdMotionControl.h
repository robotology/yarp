
///
/// $Id: EsdMotionControl.h,v 1.2 2006-06-13 14:49:31 natta Exp $
///
///

#ifndef __EsdMotionControlh__
#define __EsdMotionControlh__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>
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
 * The open parameter class containing the initialization values.
 */
class yarp::dev::EsdMotionControlParameters
{
public:
	/**
	 * Constructor.
	 */
	EsdMotionControlParameters ();
    ~EsdMotionControlParameters ();
	
	long int _txQueueSize;
	long int _rxQueueSize;
	long int _txTimeout;
	long int _rxTimeout;

	int _networkN;								/** network number */
	int _njoints;								/** number of joints (cards * 2) */
	unsigned char *_destinations;       		/** destination addresses */
	unsigned char _my_address;					/** my address */
	int _polling_interval;						/** thread polling interval [ms] */
	int _timeout;								/** number of cycles before timing out */
	int (*_p) (const char *fmt, ...);			/** printf-like function for spying messages */

    int *axisMap;
    double *angleToEncoder;
    double *zeros;
    int nj;
};


/**
 * The esd controller device driver.
 * Contains a thread that takes care of polling the can bus for incoming messages.
 */
class yarp::dev::EsdMotionControl: 
    public os::Thread, 
          public IPidControl, 
          public IPositionControl, 
          public IVelocityControl, 
          public IEncoders, 
          public IAmplifierControl,
          public IControlCalibration,
          public IControlDebug,
          public IControlLimits,
          public IPositionControl2
{
private:
    EsdMotionControl(const EsdMotionControl&);
	void operator=(const EsdMotionControl&);

public:
	/**
	 * Constructor.
	 */
    EsdMotionControl(const EsdMotionControlParameters &par);

	/**
	 * Destructor.
	 */
	virtual ~EsdMotionControl();

	/**
	 * Opens the device driver.
	 * @param d is the parameter structure 
	 * @return true/false on success.
	 */ 
	bool open(const EsdMotionControlParameters &par);

	/**
	 * Closes the device driver.
	 * @return true on success.
	 */
	virtual bool close(void);

    ///////////// PID INTERFACE
    //
    virtual bool setPid(int j, const Pid &pid);
    virtual bool setPids(const Pid *pids);
    virtual bool setReference(int j, double ref);
    virtual bool setReferences(const double *refs);
    virtual bool setErrorLimit(int j, double limit);
    virtual bool setErrorLimits(const double *limits);
    virtual bool getError(int j, double *err);
    virtual bool getErrors(double *errs);
    virtual bool getOutput(int j, double *out);
    virtual bool getOutputs(double *outs);
    virtual bool getPid(int j, Pid *pid);
    virtual bool getPids(Pid *pids);
    virtual bool getReference(int j, double *ref);
    virtual bool getReferences(double *refs);
    virtual bool getErrorLimit(int j, double *limit);
    virtual bool getErrorLimits(double *limits);
    virtual bool resetPid(int j);
    virtual bool disablePid(int j);
    virtual bool enablePid(int j);
    //
    /////////////////////////////// END PID INTERFACE
    //
    /// POSITION CONTROL INTERFACE
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
    //
    /////////////////////////////// END Position Control INTERFACE

    ///////////// Velocity control interface
    ///
    virtual bool setVelocityMode();
    virtual bool velocityMove(int j, double sp);
    virtual bool velocityMove(const double *sp);
    //
    /////////////////////////////// END Velocity Control INTERFACE

    //////////////////////// BEGIN EncoderInterface
    //
    virtual bool resetEncoder(int j);
    virtual bool resetEncoders();
    virtual bool setEncoder(int j, double val);
    virtual bool setEncoders(const double *vals);
    virtual bool getEncoder(int j, double *v);
    virtual bool getEncoders(double *encs);
    virtual bool getEncoderSpeed(int j, double *sp);
    virtual bool getEncoderSpeeds(double *spds);
    virtual bool getEncoderAcceleration(int j, double *spds);
    virtual bool getEncoderAccelerations(double *accs);
    //
    ///////////////////////// END Encoder Interface

    ////// Amplifier interface
    //
    virtual bool enableAmp(int j);
    virtual bool disableAmp(int j);
    virtual bool getCurrents(double *vals);
    virtual bool getCurrent(int j, double *val);
    virtual bool setMaxCurrent(int j, double val);
    virtual bool getAmpStatus(int *st);
    //
    /////////////// END AMPLIFIER INTERFACE

    ////// calibration
    virtual bool calibrate(int j);

    /// IControlDebug Interface
    virtual bool setPrintFunction(int (*f) (const char *fmt, ...));
    virtual bool loadBootMemory();
    virtual bool saveBootMemory();

    /////// Limits
    virtual bool setLimits(int axis, double min, double max);
    virtual bool getLimits(int axis, double *min, double *max);

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

	/// helper functions
	bool _writeWord16 (int msg, int axis, short s);
	bool _writeWord16Ex (int msg, int axis, short s1, short s2);
	bool _readWord16 (int msg, int axis, short& value);
	bool _readWord16Array (int msg, double *out);
	bool _readDWord (int msg, int axis, int& value);
	bool _readDWordArray (int msg, double *out);
	bool _writeDWord (int msg, int axis, int value);
	bool _writeNone (int msg, int axis);

	/// internal stuff.
	double *_ref_speeds;
	double *_ref_accs;
	double *_ref_positions;

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