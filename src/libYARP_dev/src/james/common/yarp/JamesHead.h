// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta, Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __YARPJAMES_HEAD__
#define __YARPJAMES_HEAD__

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/PolyDriver.h>

namespace yarp{
    namespace dev{
        class JamesHead;
    }};

/**
 * @ingroup dev_impl_wrapper
 *
 * 
 */
class yarp::dev::JamesHead: public IPositionControlRaw, 
            public IVelocityControlRaw,
            public IEncodersRaw,
            public IPidControlRaw,
            public IAmplifierControlRaw,
            public IControlLimitsRaw,
            public IControlCalibrationRaw,
            public ImplementPositionControl<JamesHead, IPositionControl>,
            public ImplementVelocityControl<JamesHead, IVelocityControl>,
            public ImplementEncoders<JamesHead, IEncoders>,
            public ImplementPidControl<JamesHead, IPidControl>,
            public ImplementAmplifierControl<JamesHead, IAmplifierControl>,
            public ImplementControlLimits<JamesHead, IControlLimits> ,
            public DeviceDriver
{
public:
    void *controller;
    PolyDriver poly;
    PolyDriver ddInertia;
    PolyDriver ddHead;
    
    int nj;
    int *axisMap;
    double *angleToEncoder;
    double *zeros;

    JamesHead();
    ~JamesHead();

    virtual bool open(yarp::os::Searchable& config);

    virtual bool close();

    virtual bool getAxes(int *ax);

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

    virtual bool setPositionMode();

    virtual bool positionMoveRaw(int j, double ref);

    virtual bool positionMoveRaw(const double *refs);

    virtual bool relativeMoveRaw(int j, double delta);

    virtual bool relativeMoveRaw(const double *deltas);
    
    virtual bool checkMotionDoneRaw(int j, bool *flag);

    virtual bool checkMotionDoneRaw(bool *flag);

    virtual bool setRefSpeedRaw(int j, double sp);

    virtual bool setRefSpeedsRaw(const double *spds);

    virtual bool setRefAccelerationRaw(int j, double acc);

    virtual bool setRefAccelerationsRaw(const double *accs);

    virtual bool getRefSpeedRaw(int j, double *ref);

    virtual bool getRefSpeedsRaw(double *spds);

    virtual bool getRefAccelerationRaw(int j, double *acc);

    virtual bool getRefAccelerationsRaw(double *accs);

    virtual bool setVelocityMode()
    {return true;}

    virtual bool velocityMoveRaw(int j, double v);
    virtual bool velocityMoveRaw(const double *v);
    
    virtual bool stopRaw(int j);

    virtual bool stopRaw();

    virtual bool setPidRaw(int j, const Pid &pid)
    {return true;}

    virtual bool setPidsRaw(const Pid *pids)
    {return true;}

    virtual bool setReferenceRaw(int j, double ref)
    {return true;}

    virtual bool setReferencesRaw(const double *refs)
    {return true;}

    virtual bool setErrorLimitRaw(int j, double limit)
    {return true;}

    virtual bool setErrorLimitsRaw(const double *limits)
    {return true;}

    virtual bool getErrorRaw(int j, double *err)
    {return true;}

    virtual bool getErrorsRaw(double *errs)
    {return true;}

    virtual bool getOutputRaw(int j, double *out)
    {return true;}

    virtual bool getOutputsRaw(double *outs)
    {return true;}

    virtual bool getPidRaw(int j, Pid *pid)
    {return true;}

    virtual bool getPidsRaw(Pid *pids)
    {return true;}

    virtual bool getReferenceRaw(int j, double *ref)
    {return true;}

    virtual bool getReferencesRaw(double *refs)
    {return true;}

    virtual bool getErrorLimitRaw(int j, double *limit)
    {return true;}

    virtual bool getErrorLimitsRaw(double *limits)
    {return true;}

    virtual bool resetPidRaw(int j)
    {return true;}

    virtual bool setOffsetRaw(int j, double v)
    {return true;}

    virtual bool disablePidRaw(int j);

    virtual bool enablePidRaw(int j);

    virtual bool enableAmpRaw(int j);

    virtual bool disableAmpRaw(int j);

    virtual bool getCurrentsRaw(double *vals)
    {return true;}

    virtual bool getCurrentRaw(int j, double *val)
    {return true;}

    virtual bool setMaxCurrentRaw(int j, double v)
    {return true;}

    virtual bool getAmpStatusRaw(int *st)
    {return true;}

    virtual bool setLimitsRaw(int axis, double min, double max)
    {return true;}
    
    virtual bool getLimitsRaw(int axis, double *min, double *max)
    {return true;}

    virtual bool calibrateRaw(int j, double p)
    {return true;}

    virtual bool doneRaw(int j)
    {return true;}

};

#endif 
