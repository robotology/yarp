/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IPOSITIONDIRECTTEST_H
#define IPOSITIONDIRECTTEST_H

#include <memory>
#include <numeric>
#include <vector>

#include <yarp/os/Time.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IPositionDirect.h>
#include <catch2/catch_amalgamated.hpp>
#include "Utils.h"

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iPositionDirect_test_1(IPositionDirect* ipdir, IControlMode* icmd)
    {
        REQUIRE(ipdir != nullptr);
        REQUIRE(icmd != nullptr);

        bool b=false;
#if 0
        size_t ax=0;
#else
        int ax=0;
#endif
        b = ipdir->getAxes(&ax);
        CHECK(b);
        REQUIRE(ax != 0);

        for (size_t i = 0; i < ax; i++)
        {
            b = icmd->setControlMode(i, VOCAB_CM_POSITION_DIRECT);
            CHECK(b);
            yarp::os::Time::delay(0.10); // Allow some time for the command to take effect

            int mode_ret = 0;
            b = icmd->getControlMode(i, &mode_ret);
            CHECK(b);
            CHECK(mode_ret == VOCAB_CM_POSITION_DIRECT);
        }

#if 0
        {
            double ref = 12;
            double ref_check = 12;
            auto refs = std::vector<double>(ax);
            auto refs_check = std::vector<double>(ax);
            auto joints = std::vector<int>(ax);
            set_vector_crescent(joints.begin(), joints.end(), 0);

            b = ipdir->setPosition(0, ref);
            CHECK(b);
            yarp::os::Time::delay(0.020); // Allow some time for the command to take effect

            b = ipdir->getRefPosition(0, ref);
            CHECK(b);
            CHECK(ref == ref_check);

            set_vector_crescent(refs, 1.0);
            set_vector_crescent(refs_check, 1.0);

            b = ipdir->setPositions(refs);
            CHECK(b);
            yarp::os::Time::delay(0.020); // Allow some time for the command to take effect

            b = ipdir->getRefPositions(refs);
            CHECK(b);
            CHECK(vectors_equal(refs, refs_check));

            set_vector_crescent(refs, 10.0);
            set_vector_crescent(refs_check, 10.0);

            b = ipdir->setPositions(joints, refs);
            CHECK(b);
            yarp::os::Time::delay(0.020); // Allow some time for the command to take effect

            b = ipdir->getRefPositions(joints, refs);
            CHECK(b);
            CHECK(vectors_equal(refs, refs_check));
        }
#else
        {
            double ref = 12;
            double ref_check = 12;
            auto refs = std::vector<double>(ax);
            auto refs_check = std::vector<double>(ax);
            auto joints = std::vector<int>(ax);
            set_vector_crescent(ax, joints.data(), 0);

            b = ipdir->setPosition(0, ref);
            CHECK(b);
            yarp::os::Time::delay(0.020); // Allow some time for the command to take effect

            b = ipdir->getRefPosition(0, &ref);
            CHECK(b);
            CHECK(ref == ref_check);

            set_vector_crescent(ax,refs.data(), 1.0);
            set_vector_crescent(ax,refs_check.data(), 1.0);

            b = ipdir->setPositions(refs.data());
            CHECK(b);
            yarp::os::Time::delay(0.020); // Allow some time for the command to take effect

            b = ipdir->getRefPositions(refs.data());
            CHECK(b);
            CHECK(vectors_equal(ax, refs.data(), refs_check.data()));

            set_vector_crescent(ax, refs.data(), 10.0);
            set_vector_crescent(ax, refs_check.data(), 10.0);

            b = ipdir->setPositions(ax, joints.data(), refs.data());
            CHECK(b);
            yarp::os::Time::delay(0.020); // Allow some time for the command to take effect

            b = ipdir->getRefPositions(ax, joints.data(), refs.data());
            CHECK(b);
            CHECK(vectors_equal(ax, refs.data(), refs_check.data()));
        }
    }
}
#endif

#endif
