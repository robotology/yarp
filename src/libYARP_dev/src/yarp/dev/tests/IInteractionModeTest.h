/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IINTERACTIONMODETEST_H
#define IINTERACTIONMODETEST_H

#include <yarp/dev/IInteractionMode.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iInteractionMode_test_1(IInteractionMode* iint)
    {
        REQUIRE(iint != nullptr);

        bool b=false;

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
    }
}
#endif
