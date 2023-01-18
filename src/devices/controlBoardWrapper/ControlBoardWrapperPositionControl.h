/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPOSITIONCONTROL_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPOSITIONCONTROL_H

#include <yarp/dev/IPositionControl.h>

#include "ControlBoardWrapperCommon.h"


class ControlBoardWrapperPositionControl :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IPositionControl
{
public:
    inline bool getAxes(int* ax) override
    {
        return ControlBoardWrapperCommon::getAxes(ax);
    }
    bool positionMove(int j, double ref) override;
    bool positionMove(const double* refs) override;
    bool positionMove(const int n_joints, const int* joints, const double* refs) override;
    bool getTargetPosition(const int joint, double* ref) override;
    bool getTargetPositions(double* refs) override;
    bool getTargetPositions(const int n_joint, const int* joints, double* refs) override;
    bool relativeMove(int j, double delta) override;
    bool relativeMove(const double* deltas) override;
    bool relativeMove(const int n_joints, const int* joints, const double* deltas) override;
    bool checkMotionDone(int j, bool* flag) override;
    bool checkMotionDone(bool* flag) override;
    bool checkMotionDone(const int n_joints, const int* joints, bool* flags) override;
    bool setRefSpeed(int j, double sp) override;
    bool setRefSpeeds(const double* spds) override;
    bool setRefSpeeds(const int n_joints, const int* joints, const double* spds) override;
    inline bool setRefAcceleration(int j, double acc) override { return ControlBoardWrapperCommon::setRefAcceleration(j, acc); }
    inline bool setRefAccelerations(const double* accs) override { return ControlBoardWrapperCommon::setRefAccelerations(accs); }
    inline bool setRefAccelerations(const int n_joints, const int* joints, const double* accs) override { return ControlBoardWrapperCommon::setRefAccelerations(n_joints, joints, accs); }
    bool getRefSpeed(int j, double* ref) override;
    bool getRefSpeeds(double* spds) override;
    bool getRefSpeeds(const int n_joints, const int* joints, double* spds) override;
    inline bool getRefAcceleration(int j, double* acc) override { return ControlBoardWrapperCommon::getRefAcceleration(j, acc); }
    inline bool getRefAccelerations(double* accs) override { return ControlBoardWrapperCommon::getRefAccelerations(accs); }
    inline bool getRefAccelerations(const int n_joints, const int* joints, double* accs) override { return ControlBoardWrapperCommon::getRefAccelerations(n_joints, joints, accs); }
    inline bool stop(int j) override { return ControlBoardWrapperCommon::stop(j); }
    inline bool stop() override { return ControlBoardWrapperCommon::stop(); }
    inline bool stop(const int n_joints, const int* joints) override { return ControlBoardWrapperCommon::stop(n_joints, joints); }
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPOSITIONCONTROL_H
