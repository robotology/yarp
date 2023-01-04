/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ITORQUECONTROLTEST_H
#define ITORQUECONTROLTEST_H

#include <yarp/dev/ITorqueControl.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iTorqueControl_test_1(ITorqueControl* itrq)
    {
        REQUIRE(itrq != nullptr);

        bool b;

        int ax;
        b = itrq->getAxes(&ax);
        CHECK(b);
        CHECK(ax != 0);

        yarp::dev::MotorTorqueParameters params;
        //params.bemf = 0.1;
        //params.bemf_scale = 0.2;
        //params.ktau = 0.3;
        //params.ktau_scale = 0.4;
        params.viscousPos = 0.5;
        params.viscousNeg = 0.6;
        params.coulombPos = 0.7;
        params.coulombNeg = 0.8;
        b= itrq->setMotorTorqueParams(0, params);
        CHECK(b);

        yarp::dev::MotorTorqueParameters res;
        b= itrq->getMotorTorqueParams(0, &res);
        CHECK(b);
        //CHECK(res.bemf == 0.1); // interface seems functional
        //CHECK(res.bemf_scale == 0.2); // interface seems functional
        //CHECK(res.ktau == 0.3); // interface seems functional
        //CHECK(res.ktau_scale == 0.4); // interface seems functional
        CHECK(res.viscousPos == 0.5); // interface seems functional
        CHECK(res.viscousNeg == 0.6); // interface seems functional
        CHECK(res.coulombPos == 0.7); // interface seems functional
        CHECK(res.coulombNeg == 0.8); // interface seems functional
    }
}

#endif
