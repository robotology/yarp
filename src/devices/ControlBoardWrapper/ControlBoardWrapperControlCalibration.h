/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCONTROLCALIBRATION_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCONTROLCALIBRATION_H

#include <yarp/dev/IControlCalibration.h>

#include "ControlBoardWrapperCommon.h"

class ControlBoardWrapperControlCalibration :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IControlCalibration
{
public:
    bool calibrateAxisWithParams(int j, unsigned int ui, double v1, double v2, double v3) override;
    bool setCalibrationParameters(int j, const yarp::dev::CalibrationParameters& params) override;
    bool calibrationDone(int j) override;
    bool abortPark() override;
    bool abortCalibration() override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCONTROLCALIBRATION_H
