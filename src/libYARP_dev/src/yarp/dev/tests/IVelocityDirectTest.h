/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IVELOCITYDIRECTTEST_H
#define IVELOCITYDIRECTTEST_H

#include <memory>
#include <numeric>
#include <vector>

#include <yarp/os/Time.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IVelocityDirect.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    // Utility function to compare two int vectors for equality
    inline bool vectors_equal(const std::vector<double>& a, const std::vector<double>& b)
    {
        return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
    }

    inline void exec_iVelocityDirect_test_1(IVelocityDirect* ivdir, IControlMode* icmd)
    {
        REQUIRE(ivdir != nullptr);
        REQUIRE(icmd != nullptr);

        bool b=false;
        size_t ax=0;

        b = ivdir->getAxes(ax);
        CHECK(b);
        REQUIRE(ax != 0);

        for (size_t i = 0; i < ax; i++)
        {
            b = icmd->setControlMode(i, VOCAB_CM_VELOCITY_DIRECT);
            CHECK(b);
            yarp::os::Time::delay(0.020); // Allow some time for the command to take effect

            int mode_ret = 0;
            b = icmd->getControlMode(i, &mode_ret);
            CHECK(b);
            CHECK(mode_ret == VOCAB_CM_VELOCITY_DIRECT);
        }

        {
            double ref = 12;
            double ref_check = 12;
            auto refs = std::vector<double>(ax);
            auto refs_check = std::vector<double>(ax);
            auto joints = std::vector<int>(ax);
            std::iota(joints.begin(), joints.end(), 0);

            b = ivdir->setDesiredVelocity(0, ref);
            CHECK(b);
            yarp::os::Time::delay(0.020); // Allow some time for the command to take effect

            b = ivdir->getDesiredVelocity(0, ref);
            CHECK(b);
            CHECK(ref == ref_check);

            std::iota(refs.begin(), refs.end(), 1.0);
            std::iota(refs_check.begin(), refs_check.end(), 1.0);

            b = ivdir->setDesiredVelocity(refs);
            CHECK(b);
            yarp::os::Time::delay(0.020); // Allow some time for the command to take effect

            b = ivdir->getDesiredVelocity(refs);
            CHECK(b);
            CHECK(vectors_equal(refs, refs_check));

            std::iota(refs.begin(), refs.end(), 10.0);
            std::iota(refs_check.begin(), refs_check.end(), 10.0);

            b = ivdir->setDesiredVelocity(joints, refs);
            CHECK(b);
            yarp::os::Time::delay(0.020); // Allow some time for the command to take effect

            b = ivdir->getDesiredVelocity(joints, refs);
            CHECK(b);
            CHECK(vectors_equal(refs, refs_check));
        }
    }
}

#endif
