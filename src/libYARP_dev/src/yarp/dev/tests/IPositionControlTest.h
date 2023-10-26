/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IPOSITIONCONTROLTEST_H
#define IPOSITIONCONTROLTEST_H

#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IPositionControl.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iPositionControl_test_1(IPositionControl* ipos, IControlMode* icmd)
    {
        REQUIRE(ipos != nullptr);
        REQUIRE(icmd != nullptr);

        bool b;
        int ax;

        b = ipos->getAxes(&ax);
        CHECK(b);
        REQUIRE(ax > 0);

        for (size_t i = 0; i < ax; i++)
        {
            b = icmd->setControlMode(i, VOCAB_CM_POSITION);
            CHECK(b);
        }

        double ref = 0;
        b = ipos->getRefSpeed(0,&ref);
        CHECK(b);

        b = ipos->getRefAcceleration(0, &ref);
        CHECK(b);

        b = ipos->getTargetPosition(0, &ref);
        CHECK(b);
    }

    inline void exec_iPositionControl_test_unimplemented_interface(IPositionControl* ipos, IControlMode* icmd)
    {
        REQUIRE(ipos != nullptr);
        REQUIRE(icmd != nullptr);

        bool b;
        int ax;

        b = ipos->getAxes(&ax);
        CHECK(b);
        REQUIRE(ax > 0);

        for (size_t i = 0; i < ax; i++)
        {
            b = icmd->setControlMode(i, VOCAB_CM_POSITION);
            //CHECK(b); //this will fail
        }

        double ref = 0;
        b = ipos->getRefSpeed(0, &ref);
        CHECK(!b);

        b = ipos->getRefAcceleration(0, &ref);
        CHECK(!b);

        b = ipos->getTargetPosition(0, &ref);
        CHECK(!b);
    }
}

#endif
