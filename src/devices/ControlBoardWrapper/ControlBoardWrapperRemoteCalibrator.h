/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERREMOTECALIBRATOR_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERREMOTECALIBRATOR_H

#include <yarp/dev/IRemoteCalibrator.h>

#include "ControlBoardWrapperCommon.h"


class ControlBoardWrapperRemoteCalibrator :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IRemoteCalibrator
{
public:
    bool isCalibratorDevicePresent(bool* isCalib) override;
    yarp::dev::IRemoteCalibrator* getCalibratorDevice() override;
    bool calibrateSingleJoint(int j) override;
    bool calibrateWholePart() override;
    bool homingSingleJoint(int j) override;
    bool homingWholePart() override;
    bool parkSingleJoint(int j, bool _wait = true) override;
    bool parkWholePart() override;
    bool quitCalibrate() override;
    bool quitPark() override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERREMOTECALIBRATOR_H
