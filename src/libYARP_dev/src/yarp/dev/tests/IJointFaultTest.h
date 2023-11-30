/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IJOINTFAULTTEST_H
#define IJOINTFAULTTEST_H

#include <yarp/dev/IJointFault.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iJointFault_test_1(IJointFault* ifault)
    {
        REQUIRE(ifault != nullptr);

        bool b =false;
        int fault;
        std::string message;
        b = ifault->getLastJointFault(0,fault,message);
        CHECK(b);
    }
}

#endif
