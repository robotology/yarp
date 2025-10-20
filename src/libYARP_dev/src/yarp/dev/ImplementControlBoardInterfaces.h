/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTCONTROLBOARDINTERFACES_H
#define YARP_DEV_IMPLEMENTCONTROLBOARDINTERFACES_H

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ImplementControlCalibration.h>
#include <yarp/dev/api.h>

namespace yarp::dev {
class StubImplPositionControlRaw;
class StubImplEncodersRaw;
}

/**
 * Stub implementation of IEncodersRaw interface.
 * Inherit from this class if you want a stub implementation
 * of methods in IPositionControlRaw. This class allows to
 * gradually implement an interface; you just have to implement
 * functions that are useful for the underlying device.
 * Another way to see this class is as a means to convert
 * compile time errors in runtime errors.
 *
 * If you use this class please be aware that the device
 * you are wrapping might not function properly because you
 * missed to implement useful functionalities.
 *
 */
class YARP_dev_API yarp::dev::StubImplEncodersRaw: public IEncodersRaw
{
public:
    virtual ~StubImplEncodersRaw(){}

    yarp::dev::ReturnValue getAxes(int *ax) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue resetEncoderRaw(int j) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue resetEncodersRaw() override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue setEncoderRaw(int j, double val) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue setEncodersRaw(const double *vals) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getEncoderRaw(int j, double *v) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getEncodersRaw(double *encs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getEncoderSpeedRaw(int j, double *sp) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getEncoderSpeedsRaw(double *spds) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getEncoderAccelerationRaw(int j, double *spds) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}

    yarp::dev::ReturnValue getEncoderAccelerationsRaw(double *accs) override
    {return YARP_METHOD_NOT_YET_IMPLEMENTED();}
};

#endif // YARP_DEV_IMPLEMENTCONTROLBOARDINTERFACES_H
