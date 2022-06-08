/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERENCODERSTIMED_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERENCODERSTIMED_H

#include <yarp/dev/IEncodersTimed.h>

#include "ControlBoardWrapperCommon.h"

class ControlBoardWrapperEncodersTimed :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IEncodersTimed
{
public:
    inline bool getAxes(int *ax) override { return ControlBoardWrapperCommon::getAxes(ax); }
    bool resetEncoder(int j) override;
    bool resetEncoders() override;
    bool setEncoder(int j, double val) override;
    bool setEncoders(const double* vals) override;
    bool getEncoder(int j, double* v) override;
    bool getEncoders(double* encs) override;
    bool getEncoderSpeed(int j, double* sp) override;
    bool getEncoderSpeeds(double* spds) override;
    bool getEncoderAcceleration(int j, double* acc) override;
    bool getEncoderAccelerations(double* accs) override;
    bool getEncodersTimed(double* encs, double* t) override;
    bool getEncoderTimed(int j, double* v, double* t) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERENCODERSTIMED_H
