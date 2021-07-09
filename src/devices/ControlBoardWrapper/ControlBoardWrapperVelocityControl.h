/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERVELOCITYCONTROL_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERVELOCITYCONTROL_H

#include <yarp/dev/IVelocityControl.h>

#include "ControlBoardWrapperCommon.h"


class ControlBoardWrapperVelocityControl :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IVelocityControl
{
public:
    inline bool getAxes(int* ax) override { return ControlBoardWrapperCommon::getAxes(ax); }
    bool velocityMove(int j, double v) override;
    bool velocityMove(const double* v) override;
    bool velocityMove(const int n_joints, const int* joints, const double* spds) override;
    bool getRefVelocity(const int joint, double* vel) override;
    bool getRefVelocities(double* vels) override;
    bool getRefVelocities(const int n_joint, const int* joints, double* vels) override;
    inline bool setRefAcceleration(int j, double acc) override { return ControlBoardWrapperCommon::setRefAcceleration(j, acc); }
    inline bool setRefAccelerations(const double* accs) override { return ControlBoardWrapperCommon::setRefAccelerations(accs); }
    inline bool setRefAccelerations(const int n_joints, const int* joints, const double* accs) override { return ControlBoardWrapperCommon::setRefAccelerations(n_joints, joints, accs); }
    inline bool getRefAcceleration(int j, double* acc) override { return ControlBoardWrapperCommon::getRefAcceleration(j, acc); }
    inline bool getRefAccelerations(double* accs) override { return ControlBoardWrapperCommon::getRefAccelerations(accs); }
    inline bool getRefAccelerations(const int n_joints, const int* joints, double* accs) override { return ControlBoardWrapperCommon::getRefAccelerations(n_joints, joints, accs); }
    inline bool stop(int j) override { return ControlBoardWrapperCommon::stop(j); }
    inline bool stop() override { return ControlBoardWrapperCommon::stop(); }
    inline bool stop(const int n_joint, const int* joints) override { return ControlBoardWrapperCommon::stop(n_joint, joints); }
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERVELOCITYCONTROL_H
