/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERAMPLIFIERCONTROL_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERAMPLIFIERCONTROL_H

#include <yarp/dev/IAmplifierControl.h>

#include "ControlBoardWrapperCommon.h"

class ControlBoardWrapperAmplifierControl :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IAmplifierControl
{
public:
    bool enableAmp(int j) override;
    bool disableAmp(int j) override;
    bool getAmpStatus(int* st) override;
    bool getAmpStatus(int j, int* v) override;
    inline bool getCurrent(int m, double *curr) override { return ControlBoardWrapperCommon::getCurrent(m, curr); }
    inline bool getCurrents(double *currs) override { return ControlBoardWrapperCommon::getCurrents(currs); }
    bool setMaxCurrent(int j, double v) override;
    bool getMaxCurrent(int j, double* v) override;
    bool getNominalCurrent(int m, double* val) override;
    bool setNominalCurrent(int m, const double val) override;
    bool getPeakCurrent(int m, double* val) override;
    bool setPeakCurrent(int m, const double val) override;
    bool getPWM(int m, double* val) override;
    bool getPWMLimit(int m, double* val) override;
    bool setPWMLimit(int m, const double val) override;
    bool getPowerSupplyVoltage(int m, double* val) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERAMPLIFIERCONTROL_H
