// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 20014 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/CalibratorInterfaces.h>

using namespace yarp::dev;

/**
 * This interface is meant to remotize the access of the calibration device
 * in order to allow users to remotely call the calibration procedure for
 * a single joint or the whole device and let the calibrator do the job.
 * Main difference between this interface and remotizing the iControlCalibration
 * interfaces is that in this case we are calling a proper iCalibrator device
 * (whose interface is described above) that knows the correct procedure and
 * the correct parameters for the calibration, so that user doesn't have to worry
 * about configuration.
 * This is meant to be used by network wrapper devices
 */

IRemoteCalibrator::IRemoteCalibrator()
{
    _remoteCalibrator = 0;
}

bool IRemoteCalibrator::setCalibratorDevice(yarp::dev::IRemoteCalibrator *dev)
{
    if(dev!=NULL)
    {
        _remoteCalibrator = dev;
        return true;
    }
    else
        return false;
}

IRemoteCalibrator *IRemoteCalibrator::getCalibratorDevice()
{
    return _remoteCalibrator;
}

