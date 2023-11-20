/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IVELOCITYCONTROLTEST_H
#define IVELOCITYCONTROLTEST_H

#include <memory>
#include <numeric>

#include <yarp/dev/IVelocityControl.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/os/Time.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iVelocityControl_test_1(IVelocityControl* ivel, IControlMode* icmd)
    {
        REQUIRE(ivel != nullptr);
        REQUIRE(icmd != nullptr);

        bool b;
        int ax;

        b = ivel->getAxes(&ax);
        CHECK(b);
        REQUIRE(ax > 0);

        for (size_t i = 0; i< ax; i++)
        {
            b = icmd->setControlMode(i,VOCAB_CM_VELOCITY);
            CHECK(b);
        }

        const double ref_test=1.123;
        double ref=0;
        b = ivel->setRefAcceleration(0, ref_test);
        CHECK(b);
        b = ivel->getRefAcceleration(0, &ref);
        CHECK(b);
        CHECK(fabs(ref-ref_test)<0.001);

        b = ivel->velocityMove(0, ref_test);
        CHECK(b);
        yarp::os::Time::delay(0.050);
        b = ivel->getRefVelocity(0,&ref);
        CHECK(b);
        CHECK(fabs(ref - ref_test) < 0.001);

        b = ivel->stop();
        CHECK(b);
    }

    inline void exec_iVelocityControl_test_unimplemented_interface(IVelocityControl* ivel, IControlMode* icmd)
    {
        REQUIRE(ivel != nullptr);
        REQUIRE(icmd != nullptr);

        bool b;
        int ax;
        b = ivel->getAxes(&ax);
        CHECK(b);
        REQUIRE(ax > 0);

        double ref = 0;
        auto refs = std::vector<double>(ax);

        for (size_t i = 0; i < ax; i++)
        {
            b = icmd->setControlMode(i, VOCAB_CM_VELOCITY);
            //CHECK(b); //this will fail
        }

        b = ivel->setRefAcceleration(0, ref);
        CHECK(!b);
        b = ivel->getRefAcceleration(0, &ref);
        CHECK(!b);
        b = ivel->setRefAccelerations( refs.data());
        CHECK(!b);
        b = ivel->getRefAccelerations( refs.data());
        CHECK(!b);

        b = ivel->velocityMove(0, ref);
        //CHECK(!b); //this is streaming, it will return true always

        b = ivel->getRefVelocity(0, &ref);
        CHECK(!b);
        b = ivel->getRefVelocities(refs.data());
        CHECK(!b);

        b = ivel->stop();
        CHECK(!b);
    }
}

#endif
