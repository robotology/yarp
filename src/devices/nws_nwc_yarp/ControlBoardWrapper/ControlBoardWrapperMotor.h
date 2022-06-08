/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
