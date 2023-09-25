/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;

TEST_CASE("Harness Test 1", "[Harness Test]")
{
    //The purpose of this test is to verify possible memory leaks (via valgrind or other tools)
    //of the method Network::setLocalMode(). This method is indeed used in many other tests.
    Network::setLocalMode(true);

    SECTION("Harness Test 1")
    {
        bool ok = true;
        CHECK(ok);
    }

    Network::setLocalMode(false);
}
