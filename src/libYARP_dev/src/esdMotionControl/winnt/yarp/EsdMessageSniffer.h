// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

//
// $Id: EsdMessageSniffer.h,v 1.3 2006-09-06 21:30:55 eshuy Exp $
//
//

#ifndef __YARPEsdMessageSnifferh__
#define __YARPEsdMessageSnifferh__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Thread.h>

namespace yarp{
    namespace dev{
        class EsdMessageSniffer;
        struct EsdMessageSnifferParameters;
    }
}

/**
 * \file EsdMessageSniffer.h 
 * class for interfacing with the ESD can device driver and reading messages.
 */

/**
 * A container class to pass parameters to the open() method of the device driver.
 */
struct yarp::dev::EsdMessageSnifferParameters
{
    /**
     * Constructor.
     * @param nj is the number of controller axes.
     */
    EsdMessageSnifferParameters(int nj);

    /**
     * Destructor.
     */
    ~EsdMessageSnifferParameters();

	long int _txQueueSize;
	long int _rxQueueSize;
	long int _txTimeout;
	long int _rxTimeout;

	int _networkN;								/** network number */
	int _njoints;								/** number of joints (cards * 2) */
	unsigned char _my_address;					/** my address */
	int _polling_interval;						/** thread polling interval [ms] */
	int _timeout;								/** number of cycles before timing out */
	int (*_p) (const char *fmt, ...);			/** printf-like function for spying messages */

	unsigned char *_destinations;       		/** destination addresses */
    int *_axisMap;                              /** axis remapping lookup-table */
    double *_angleToEncoder;                    /** angle to encoder conversion factors */
    double *_zeros;                             /** encoder zeros */
};

/**
 * @ingroup dev_impl
 *
 * The EsdMessageSniffer device driver. Poll the CAN bus for incoming broadcast messages
 * and make them available through appropriate interfaces.
 */
class yarp::dev::EsdMessageSniffer : 
    public DeviceDriver,
            public os::Thread, 
            public IPidControlRaw, 
            public IAmplifierControlRaw,
            public IEncodersRaw, 
            public ImplementPidControl<EsdMessageSniffer, IPidControl>,
            public ImplementAmplifierControl<EsdMessageSniffer, IAmplifierControl>,
            public ImplementEncoders<EsdMessageSniffer, IEncoders>
{
private:
	EsdMessageSniffer(const EsdMessageSniffer&);
	void operator=(const EsdMessageSniffer&);

public:
	/**
	 * Constructor.
	 */
	EsdMessageSniffer();

	/**
	 * Destructor.
	 */
	virtual ~EsdMessageSniffer();

	/**
	 * Open the device driver.
	 * @param d is the pointer to the parameter structure which is expected to be
	 * of type EsdMessageSnifferParameters.
	 * @return true/false on success/failure.
	 */ 
	virtual bool open(EsdMessageSnifferParameters& params);

    /**
     * Open the device driver and start communication with the hardware.
     * @param config is a Searchable object containing the list of parameters.
     * @return true on success/failure.
     */
    virtual bool open(yarp::os::Searchable& config);

	/**
	 * Close the device driver.
	 * @return true/false on success/failure.
	 */
	virtual bool close(void);

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @param axis pointer to storage, return value
     * @return true/false.
     */
    virtual bool getAxes(int *axis);

    virtual bool resetEncoderRaw(int j) { return NOT_YET_IMPLEMENTED("resetEncoderRaw"); }
    virtual bool resetEncodersRaw() { return NOT_YET_IMPLEMENTED("resetEncodersRaw"); }
    virtual bool setEncoderRaw(int j, double val) { return NOT_YET_IMPLEMENTED("setEncoderRaw"); }
    virtual bool setEncodersRaw(const double *vals) { return NOT_YET_IMPLEMENTED("setEncodersRaw"); }
    virtual bool getEncoderSpeedRaw(int j, double *sp) { return NOT_YET_IMPLEMENTED("getEncoderSpeedRaw"); }
    virtual bool getEncoderSpeedsRaw(double *spds) { return NOT_YET_IMPLEMENTED("getEncoderSpeedsRaw"); }
    virtual bool getEncoderAccelerationRaw(int j, double *spds) { return NOT_YET_IMPLEMENTED("getEncoderAccelerationRaw"); }
    virtual bool getEncoderAccelerationsRaw(double *accs) { return NOT_YET_IMPLEMENTED("getEncoderAccelerationsRaw"); }

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    virtual bool getEncoderRaw(int j, double *v) { return getBCastPosition(j, v); }

    /**
     * Read the position of all axes.
     * @param encs pointer to the array that will contain the output
     * @return true/false on success/failure
     */
    virtual bool getEncodersRaw(double *encs) { return getBCastPositions(encs); }

    /** Get the current error for a joint.
     * @param j joint number
     * @param err pointer to the storage for the return value
     * @return true/false on success failure
     */
    virtual bool getErrorRaw(int j, double *err) { return getBCastPositionError(j, err); }

    /** Get the error of all joints.
     * @param errs pointer to the vector that will store the errors
     */
    virtual bool getErrorsRaw(double *errs) { return getBCastPositionErrors(errs); }

    /** Get the output of the controller (e.g. pwm value)
     * @param j joint number
     * @param out pointer to storage for return value
     * @return success/failure
     */
    virtual bool getOutputRaw(int j, double *out) { return getBCastPIDOutput(j, out); }

    /** Get the output of the controllers (e.g. pwm value)
     * @param outs pinter to the vector that will store the output values
     */
    virtual bool getOutputsRaw(double *outs) { return getBCastPIDOutputs(outs); }

    virtual bool setPidRaw(int j, const Pid &pid) { return NOT_YET_IMPLEMENTED("setPidRaw"); }
    virtual bool setPidsRaw(const Pid *pids) { return NOT_YET_IMPLEMENTED("setPidsRaw"); }
    virtual bool setReferenceRaw(int j, double ref) { return NOT_YET_IMPLEMENTED("setReferenceRaw"); }
    virtual bool setReferencesRaw(const double *refs) { return NOT_YET_IMPLEMENTED("setReferencesRaw"); }
    virtual bool setErrorLimitRaw(int j, double limit) { return NOT_YET_IMPLEMENTED("setErrorLimitRaw"); }
    virtual bool setErrorLimitsRaw(const double *limits) { return NOT_YET_IMPLEMENTED("setErrorLimitsRaw"); }
    virtual bool getPidRaw(int j, Pid *pid) { return NOT_YET_IMPLEMENTED("getPidRaw"); }
    virtual bool getPidsRaw(Pid *pids) { return NOT_YET_IMPLEMENTED("getPidsRaw"); }
    virtual bool getReferenceRaw(int j, double *ref) { return NOT_YET_IMPLEMENTED("getReferenceRaw"); }
    virtual bool getReferencesRaw(double *refs) { return NOT_YET_IMPLEMENTED("getReferencesRaw"); }
    virtual bool getErrorLimitRaw(int j, double *limit) { return NOT_YET_IMPLEMENTED("getErrorLimitRaw"); }
    virtual bool getErrorLimitsRaw(double *limits) { return NOT_YET_IMPLEMENTED("getErrorLimitsRaw"); }
    virtual bool resetPidRaw(int j) { return NOT_YET_IMPLEMENTED("resetPidRaw"); }
    virtual bool disablePidRaw(int j) { return NOT_YET_IMPLEMENTED("disablePidRaw"); }
    virtual bool enablePidRaw(int j) { return NOT_YET_IMPLEMENTED("enablePidRaw"); }
	virtual bool setOffsetRaw(int j, double v) { return NOT_YET_IMPLEMENTED("setOffsetRaw"); }

    /**
     * Read the electric current going to all motors.
     * @param vals pointer to storage for the output values
     * @return hopefully true, false in bad luck.
     */
    virtual bool getCurrentsRaw(double *vals) { return getBCastCurrents(vals); }

    /**
     * Read the electric current going to a given motor.
     * @param j motor number
     * @param val pointer to storage for the output value
     * @return probably true, might return false in bad times
     */
    virtual bool getCurrentRaw(int j, double *val) { return getBCastCurrent(j, val); }

    /* Get the status of the amplifiers, coded in a 32 bits integer for
     * each amplifier (at the moment contains only the fault, it will be 
     * expanded in the future).
     * @param st pointer to storage
     * @return true in good luck, false otherwise.
     */
    virtual bool getAmpStatusRaw(int *st) { return getBCastFaults(st); }

    virtual bool enableAmpRaw(int j) { return NOT_YET_IMPLEMENTED("enableAmpRaw"); }
    virtual bool disableAmpRaw(int j) { return NOT_YET_IMPLEMENTED("disableAmpRaw"); }
    virtual bool setMaxCurrentRaw(int j, double v) { return NOT_YET_IMPLEMENTED("setMaxCurrentRaw"); }

protected:
    bool NOT_YET_IMPLEMENTED(const char *txt) {
        ACE_OS::fprintf(stderr, "%s not yet implemented for EsdMotionControl\n", txt);
        return false;
    }

    bool setBCastMessages (int i, int value);
	bool getBCastPositions (double *p);
	bool getBCastPosition (int i, double *value);
	bool getBCastPIDOutput (int i, double *value);
	bool getBCastPIDOutputs (double *p);
	bool getBCastCurrent (int i, double *value);
	bool getBCastCurrents (double *p);
	bool getBCastFaults (int *p);
	bool getBCastPositionError (int i, double *value);
	bool getBCastPositionErrors (double *cmd);

	bool setDebugPrintFunction (void *cmd);

protected:
	void *system_resources;
    yarp::os::Semaphore _mutex;
    yarp::os::Semaphore _done;

	bool _writerequested;
	bool _noreply;

	/**
	 * helper function to check whether the enabled flag is on or off.
	 * @param axis is the axis to check for.
	 * @return true if the axis is enabled and processing of the message
	 * can in fact continue.
	 */
	inline bool ENABLED (int axis);

	/**
	 * pointer to the function printing the device debug information.
	 */
	int (*_p) (const char *fmt, ...);

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
