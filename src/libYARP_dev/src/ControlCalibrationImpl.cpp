// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>

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
            fprintf(stderr, "Going to call calibrator\n");
            ret=calibrator->calibrate(dynamic_cast<DeviceDriver *>(this));
        }
	else
		fprintf(stderr, "Warning called calibrate but no calibrator was set\n");

    return ret;
}

bool IControlCalibration::park(bool wait)
{
    bool ret=false;
    if (calibrator!=0)
    {
        fprintf(stderr, "Going to call calibrator\n");
        ret=calibrator->park(wait);
    }
   	else
		fprintf(stderr, "Warning called park but no calibrator was set\n");

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
            fprintf(stderr, "Going to call calibrator\n");
            ret=calibrator->calibrate(dynamic_cast<DeviceDriver *>(this));
        }
	else
		fprintf(stderr, "Warning called calibrate but no calibrator was set\n");

    return ret;
}

bool IControlCalibration2::park(bool wait)
{
    bool ret=false;
    if (calibrator!=0)
    {
        fprintf(stderr, "Going to call calibrator\n");
        ret=calibrator->park(wait);
    }
   	else
		fprintf(stderr, "Warning called park but no calibrator was set\n");
    
    return ret;
}
