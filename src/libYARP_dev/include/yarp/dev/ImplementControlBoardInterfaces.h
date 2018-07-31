/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IMPLEMENTCONTROLBOARDINTERFACES_H
#define YARP_DEV_IMPLEMENTCONTROLBOARDINTERFACES_H

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ImplementControlCalibration.h>
#include <yarp/dev/api.h>

namespace yarp
{
    namespace dev
    {
        class StubImplPositionControlRaw;
        class StubImplEncodersRaw;
    }
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
private:
    /**
     * Helper for printing error message, see below.
     * Implemented in ControlBoardInterfacesImpl.cpp.
     */
    bool NOT_YET_IMPLEMENTED(const char *func=0);

public:
    virtual ~StubImplEncodersRaw(){}

    virtual bool getAxes(int *ax) override
    {return NOT_YET_IMPLEMENTED("getAxes");}

    virtual bool resetEncoderRaw(int j) override
    {return NOT_YET_IMPLEMENTED("resetEncoderRaw");}

    virtual bool resetEncodersRaw() override
    {return NOT_YET_IMPLEMENTED("resetEncodersRaw");}

    virtual bool setEncoderRaw(int j, double val) override
    {return NOT_YET_IMPLEMENTED("setEncoderRaw");}

    virtual bool setEncodersRaw(const double *vals) override
    {return NOT_YET_IMPLEMENTED("setEncodersRaw");}

    virtual bool getEncoderRaw(int j, double *v) override
    {return NOT_YET_IMPLEMENTED("getEncoderRaw");}

    virtual bool getEncodersRaw(double *encs) override
    {return NOT_YET_IMPLEMENTED("getEncodersRaw");}

    virtual bool getEncoderSpeedRaw(int j, double *sp) override
    {return NOT_YET_IMPLEMENTED("getEncoderSpeedRaw");}

    virtual bool getEncoderSpeedsRaw(double *spds) override
    {return NOT_YET_IMPLEMENTED("getEncoderSpeedsRaw");}

    virtual bool getEncoderAccelerationRaw(int j, double *spds) override
    {return NOT_YET_IMPLEMENTED("getEncoderAccelerationRaw");}

    virtual bool getEncoderAccelerationsRaw(double *accs) override
    {return NOT_YET_IMPLEMENTED("getEncoderAccelerationsRaw");}
};

#endif // YARP_DEV_IMPLEMENTCONTROLBOARDINTERFACES_H
