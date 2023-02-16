/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IJOYPADCONTROLLERTEST_H
#define IJOYPADCONTROLLERTEST_H

#include <yarp/dev/IJoypadController.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iJoypadController_test_1(yarp::dev::IJoypadController* iJoy)
    {
        REQUIRE(iJoy != nullptr);

        bool b=false;
        unsigned int count=0;

        b = iJoy->getAxisCount(count);  CHECK(b); CHECK(count==4);
    }
}

#endif
