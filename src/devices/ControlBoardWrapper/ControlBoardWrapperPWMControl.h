/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPWMCONTROL_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPWMCONTROL_H

#include <yarp/dev/IPWMControl.h>

#include "ControlBoardWrapperCommon.h"


class ControlBoardWrapperPWMControl :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IPWMControl
{
public:
    inline bool getNumberOfMotors(int* num) override { return ControlBoardWrapperCommon::getNumberOfMotors(num); }
    bool setRefDutyCycle(int j, double v) override;
    bool setRefDutyCycles(const double* v) override;
    bool getRefDutyCycle(int j, double* v) override;
    bool getRefDutyCycles(double* v) override;
    bool getDutyCycle(int j, double* v) override;
    bool getDutyCycles(double* v) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPWMCONTROL_H
