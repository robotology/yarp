/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("Harness Test 2", "[Harness Test]")
{
    //The purpose of this test is to verify possible memory leaks (via valgrind or other tools)
    //of the method yarp harness framework, involving a minimal set of libraries.
    //This specific test links to YARP_harness and yarp_os.
    SECTION("Harness Test 2")
    {
        bool ok = true;
        CHECK(ok);
    }
}
