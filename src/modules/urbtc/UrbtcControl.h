 /*
 * Copyright (C) 2007 Jonas Ruesch
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef URBTCCONTROL
#define URBTCCONTROL

#include <unistd.h>         /* for read, write, close */
#include <sys/ioctl.h>      /* for ioctl */
#include <sys/types.h>      /* for open */
#include <sys/stat.h>       /* for open */
#include <fcntl.h>          /* for open */

#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Runnable.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardPid.h>
#include <yarp/os/Bottle.h>
        

#include "../iMCs01/driver/urbtc.h"
#include "../iMCs01/driver/urobotc.h"

namespace yarp{
    namespace dev{
        class UrbtcControl;
    }
}

using namespace std;
using namespace yarp::os;

/**
 * @ingroup dev_impl_motor
 *
 * Device driver implementing some controlboard interfaces for the urbtc iMCs01 controlboard from http://www.ixs.co.jp/en.\n
 * \n
 * Please note, encoder and position values have internally a range of short int only. For this reason
 * absolute (raw) position control works only within the range of -32768 to +32767. Currently all movements (including velocityMove)
 * are bounded to this range.\n
 * 
 */

class yarp::dev::UrbtcControl :  public IPositionControlRaw,
                                 public ImplementPositionControl<UrbtcControl, IPositionControl>,
                                 public IVelocityControlRaw,
                                 public ImplementVelocityControl<UrbtcControl, IVelocityControl>,
                                 public IPidControlRaw,
                                 public ImplementPidControl<UrbtcControl, IPidControl>,
                                 public IEncodersRaw,
                                 public ImplementEncoders<UrbtcControl, IEncoders>,
                                 public IControlLimitsRaw,
                                 public ImplementControlLimits<UrbtcControl, IControlLimits>,
                                 public DeviceDriver,
                                 public yarp::os::Thread
{

protected:

    int                     _intNumControllers;     // number of encountered controllers (urbtc devicefiles)
    unsigned short          _intOffset;             // the initial offset of 32768 (without this motor will change position when initialized)
    int                     *_fd;                   // array of devicefile descriptors
    char                   **_deviceFilename;      // array of devicefilenames
    ccmd                    *_cmd;                  // array of ccmd structs (urbtc driver)
    uout                    *_out;                  // array of uout structs (urbtc driver)
    uin                     *_in;                   // array of uin structs (urbtc driver);

    char                    *_devicePrefix;
    yarp::dev::Pid          _pidPosDefault;         // default pid values for position control 
    yarp::dev::Pid          _pidVelDefault;         // default pid values for velocity control 
    double					_pidAccuracy;           // to convert from nominator/denominator representation to 
    double                  _dblFactDeg2Raw;         // this times radian value equals encoder value (without gear!)
    int						_intLimitDefault;		// +/- defalt limits

	int*					_limitsMax;				// upper limits in encoder tics
	int*					_limitsMin;				// lower limits in encoder tics
	bool*					_limitsExceeding;		// flags for limit exceedings

    int                     getNumberOfDevices();
    void                    setDeviceFilenames(int num);

    long                    *_pos;                  // summed encoder/position values for each axis
    long                    *_posOld;               // old summed encoder/positon values (t-1)
    long                    *_posDesired;           // position specified by the user
    int                     *_periodCounter;        // counter for encoder period (one period from -32768 to 32767)
    int                     _periodTicks;           // how many ticks a period has; (e.g.65535)
    int                     _periodTicksHalf;       // half of the period Ticks (use for +/-) (-32768 to 32767)
    int                     _posAccuracy;           // how accurate should checkMotionDoneRaw behave?

    int                     _threadRate;            // milliseconds
    yarp::os::Semaphore     _mutex;
    
    // Thread
    virtual void run();

    short                   getUrbtcEncoderValue(int axis);

    //bool                    velocityMoveClose(int axis, long position, unsigned short speed, long tolerance);
    //bool                    positionMoveClose(int axis, long position, unsigned short speed, long tolerance);

public:

    UrbtcControl();
    virtual ~UrbtcControl();

    


    // IPositionControlRaw
    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();
    virtual bool getAxes(int *ax) ;
    virtual bool setPositionMode();
    virtual bool positionMoveRaw(int j, double ref) ;
    virtual bool positionMoveRaw(const double *refs) ;
    /**
    * Caution, delta must be << 32768
    */
    virtual bool relativeMoveRaw(int j, double delta) ;
    /**
    * Caution, delta must be << 32768
    */
    virtual bool relativeMoveRaw(const double *deltas) ;
    virtual bool checkMotionDoneRaw(int j, bool *flag) ;
    virtual bool checkMotionDoneRaw(bool *flag) ;
    virtual bool setRefSpeedRaw(int j, double sp) ;
    virtual bool setRefSpeedsRaw(const double *spds) ;
    virtual bool setRefAccelerationRaw(int j, double acc) ;
    virtual bool setRefAccelerationsRaw(const double *accs) ;
    virtual bool getRefSpeedRaw(int j, double *ref) ;
    virtual bool getRefSpeedsRaw(double *spds) ;
    virtual bool getRefAccelerationRaw(int j, double *acc) ;
    virtual bool getRefAccelerationsRaw(double *accs) ;
    virtual bool stopRaw(int j) ;
    virtual bool stopRaw() ;

    // IVelocityControlRaw
    virtual bool setVelocityMode();
    virtual bool velocityMoveRaw(int j, double sp);
    virtual bool velocityMoveRaw(const double *sp);

    // IPidControlRaw
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

    // IEncodersRaw
    virtual bool resetEncoderRaw(int j);
    virtual bool resetEncodersRaw() ;
    virtual bool setEncoderRaw(int j, double val) ;
    virtual bool setEncodersRaw(const double *vals) ;
    virtual bool getEncoderRaw(int j, double *v);
    virtual bool getEncodersRaw(double *encs) ;
    virtual bool getEncoderSpeedRaw(int j, double *sp) ;
    virtual bool getEncoderSpeedsRaw(double *spds) ;
    virtual bool getEncoderAccelerationRaw(int j, double *spds) ;
    virtual bool getEncoderAccelerationsRaw(double *accs) ;

    // IControlLimits
    virtual bool setLimitsRaw (int axis, double min, double max);
    virtual bool getLimitsRaw (int axis, double *min, double *max);

};

#endif 
 
