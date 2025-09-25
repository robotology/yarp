/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IPOSITIONCONTROLTEST_H
#define IPOSITIONCONTROLTEST_H

#include <memory>
#include <numeric>
#include <vector>

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

        {
            double ref = 0;
            auto refs = std::vector<double>(ax);
            auto joints = std::vector<int>(ax);
            std::iota(joints.begin(), joints.end(), 0);

            b = ipos->setTrajSpeed(0, ref);
            CHECK(b);

            b = ipos->setTrajSpeeds(refs.data());
            CHECK(b);

            b = ipos->setTrajSpeeds(ax, joints.data(), refs.data());
            CHECK(b);

            b = ipos->setTrajAcceleration(0, ref);
            CHECK(b);

            b = ipos->setTrajAccelerations(refs.data());
            CHECK(b);

            b = ipos->setTrajAccelerations(ax, joints.data(), refs.data());
            CHECK(b);
        }
        {
            double ref = 0;
            auto refs = std::vector<double>(ax);
            auto joints = std::vector<int>(ax);
            std::iota(joints.begin(), joints.end(), 0);

            b = ipos->getTrajSpeed(0,&ref);
            CHECK(b);

            b = ipos->getTrajSpeeds(refs.data());
            CHECK(b);

            b = ipos->getTrajSpeeds(ax, joints.data(), refs.data());
            CHECK(b);

            b = ipos->getTrajAcceleration(0, &ref);
            CHECK(b);

            b = ipos->getTrajAccelerations(refs.data());
            CHECK(b);

            b = ipos->getTrajAccelerations(ax, joints.data(), refs.data());
            CHECK(b);

            b = ipos->getTargetPosition(0, &ref);
            CHECK(b);

            b = ipos->getTargetPositions(refs.data());
            CHECK(b);

            b = ipos->getTargetPositions(ax, joints.data(), refs.data());
            CHECK(b);
        }

        {
            double ref = 0;
            auto refs = std::vector<double>(ax);
            auto joints = std::vector<int>(ax);
            std::iota(joints.begin(), joints.end(), 0);
            b = ipos->positionMove(0, ref);
            CHECK(b);

            b = ipos->positionMove(refs.data());
            CHECK(b);

            b = ipos->positionMove(ax, joints.data(), refs.data());
            CHECK(b);

            b = ipos->relativeMove(0, ref);
            CHECK(b);

            b = ipos->relativeMove(refs.data());
            CHECK(b);

            b = ipos->relativeMove(ax, joints.data(), refs.data());
            CHECK(b);
        }

        {
            bool flag;
            auto flags = std::make_unique<bool>(ax);
            auto joints = std::vector<int>(ax);
            std::iota(joints.begin(), joints.end(), 0);
            b = ipos->checkMotionDone(0,&flag);
            CHECK(b);

            b = ipos->checkMotionDone(flags.get());
            CHECK(b);

            b = ipos->checkMotionDone(ax,joints.data(),flags.get());
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
        b = ipos->getTrajSpeed(0, &ref);
        CHECK(!b);

        b = ipos->getTrajAcceleration(0, &ref);
        CHECK(!b);

        b = ipos->getTargetPosition(0, &ref);
        CHECK(!b);
    }
}

#endif
