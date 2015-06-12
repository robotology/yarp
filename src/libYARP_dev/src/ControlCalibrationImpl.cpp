// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Lorenzo Natale.
* Copyright (C) 2006 The Robotcub consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/



#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Log.h>

#include <stdio.h>

using namespace yarp::dev;

IControlCalibration::IControlCalibration()
{
    calibrator=0;
}

bool IControlCalibration::setCalibrator(ICalibrator *c)
{
    if (c!=0)
    {
        calibrator=c;
        return true;
    }

    return false;
}

bool IControlCalibration::calibrate()
{
    bool ret = false;
    if (calibrator!=0)
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
    if (calibrator!=0)
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
    calibrator=0;
}

bool IControlCalibration2::setCalibrator(ICalibrator *c)
{
    if (c!=0)
    {
        calibrator=c;
        return true;
    }

    return false;
}

bool IControlCalibration2::calibrate()
{
    bool ret = false;
    if (calibrator!=0)
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
    if (calibrator!=0)
        ret=calibrator->quitCalibrate();
    return ret;
}

bool IControlCalibration2::abortPark()
{
    bool ret=false;
    if (calibrator!=0)
        ret=calibrator->quitPark();
    return ret;
}

bool IControlCalibration2::park(bool wait)
{
    bool ret=false;
    if (calibrator!=0)
    {
        yDebug("Going to call calibrator\n");
        ret=calibrator->park(dynamic_cast<DeviceDriver *>(this), wait);
    }
    else
        yWarning("Warning called park but no calibrator was set\n");

    return ret;
}
