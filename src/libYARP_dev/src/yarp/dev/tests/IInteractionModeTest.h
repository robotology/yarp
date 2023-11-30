/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IINTERACTIONMODETEST_H
#define IINTERACTIONMODETEST_H

#include <memory>
#include <numeric>
#include <vector>

#include <yarp/dev/IAxisInfo.h>
#include <yarp/dev/IInteractionMode.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iInteractionMode_test_1(IInteractionMode* iint, IAxisInfo* iinfo)
    {
        REQUIRE(iint != nullptr);
        REQUIRE(iinfo != nullptr);

        int ax;
        bool b = iinfo->getAxes(&ax);
        CHECK(b);
        REQUIRE(ax > 0);

        yarp::dev::InteractionModeEnum val;
        yarp::dev::InteractionModeEnum ret;

        b = iint->getInteractionMode(0,&ret);
        CHECK(b);

        val = yarp::dev::InteractionModeEnum::VOCAB_IM_STIFF;
        b = iint->setInteractionMode(0,val);
        CHECK(b);

        b = iint->getInteractionMode(0, &ret);
        CHECK(b);

        val = yarp::dev::InteractionModeEnum::VOCAB_IM_COMPLIANT;
        b = iint->setInteractionMode(0, val);
        CHECK(b);

        b = iint->getInteractionMode(0, &ret);
        CHECK(b);

        auto modes = std::vector< yarp::dev::InteractionModeEnum>(ax);
        b = iint->getInteractionModes(modes.data());
        CHECK(b);

        auto joints = std::vector<int>(ax);
        std::iota(joints.begin(), joints.end(), 0);
        b = iint->getInteractionModes(ax, joints.data(), modes.data());
        CHECK(b);

        for (size_t j = 0; j < ax; j++)
        {
            modes[j]= yarp::dev::InteractionModeEnum::VOCAB_IM_STIFF;
        }
        b = iint->setInteractionModes(modes.data());
        CHECK(b);
        b = iint->setInteractionModes(ax, joints.data(), modes.data());
        CHECK(b);

    }
}
#endif
