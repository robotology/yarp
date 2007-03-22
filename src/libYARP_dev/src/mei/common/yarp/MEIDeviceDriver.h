/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

//
//
// Wrapper for the MEI motor board
//
// April 2003 -- by nat
//
// win32: link Medvc60f.lib or later versions.
//
// $Id: MEIDeviceDriver.h,v 1.1 2007-03-22 14:08:24 claudio72 Exp $

#ifndef _MEI_DEVICE_DRIVER__
#define _MEI_DEVICE_DRIVER__

//#include <yarp/YARPConfig.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>


#include "../../../ControlBoardInterfacesImpl.inl"
//#include <yarp/YARPControlBoardUtils.h>
//#include <yarp/YARPSemaphore.h>

#include <stdlib.h>
#include <string.h>


namespace yarp {
    namespace dev {
        class MEIDeviceDriver;
    }
}


//
//
typedef short int16;

//
// constants - depend on the card type.
const int MAX_PORTS = 2;

struct MEIOpenParameters
{
	MEIOpenParameters()
	{
		hwnd = 0;
		nj = 8;
		meiPortAddr = 0x300;
		ioPorts = 0x00;
	}

	void *hwnd;
	int nj;
	int meiPortAddr;
	int ioPorts;		// bit 0 = 1 means port 0 -> output, and so on.
};

// single axis params
struct SingleAxisParameters
{
	int axis;
	void *parameters;
};

struct IOParameters
{
	int port;
	short value;
};


enum ControlBoardEvents
{
CBNoEvent = 0,
CBStopEvent = 1,
CBEStopEvent = 2,
CBAbortEvent = 3,
CBNEvents = 4 // required ! tells the total number of events
};




enum ControlBoardHomeConfig
{
	CBHomeOnly = 0,
	CBLowHomeAndIndex = 1,
	CBIndexOnly = 2,
	CBHighHomeAndIndex = 3,
	CBNHomeConfig = 4	// required for future use ?
};

struct LowLevelPID
{
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
};
///
///
///
class yarp::dev::MEIDeviceDriver: /*
public IPositionControlRaw, 
            public IVelocityControlRaw,
            public IEncodersRaw,
            public IPidControlRaw,
            public IAmplifierControlRaw,
            public IControlLimitsRaw,
            public IControlCalibrationRaw,
            public ImplementPositionControl<MEIDeviceDriver, IPositionControl>,
            public ImplementVelocityControl<MEIDeviceDriver, IVelocityControl>,
            public ImplementPidControl<MEIDeviceDriver, IPidControl>,
            public ImplementEncoders<MEIDeviceDriver, IEncoders>,
            public ImplementControlCalibration<MEIDeviceDriver, IControlCalibration>,
            public ImplementAmplifierControl<MEIDeviceDriver, IAmplifierControl>,
            public ImplementControlLimits<MEIDeviceDriver, IControlLimits>,
   */         public DeviceDriver
{
public:
	MEIDeviceDriver();
	~MEIDeviceDriver();

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
	virtual bool close();

private:
	//  functions
	int setSpeed(void *sp);
	int setPosition(void *pos);
//	int setPosition(int axis);		//Mattia

	int setAcceleration(void *acc);
	int setPid(void *cmd);
	int getPosition(void *j);

	int setOutputPort(void *cmd);
	int getOutputPort(void *cmd);
	int setOutputBit(void *bit);
	int clearOutputBit(void *bit);

	int setSpeeds(void *spds);
	int setPositions(void *pos);
	int setAccelerations(void *acc);
	int getPositions(void *j);
	int setOffsets(void *offs);
	int setOffset(void *cmd);
	int vMove(void *spds);
	int safeVMove(void *spds);

	int definePositions(void *pos);
//	int definePosition(int16 axis, double pos);	//Mattia
	int definePosition(void *cmd);

	int stopAxes(void *p);
	int readSwitches(void *p);

	int getSpeeds(void *sps);
	
	int getRefSpeeds(void *sps);
	int getRefAccelerations(void *accs);
	int getRefPositions(void *pos);
	int getPid(void *par);

	int getTorques(void *trqs);

	int getTorqueLimit(void *cmd);
	int getTorqueLimits(void *trqs);

	int setTorqueLimit(void *cmd);
	int setTorqueLimits(void *trqs);
	int getErrors(void *errs);
	int setIntLimits(void *lmts);
	int setIntLimit(void *cmd);

	int readInput(void *input);
	
	int initPortAsInput(void *p);
	int initPortAsOutput(void *p);
	int setAmpEnableLevel(void *cmd);
	int setAmpEnable(void *cmd);

	int disableAmp(void *axis);
	int enableAmp(void *axis);

	int controllerRun(void *axis);
	int controllerIdle(void *axis);
	int clearStop(void *axis);
	int setStopRate(void *axis);

	int setPositiveLimit(void *cmd);
	int setNegativeLimit(void *cmd);
	int setPositiveLevel(void *cmd);
	int setNegativeLevel(void *cmd);
	
	int setCommands(void *cmd);
	int setCommand(void *cmd);

	int setHomeIndexConfig(void *cmd);
	int setHomeLevel(void *cmd);
	int setHome(void *cmd);
	int setHomeProcedure(void *cmd);

	// MOTION STATUS
	// non blocking call, check motion done and return
	int checkMotionDone(void *cmd);

	// blocking call, poll with sleep(time)
	// time is ms, 0: busy waiting
	int waitForMotionDone(void *cmd);
	int checkFramesLeft(void *cmd);
	int waitForFramesLeft(void *cmd);

	// analog input
	int readAnalog(void *cmd);
	int setAxisAnalog(void *cmd);
	
	int dummy(void *d);	// dummy function, for debug purpose


////////////////////////////
/*


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

//////////////////////////////
*/
private:
	bool implemented;
	int _njoints;   
	int _dsp_rate;

	double *_ref_speeds;
	double *_ref_accs;
	double *_ref_positions;
	int16 **_filter_coeffs;
		
	int16 *_all_axes;
	int *_events;

	int16 *_winding;			/// counting how many times the encoder wraps up.
	double *_16bit_oldpos;		/// storing the old 16 bit position value (encoder).
	double *_position_zero;		/// the zero reading of the encoders.

	/// PASA:
	/// the dev driver doesn't have any control on the calibration of encoders!

	inline int _sgn(double x) { return (x>0) ? 1 : -1; }
	inline int round(double x) { return ((x)>=0?(int)((x)+0.5):(int)((x)-0.5)); }
};

#endif // .h