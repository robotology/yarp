/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCURRENTCONTROL_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCURRENTCONTROL_H

#include <yarp/dev/ICurrentControl.h>

#include "ControlBoardWrapperCommon.h"

class ControlBoardWrapperCurrentControl :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::ICurrentControl
{
public:
    inline bool getNumberOfMotors(int* num) override { return ControlBoardWrapperCommon::getNumberOfMotors(num); }
    inline bool getCurrent(int m, double *curr) override { return ControlBoardWrapperCommon::getCurrent(m, curr); }
    inline bool getCurrents(double *currs) override { return ControlBoardWrapperCommon::getCurrents(currs); }
    bool getCurrentRange(int j, double* min, double* max) override;
    bool getCurrentRanges(double* min, double* max) override;
    bool setRefCurrents(const double* t) override;
    bool setRefCurrent(int j, double t) override;
    bool setRefCurrents(const int n_joint, const int* joints, const double* t) override;
    bool getRefCurrents(double* t) override;
    bool getRefCurrent(int j, double* t) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCURRENTCONTROL_H
