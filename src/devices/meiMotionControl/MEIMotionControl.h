/*
* Copyright (C) 2007 Mattia Castelnovi
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#ifndef MEIMotionControlh
#define MEIMotionControlh

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>


using namespace yarp::os;

namespace yarp{
    namespace dev{
        class MEIMotionControl;
        class MEIMotionControlParameters;
    }
}

class LowLevelPID
{

public:

	double KP;
	double KD;
	double KI;
	double AC_FF;
	double VEL_FF;
	double I_LIMIT;
	double OFFSET;
	double T_LIMIT;
	double SHIFT;
	double FRICT_FF;


	LowLevelPID()
	{
		KP = 0.0;
		KD = 0.0;
		KI = 0.0;
		AC_FF = 0.0;
		VEL_FF = 0.0;
		I_LIMIT = 0.0;
		OFFSET = 0.0;
		T_LIMIT = 0.0;
		SHIFT = 0.0;
		FRICT_FF = 0.0;
	}

	LowLevelPID(double kp , double kd , double ki , double ac_ff, double vel_ff,
				double i_limit, double offset, double t_limit, double shift, double frict_ff)
	{
		KP = kp;
		KD = kd;
		KI = ki;
		AC_FF = ac_ff;
		VEL_FF = vel_ff;
		I_LIMIT = i_limit;
		OFFSET = offset;
		T_LIMIT = t_limit;
		SHIFT = shift;
		FRICT_FF = frict_ff;
	}

	LowLevelPID(double *pid_par)
	{
		KP = pid_par[0];
		KD = pid_par[1];
		KI = pid_par[2];
		AC_FF = pid_par[3];
		VEL_FF = pid_par[4];
		I_LIMIT = pid_par[5];
		OFFSET = pid_par[6];
		T_LIMIT = pid_par[7];
		SHIFT = pid_par[8];
		FRICT_FF = pid_par[9];
	}


};

typedef short int16;

const int MAX_PORTS = 2;

//REMEMBER
//all the command you get to the motor have to be converte with
//functions like RadToEnc otherwise the values and the signs
//are not coherent with what you want (for example D-H configuration)

class BabyArmProperty : public Property {
public:
	BabyArmProperty() {
		fromString("(Joints	6)\
					(MaxDAC	32767.0 32767.0 32767.0 32767.0 32767.0 32767.0)\
					(AxisMap 0 1 2 3 4 5)\
					(EncWheel 1000.0 1000.0 1000.0 800.0 800.0 800.0)\
					(Encoder 46.72 -69.9733 42.9867 -43.5111 39.3846 -31.7692)\
					(FwdCouple 0.0 0.0 0.0 9.8462 1.0 -5.5999886532)\
					(Zeros 0.0 1.570796 -1.570796 0.0 0.0 0.0)\
					(Signs 0 0 0 0 0 0)\
					(Stiff 0 0 0 1 1 1)\
					(Min  -5000.0 -50000.0 -28000.0 -20000.0 -100000.0 -100000.0)\
					(Max 9000.0 12000.0 7000.0 40000.0 10000.0 100000.0)\
					(Enable	1 1 1 0 0 0)\
					(Learn 0)\
					(Rate 40)\
					(TrajSteps	50)\
					(meiPortAddr 0x300)\
					(RestingWayPoint1  5.0 0.0 0.0 0.0 0.0 0.0)\
					(RestingFinal	  -8.0 0.0 0.0 0.0 0.0 0.0)\
					(Park1	 10.0 0.0 0.0 0.0 0.0 0.0)\
					(Park2	-10.0 0.0 0.0 0.0 0.0 0.0)\
					(ParkSpeeds	10 10 10 20 20 20)\
					(Speeds		1 1 1 1 1 1)\
					(Accelerations	200 200 200 500 500 500)\
					(ArmStatusPortName	 \\\\armcontrol\\\\o:status)\
					(ArmPositionPortName \\\\armcontrol\\\\position\\\\o:vect)\
					(ArmTorquesPortName	 \\\\armcontrol\\\\torques\\\\o:vect)\
					(RandomThreadRate	500)\
					(MAXINIT  0.0  45.0  25.0 0.0 0.0 0.0)\
					(MININIT -5.0  15.0 -30.0 0.0 0.0 0.0)\
					(MAXFINAL	0.0  45.0  25.0 0.0 0.0 0.0)\
					(MINFINAL  -5.0  15.0 -30.0 0.0 0.0 0.0)\
					(GAUSS_AVERAGE	-2 0 0 0 0 0)\
					(GAUSS_STD  3    3   3 0 0 0)\
					(GAUSS_MAX  2   10 10 0 0 0)\
					(GAUSS_MIN -10 -10 -10 0 0 0)\
					(PortName \\\\armcontrol\\\\wrist\\\\i:vect)\
					(LPid0 -300.0 -1230.0  0.0  0.0  -82.0 32767.0    0.0 32767.0 0.0  -25.0)\
					(LPid1 -270.0  -500.0 -2.0  0.0 -100.0 32767.0    0.0 32767.0 0.0    0.0)\
					(LPid2 -290.0  -700.0 -2.0  0.0 -150.0 32767.0    0.0 32767.0 0.0 -100.0)\
					(LPid3 -320.0  -600.0 -1.0  0.0    0.0 32767.0    0.0 32767.0 0.0    0.0)\
					(LPid4 -400.0  -900.0 -2.0  0.0 -100.0 32767.0 1000.0 32767.0 0.0 -100.0)\
					(LPid5 -300.0  -600.0 -20.0 0.0 -100.0 32767.0    0.0 32767.0 0.0    0.0)");

    };
} ;


// why is this file appearing twice? in src/mei and here? --paulfitz
// removing messy namespace from documentation

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace _BabybotArm
{
	const int _nj = 6;
	const LowLevelPID _highPIDs[_nj] =
	{
		LowLevelPID(-310.0, -1500.0, 0.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, -30.0),	//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(-270.0, -500.0, 0.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(-290.0, -700.0, 0.0, 0.0, -150.0, 32767.0, 0.0, 32767.0, 0.0, -100),	
		LowLevelPID(-320.0, -600.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(-300.0, -900.0, 0.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, -100.0),	
		LowLevelPID(-300.0, -600.0, 0.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
	};
	
	const LowLevelPID _lowPIDs[_nj] = 
	{
		LowLevelPID(-5.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),		//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(-5.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(-5.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
		LowLevelPID(-320.0, -600.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(-300.0, -900.0, 0.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, -100.0),	
		LowLevelPID(-300.0, -600.0, 0.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
	};

	const double _zeros[_nj] = {0.0, 1.570796, -1.570796, 0.0, 0.0, 0.0};
	const int _axis_map[_nj] = {0, 1, 2, 3, 4, 5};
	const int _signs[_nj] = {0, 0, 0, 0, 0, 0};
	const double _encWheels[_nj] = {1000.0, 1000.0, 1000.0, 800.0, 800.0, 800.0};
//	const double _encoders[_nj] = {-46.72, 69.9733, -42.9867, 43.5111, 39.3846, 31.7692};
	const double _encoders[_nj] = {46.72, -69.9733, 42.9867, -43.5111, 39.3846, -31.7692};//10/10
//	const double _encoders[_nj] = {46.72, 69.9733, 42.9867, 43.5111, -39.3846, 31.7692};
	const double _fwdCouple[_nj] = {0.0, 0.0, 0.0, 9.8462*_encWheels[3], 1.0*_encWheels[4], -5.5999886532*_encWheels[5]};//10/10
//	const double _fwdCouple[_nj] = {0.0, 0.0, 0.0, 9.8462, 1.0, -5.5999886532};
//	const double _fwdCouple[_nj] = {0.0, 0.0, 0.0, -9.8462, 1.0, 5.5999886532};
	const int _stiffPID[_nj] = {0, 0, 0, 1, 1, 1};
	const double _maxDAC[_nj] = {32767.0, 32767.0, 32767.0, 32767.0, 32767.0, 32767.0};

	const double _limitsMax[_nj]= { 5000.0,  50000.0, 28000.0,  20000.0,  100000.0,  100000.0};
	const double _limitsMin[_nj]= {-9000.0 ,-12000.0, -7000.0, -40000.0 , -10000.0 ,-100000.0};
	
//	const double _limitsMax[_nj]= {30.0 , 50.0  ,40.0,  90.0,	 70.0,   40.0};
//	const double _limitsMin[_nj]= {-20.0 ,-30.0, -40.0, -90.0 ,-90.0 ,-270.0};
}; // namespace

#endif

/**
 * \file MEIMotionControl.h 
 * class for interfacing with the MEI can device driver.
 */

/**
 * \include UserDoc_dev_motorcontrol.dox
 */

/**
 * The open parameter class containing the initialization values.
 */
class yarp::dev::MEIMotionControlParameters
{
private:
    MEIMotionControlParameters (const MEIMotionControlParameters&);
    void operator= (const MEIMotionControlParameters&);

public:
	/**
	 * Constructor (please make sure you use the constructor to allocate
     * memory).
     * @param nj is the number of controlled joints/axes.
	 */
	MEIMotionControlParameters (int nj);
//	MEIMotionControlParameters ();

    /**
     * Destructor, with memory deallocation.
     */
	~MEIMotionControlParameters();

	int load();

//	virtual bool open(const MEIMotionControlParameters &par);

	int copy (const MEIMotionControlParameters& peer);

//private:
	void _realloc(int nj);

public:
			int *_limitsMax;
			int *_limitsMin;
			double *_zeros;
			int *_signs;
			int *_axis_map;
			int *_inv_axis_map;
			double *_encoderToAngles;

			LowLevelPID *_highPIDs;
			LowLevelPID *_lowPIDs;
			double *_fwdCouple;
			double *_invCouple;
			int *_stiffPID;
			double *_maxDAC;
			int _njoints;
//		int (*_p) (const char *fmt, ...);			/** printf-like function for spying messages */
	


	int ioPorts;
	Pid *_pids;                                  /** initial gains */

};


class yarp::dev::MEIMotionControl: 
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
            public ImplementPositionControl<MEIMotionControl, IPositionControl>,
            public ImplementVelocityControl<MEIMotionControl, IVelocityControl>,
            public ImplementPidControl<MEIMotionControl, IPidControl>,
            public ImplementEncoders<MEIMotionControl, IEncoders>,
            public ImplementControlCalibration<MEIMotionControl, IControlCalibration>,
            public ImplementAmplifierControl<MEIMotionControl, IAmplifierControl>,
            public ImplementControlLimits<MEIMotionControl, IControlLimits>
{
private:
    MEIMotionControl(const MEIMotionControl&);
	void operator=(const MEIMotionControl&);



public:
//private:
	bool implemented;

//	int _njoints;   
	int _dsp_rate;


			int16 **_filter_coeffs;

	int16 *_all_axes;

	int16 *_winding;			/// counting how many times the encoder wraps up.
	double *_16bit_oldpos;		/// storing the old 16 bit position value (encoder).
	double *_position_zero;		/// the zero reading of the encoders.








	/**
	 * Default constructor. Construction is done in two stages, first build the
     * object and then open the device driver.
	 */
    MEIMotionControl();

	/**
	 * Destructor.
	 */
	virtual ~MEIMotionControl();

	/**
	 * Open the device driver.
	 * @param par is the parameter structure 
	 * @return true/false on success/failure.
	 */ 
	virtual bool open(const MEIMotionControlParameters &par);

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



	
	/// PASA:
	/// the dev driver doesn't have any control on the calibration of encoders!

	inline int _sgn(double x) { return (x>0) ? 1 : -1; }
	inline int round(double x) { return ((x)>=0?(int)((x)+0.5):(int)((x)-0.5)); }





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
    virtual bool velocityMoveRaw(int j, double sp);
    virtual bool velocityMoveRaw(const double *sp);
	int safeVMove(double *spds, double *accs);
	int safeVMove(double *spds);
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
    virtual bool calibrateRaw(int j, double myzero);
//	virtual bool calibrateRaw(int axis, double *speeds, double *accs);
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
//    yarp::os::Semaphore _mutex;
//    yarp::os::Semaphore _done;


	bool _writerequested;
	bool _noreply;
    bool _opened;
	bool _alreadyinint;
	bool _positionmode;
	bool _amplifiers;
	bool _softwareLimits;
	bool _firsttime;

	
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
//	double *_command_speeds;	// used for velocity control.
	double *_ref_accs;			// for velocity control, in position min jerk eq is used.
	double *_ref_positions;		// for position control.	    
	double *_angleToEncoder;                    /** angle to encoder conversion factors from ESD to be checked */



	enum { MAX_SHORT = 32767, MIN_SHORT = -32768, MAX_INT = 0x7fffffff, MIN_INT = 0x80000000 };
	enum { MEI_CAN_SKIP_ADDR = 0x80 };

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
