/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperRemoteCalibrator.h"

#include <yarp/os/LogStream.h>

#include "ControlBoardWrapperLogComponent.h"

using yarp::dev::IRemoteCalibrator;

IRemoteCalibrator* ControlBoardWrapperRemoteCalibrator::getCalibratorDevice()
{
    yCTrace(CONTROLBOARDWRAPPER);
    return yarp::dev::IRemoteCalibrator::getCalibratorDevice();
}

bool ControlBoardWrapperRemoteCalibrator::isCalibratorDevicePresent(bool* isCalib)
{
    yCTrace(CONTROLBOARDWRAPPER);
    return yarp::dev::IRemoteCalibrator::isCalibratorDevicePresent(isCalib);
}

bool ControlBoardWrapperRemoteCalibrator::calibrateSingleJoint(int j)
{
    yCTrace(CONTROLBOARDWRAPPER);
    if (!getCalibratorDevice()) {
        return false;
    }

    return IRemoteCalibrator::getCalibratorDevice()->calibrateSingleJoint(j);
}

bool ControlBoardWrapperRemoteCalibrator::calibrateWholePart()
{
    yCTrace(CONTROLBOARDWRAPPER);
    if (!getCalibratorDevice()) {
        return false;
    }

    return getCalibratorDevice()->calibrateWholePart();
}

bool ControlBoardWrapperRemoteCalibrator::homingSingleJoint(int j)
{
    yCTrace(CONTROLBOARDWRAPPER);
    if (!getCalibratorDevice()) {
        return false;
    }

    return getCalibratorDevice()->homingSingleJoint(j);
}

bool ControlBoardWrapperRemoteCalibrator::homingWholePart()
{
    yCTrace(CONTROLBOARDWRAPPER);
    if (!getCalibratorDevice()) {
        return false;
    }

    return getCalibratorDevice()->homingWholePart();
}

bool ControlBoardWrapperRemoteCalibrator::parkSingleJoint(int j, bool _wait)
{
    yCTrace(CONTROLBOARDWRAPPER);
    if (!getCalibratorDevice()) {
        return false;
    }

    return getCalibratorDevice()->parkSingleJoint(j, _wait);
}

bool ControlBoardWrapperRemoteCalibrator::parkWholePart()
{
    yCTrace(CONTROLBOARDWRAPPER);
    if (!getCalibratorDevice()) {
        return false;
    }

    return getCalibratorDevice()->parkWholePart();
}

bool ControlBoardWrapperRemoteCalibrator::quitCalibrate()
{
    yCTrace(CONTROLBOARDWRAPPER);
    if (!getCalibratorDevice()) {
        return false;
    }

    return getCalibratorDevice()->quitCalibrate();
}

bool ControlBoardWrapperRemoteCalibrator::quitPark()
{
    yCTrace(CONTROLBOARDWRAPPER);
    if (!getCalibratorDevice()) {
        return false;
    }

    return getCalibratorDevice()->quitPark();
}
