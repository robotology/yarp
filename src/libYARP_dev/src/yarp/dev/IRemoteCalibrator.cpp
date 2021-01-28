/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/IRemoteCalibrator.h>

using namespace yarp::dev;

/**
 * This interface is meant to remotize the access of the calibration device
 * in order to allow users to remotely call the calibration procedure for
 * a single joint or the whole device and let the calibrator do the job.
 * Main difference between this interface and remotizing the iControlCalibration
 * interfaces is that in this case we are calling a proper iCalibrator device
 * that knows the correct procedure and the correct parameters for the calibration,
 * so that user doesn't have to worry about configuration parameters.
 */

IRemoteCalibrator::IRemoteCalibrator()
{
    _remoteCalibrator = nullptr;
}

bool IRemoteCalibrator::setCalibratorDevice(yarp::dev::IRemoteCalibrator *dev)
{
    _remoteCalibrator = dev;
    return true;
}

IRemoteCalibrator *IRemoteCalibrator::getCalibratorDevice()
{
    return _remoteCalibrator;
}

bool IRemoteCalibrator::isCalibratorDevicePresent(bool *isCalib)
{
    _remoteCalibrator == nullptr ? *isCalib = false : *isCalib = true;
    return true;
}

void IRemoteCalibrator::releaseCalibratorDevice()
{
    _remoteCalibrator = nullptr;
}
