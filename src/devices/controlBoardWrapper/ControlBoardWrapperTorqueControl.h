/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERTORQUECONTROL_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERTORQUECONTROL_H

#include <yarp/dev/ITorqueControl.h>

#include "ControlBoardWrapperCommon.h"

class ControlBoardWrapperTorqueControl :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::ITorqueControl
{
public:
    inline bool getAxes(int *ax) override { return ControlBoardWrapperCommon::getAxes(ax); }
    bool getRefTorques(double* refs) override;
    bool getRefTorque(int j, double* t) override;
    bool setRefTorques(const double* t) override;
    bool setRefTorque(int j, double t) override;
    bool setRefTorques(const int n_joint, const int* joints, const double* t) override;
    bool getMotorTorqueParams(int j, yarp::dev::MotorTorqueParameters* params) override;
    bool setMotorTorqueParams(int j, const yarp::dev::MotorTorqueParameters params) override;
    bool getTorque(int j, double* t) override;
    bool getTorques(double* t) override;
    bool getTorqueRange(int j, double* min, double* max) override;
    bool getTorqueRanges(double* min, double* max) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERTORQUECONTROL_H
