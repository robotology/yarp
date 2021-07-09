/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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

    bool getAxes(int *ax) override
    {return NOT_YET_IMPLEMENTED("getAxes");}

    bool resetEncoderRaw(int j) override
    {return NOT_YET_IMPLEMENTED("resetEncoderRaw");}

    bool resetEncodersRaw() override
    {return NOT_YET_IMPLEMENTED("resetEncodersRaw");}

    bool setEncoderRaw(int j, double val) override
    {return NOT_YET_IMPLEMENTED("setEncoderRaw");}

    bool setEncodersRaw(const double *vals) override
    {return NOT_YET_IMPLEMENTED("setEncodersRaw");}

    bool getEncoderRaw(int j, double *v) override
    {return NOT_YET_IMPLEMENTED("getEncoderRaw");}

    bool getEncodersRaw(double *encs) override
    {return NOT_YET_IMPLEMENTED("getEncodersRaw");}

    bool getEncoderSpeedRaw(int j, double *sp) override
    {return NOT_YET_IMPLEMENTED("getEncoderSpeedRaw");}

    bool getEncoderSpeedsRaw(double *spds) override
    {return NOT_YET_IMPLEMENTED("getEncoderSpeedsRaw");}

    bool getEncoderAccelerationRaw(int j, double *spds) override
    {return NOT_YET_IMPLEMENTED("getEncoderAccelerationRaw");}

    bool getEncoderAccelerationsRaw(double *accs) override
    {return NOT_YET_IMPLEMENTED("getEncoderAccelerationsRaw");}
};

#endif // YARP_DEV_IMPLEMENTCONTROLBOARDINTERFACES_H
