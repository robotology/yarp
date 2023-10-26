/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ITORQUECONTROLTEST_H
#define ITORQUECONTROLTEST_H

#include <yarp/dev/ITorqueControl.h>
#include <yarp/dev/IControlMode.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iTorqueControl_test_1(ITorqueControl* itrq, IControlMode* icmd)
    {
        REQUIRE(itrq != nullptr);
        REQUIRE(icmd != nullptr);

        bool b;

        int ax;
        b = itrq->getAxes(&ax);
        CHECK(b);
        REQUIRE(ax > 0);

        for (size_t i = 0; i < ax; i++)
        {
            b = icmd->setControlMode(i, VOCAB_CM_TORQUE);
            CHECK(b);
        }

        yarp::dev::MotorTorqueParameters params;
        //params.bemf = 0.1;
        //params.bemf_scale = 0.2;
        //params.ktau = 0.3;
        //params.ktau_scale = 0.4;
        params.viscousPos = 0.5;
        params.viscousNeg = 0.6;
        params.coulombPos = 0.7;
        params.coulombNeg = 0.8;
        params.velocityThres = 0.9;
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
        CHECK(res.velocityThres == 0.9); // interface seems functional
    }

    inline void exec_iTorqueControl_test_unimplemented_interface(ITorqueControl* itrq, IControlMode* icmd)
    {
        REQUIRE(itrq != nullptr);
        REQUIRE(icmd != nullptr);

        bool b;
        int ax;
        double val, val1, val2;
        yarp::dev::MotorTorqueParameters param;

        b = itrq->getAxes(&ax);
        CHECK(b);
        REQUIRE(ax > 0);

        for (size_t i = 0; i < ax; i++)
        {
            b = icmd->setControlMode(i, VOCAB_CM_TORQUE);
            //CHECK(b); //this will fail
        }

        double* vals = new double [ax];
        double* refvals = new double[ax];
        double* vals1 = new double[ax];
        double* vals2 = new double[ax];

        b = itrq->setMotorTorqueParams(0, param);
        CHECK(!b);

        b = itrq->getMotorTorqueParams(0, &param);
        CHECK(!b);

        b = itrq->getRefTorque(0, &val);
        CHECK(!b);

        b = itrq->getRefTorques(refvals);
        CHECK(!b);

        b = itrq->getTorque(0,&val);
        CHECK(!b);

        b = itrq->getTorqueRange(0, &val1, &val2);
        CHECK(!b);

        b = itrq->getTorqueRanges(vals1, vals2);
        CHECK(!b);

        b = itrq->getTorques(vals); //streaming
   //     CHECK(!b); //this is streaming, it will return true if data is arrived in time

        b = itrq->setRefTorque(0,val); //streaming
   //     CHECK(!b); //this is streaming, it will return true always

        b = itrq->setRefTorques(refvals); //streaming
   //     CHECK(!b); //this is streaming, it will return true always

        delete[] vals;
        delete[] refvals;
        delete[] vals1;
        delete[] vals2;
    }
}

#endif
