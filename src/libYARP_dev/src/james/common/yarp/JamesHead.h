// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __YARPJAMES_HEAD__
#define __YARPJAMES_HEAD__

#include <yarp/dev/ControlBoardInterfaces.h>
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
            public IEncodersRaw,
            public DeviceDriver
{
public:
    void *controller;
    PolyDriver poly;
    PolyDriver ddInertia;
    PolyDriver ddHead;
    
    int nj;

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

    virtual bool stopRaw(int j);

    virtual bool stopRaw();
};

#endif 
