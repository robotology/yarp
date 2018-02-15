/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Log.h>

#include <cstdio>

using namespace yarp::dev;

IControlCalibration::IControlCalibration()
{
    calibrator=nullptr;
}

bool IControlCalibration::setCalibrator(ICalibrator *c)
{
    if (c!=nullptr)
    {
        calibrator=c;
        return true;
    }

    return false;
}

bool IControlCalibration::calibrate()
{
    bool ret = false;
    if (calibrator!=nullptr)
    {
        yDebug("Going to call calibrator\n");
        ret=calibrator->calibrate(dynamic_cast<DeviceDriver *>(this));
    }
    else
        yWarning("Warning called calibrate but no calibrator was set\n");

    return ret;
}

bool IControlCalibration::park(bool wait)
{
    bool ret=false;
    if (calibrator!=nullptr)
    {
        yDebug("Going to call calibrator\n");
        ret=calibrator->park(dynamic_cast<DeviceDriver *>(this), wait);
    }
    else
        yWarning("Warning called park but no calibrator was set\n");

    return ret;
}


/// IControlCalibration2
IControlCalibration2::IControlCalibration2()
{
    calibrator=nullptr;
}

bool IControlCalibration2::setCalibrator(ICalibrator *c)
{
    if (c!=nullptr)
    {
        calibrator=c;
        return true;
    }

    return false;
}

bool IControlCalibration2::calibrate()
{
    bool ret = false;
    if (calibrator!=nullptr)
    {
        yDebug("Going to call calibrator\n");
        ret=calibrator->calibrate(dynamic_cast<DeviceDriver *>(this));
    }
    else
        yWarning("Warning called calibrate but no calibrator was set\n");

    return ret;
}

bool IControlCalibration2::abortCalibration()
{
    bool ret=false;
    if (calibrator!=nullptr)
        ret=calibrator->quitCalibrate();
    return ret;
}

bool IControlCalibration2::abortPark()
{
    bool ret=false;
    if (calibrator!=nullptr)
        ret=calibrator->quitPark();
    return ret;
}

bool IControlCalibration2::park(bool wait)
{
    bool ret=false;
    if (calibrator!=nullptr)
    {
        yDebug("Going to call calibrator\n");
        ret=calibrator->park(dynamic_cast<DeviceDriver *>(this), wait);
    }
    else
        yWarning("Warning called park but no calibrator was set\n");

    return ret;
}
