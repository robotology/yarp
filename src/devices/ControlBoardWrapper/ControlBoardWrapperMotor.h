/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERMOTOR_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERMOTOR_H

#include <yarp/dev/IMotor.h>

#include "ControlBoardWrapperCommon.h"

class ControlBoardWrapperMotor :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IMotor
{
public:
    inline bool getNumberOfMotors(int* num) override { return ControlBoardWrapperCommon::getNumberOfMotors(num); }
    bool getTemperature(int m, double* val) override;
    bool getTemperatures(double* vals) override;
    bool getTemperatureLimit(int m, double* val) override;
    bool setTemperatureLimit(int m, const double val) override;
    bool getGearboxRatio(int m, double* val) override;
    bool setGearboxRatio(int m, const double val) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERMOTOR_H
