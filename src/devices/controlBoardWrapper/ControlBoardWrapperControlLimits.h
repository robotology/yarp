/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCONTROLLIMITS_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCONTROLLIMITS_H

#include <yarp/dev/IControlLimits.h>

#include "ControlBoardWrapperCommon.h"

class ControlBoardWrapperControlLimits :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IControlLimits
{
public:
    bool setLimits(int j, double min, double max) override;
    bool getLimits(int j, double* min, double* max) override;
    bool setVelLimits(int j, double min, double max) override;
    bool getVelLimits(int j, double* min, double* max) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCONTROLLIMITS_H
