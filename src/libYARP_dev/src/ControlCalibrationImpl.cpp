// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>

using namespace yarp::dev;

bool IControlCalibrationRaw::setCalibrator(ICalibrator *c)
{
    if (c!=0)
    {
        calibrator=c;
        return true;
    }

    return false;
}

IControlCalibrationRaw::IControlCalibrationRaw()
{
    calibrator=0;
}

bool IControlCalibrationRaw::calibrate()
{
    bool ret = false;

    if (calibrator!=0)
        ret=calibrator->calibrate(dynamic_cast<DeviceDriver *>(this));

    return ret;
}
