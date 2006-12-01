// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
/*
 * Copyright (C) 2006 Assif Mirza
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */
#ifndef __KASPAR_CONTROL_DEVICE_H__
#define __KASPAR_CONTROL_DEVICE_H__

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>

#include "Servo.h"

namespace yarp {
    namespace dev {
        class DimaxU2C;
    }
}

/**
 * @ingroup dev_impl
 *
 * A basic driver for dimax u2c 12
 *
 * @author Assif Mirza
 *
 */
class yarp::dev::DimaxU2C : 
	public yarp::dev::DeviceDriver ,
	public yarp::dev::IPositionControlRaw,
  public yarp::dev::ImplementPositionControl<DimaxU2C, yarp::dev::IPositionControl>
{
public:
    DimaxU2C();
    ~DimaxU2C();

    // methods to implement DeviceDriver Interface
    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    // virtual methods overriding implemented IPositionControlRaw Interface
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
    /////////////////////////////// END Position Control Raw INTERFACE
private:
	Servo *servos;
	int numJoints;
	double *speeds;
	double *accels;

};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_dimax_u2c dimax_u2c
 *
 *  A motor driver, see yarp::dev::DimaxU2C.
 *
 */


#endif
