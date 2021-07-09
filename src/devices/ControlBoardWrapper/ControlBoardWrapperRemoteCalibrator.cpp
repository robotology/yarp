/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperRemoteCalibrator.h"

#include <yarp/os/LogStream.h>

#include "ControlBoardLogComponent.h"

using yarp::dev::IRemoteCalibrator;

IRemoteCalibrator* ControlBoardWrapperRemoteCalibrator::getCalibratorDevice()
{
    yCTrace(CONTROLBOARD);
    return yarp::dev::IRemoteCalibrator::getCalibratorDevice();
}

bool ControlBoardWrapperRemoteCalibrator::isCalibratorDevicePresent(bool* isCalib)
{
    yCTrace(CONTROLBOARD);
    return yarp::dev::IRemoteCalibrator::isCalibratorDevicePresent(isCalib);
}

bool ControlBoardWrapperRemoteCalibrator::calibrateSingleJoint(int j)
{
    yCTrace(CONTROLBOARD);
    if (!getCalibratorDevice()) {
        return false;
    }

    return IRemoteCalibrator::getCalibratorDevice()->calibrateSingleJoint(j);
}

bool ControlBoardWrapperRemoteCalibrator::calibrateWholePart()
{
    yCTrace(CONTROLBOARD);
    if (!getCalibratorDevice()) {
        return false;
    }

    return getCalibratorDevice()->calibrateWholePart();
}

bool ControlBoardWrapperRemoteCalibrator::homingSingleJoint(int j)
{
    yCTrace(CONTROLBOARD);
    if (!getCalibratorDevice()) {
        return false;
    }

    return getCalibratorDevice()->homingSingleJoint(j);
}

bool ControlBoardWrapperRemoteCalibrator::homingWholePart()
{
    yCTrace(CONTROLBOARD);
    if (!getCalibratorDevice()) {
        return false;
    }

    return getCalibratorDevice()->homingWholePart();
}

bool ControlBoardWrapperRemoteCalibrator::parkSingleJoint(int j, bool _wait)
{
    yCTrace(CONTROLBOARD);
    if (!getCalibratorDevice()) {
        return false;
    }

    return getCalibratorDevice()->parkSingleJoint(j, _wait);
}

bool ControlBoardWrapperRemoteCalibrator::parkWholePart()
{
    yCTrace(CONTROLBOARD);
    if (!getCalibratorDevice()) {
        return false;
    }

    return getCalibratorDevice()->parkWholePart();
}

bool ControlBoardWrapperRemoteCalibrator::quitCalibrate()
{
    yCTrace(CONTROLBOARD);
    if (!getCalibratorDevice()) {
        return false;
    }

    return getCalibratorDevice()->quitCalibrate();
}

bool ControlBoardWrapperRemoteCalibrator::quitPark()
{
    yCTrace(CONTROLBOARD);
    if (!getCalibratorDevice()) {
        return false;
    }

    return getCalibratorDevice()->quitPark();
}
