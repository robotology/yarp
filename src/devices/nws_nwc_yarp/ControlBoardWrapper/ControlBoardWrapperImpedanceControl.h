/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERIMPEDANCECONTROL_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERIMPEDANCECONTROL_H

#include <yarp/dev/IImpedanceControl.h>

#include "ControlBoardWrapperCommon.h"

class ControlBoardWrapperImpedanceControl :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IImpedanceControl
{
public:
    inline bool getAxes(int* ax) override { return ControlBoardWrapperCommon::getAxes(ax); }
    bool setImpedance(int j, double stiff, double damp) override;
    bool setImpedanceOffset(int j, double offset) override;
    bool getImpedance(int j, double* stiff, double* damp) override;
    bool getImpedanceOffset(int j, double* offset) override;
    bool getCurrentImpedanceLimit(int j, double* min_stiff, double* max_stiff, double* min_damp, double* max_damp) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERIMPEDANCECONTROL_H
