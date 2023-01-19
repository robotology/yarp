/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERMOTORENCODERS_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERMOTORENCODERS_H

#include <yarp/dev/IMotorEncoders.h>

#include "ControlBoardWrapperCommon.h"

class ControlBoardWrapperMotorEncoders :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IMotorEncoders
{
public:
    bool getNumberOfMotorEncoders(int* num) override;
    bool resetMotorEncoder(int m) override;
    bool resetMotorEncoders() override;
    bool setMotorEncoderCountsPerRevolution(int m, const double cpr) override;
    bool getMotorEncoderCountsPerRevolution(int m, double* cpr) override;
    bool setMotorEncoder(int m, const double val) override;
    bool setMotorEncoders(const double* vals) override;
    bool getMotorEncoder(int m, double* v) override;
    bool getMotorEncoders(double* encs) override;
    bool getMotorEncodersTimed(double* encs, double* t) override;
    bool getMotorEncoderTimed(int m, double* v, double* t) override;
    bool getMotorEncoderSpeed(int m, double* sp) override;
    bool getMotorEncoderSpeeds(double* spds) override;
    bool getMotorEncoderAcceleration(int m, double* acc) override;
    bool getMotorEncoderAccelerations(double* accs) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERMOTORENCODERS_H
