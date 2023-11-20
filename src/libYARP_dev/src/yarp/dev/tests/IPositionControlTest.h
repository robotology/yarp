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
        const int nr_ax = 2;

        REQUIRE(ipos != nullptr);
        REQUIRE(icmd != nullptr);

        bool b;
        int ax;

        b = ipos->getAxes(&ax);
        CHECK(b);
        REQUIRE(ax == nr_ax);

        for (size_t i = 0; i < ax; i++)
        {
            b = icmd->setControlMode(i, VOCAB_CM_POSITION);
            CHECK(b);
        }

        {
            double ref = 0;
            double refs[nr_ax];
            int joints[nr_ax] = { 0,1 };

            b = ipos->setRefSpeed(0, ref);
            CHECK(b);

            b = ipos->setRefSpeeds(refs);
            CHECK(b);

            b = ipos->setRefSpeeds(nr_ax, joints, refs);
            CHECK(b);

            b = ipos->setRefAcceleration(0, ref);
            CHECK(b);

            b = ipos->setRefAccelerations(refs);
            CHECK(b);

            b = ipos->setRefAccelerations(nr_ax, joints, refs);
            CHECK(b);
        }
        {
            double ref = 0;
            double refs[nr_ax];
            int joints[nr_ax] = { 0,1 };

            b = ipos->getRefSpeed(0,&ref);
            CHECK(b);

            b = ipos->getRefSpeeds(refs);
            CHECK(b);

            b = ipos->getRefSpeeds(nr_ax, joints, refs);
            CHECK(b);

            b = ipos->getRefAcceleration(0, &ref);
            CHECK(b);

            b = ipos->getRefAccelerations(refs);
            CHECK(b);

            b = ipos->getRefAccelerations(nr_ax, joints, refs);
            CHECK(b);

            b = ipos->getTargetPosition(0, &ref);
            CHECK(b);

            b = ipos->getTargetPositions(refs);
            CHECK(b);

            b = ipos->getTargetPositions(nr_ax, joints, refs);
            CHECK(b);
        }

        {
            double ref = 0;
            double refs[nr_ax];
            int joints[nr_ax] = { 0,1 };
            b = ipos->positionMove(0, ref);
            CHECK(b);

            b = ipos->positionMove(refs);
            CHECK(b);

            b = ipos->positionMove(nr_ax, joints, refs);
            CHECK(b);

            b = ipos->relativeMove(0, ref);
            CHECK(b);

            b = ipos->relativeMove(refs);
            CHECK(b);

            b = ipos->relativeMove(nr_ax, joints, refs);
            CHECK(b);
        }

        {
            bool flag;
            bool flags[nr_ax];
            int joints[nr_ax] = {0,1};
            b = ipos->checkMotionDone(0,&flag);
            CHECK(b);

            b = ipos->checkMotionDone(flags);
            CHECK(b);

            b = ipos->checkMotionDone(nr_ax,joints,flags);
            CHECK(b);
        }

        {
            b = ipos->stop();
            CHECK(b);
        }
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
