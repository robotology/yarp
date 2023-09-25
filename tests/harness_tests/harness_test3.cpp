/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("Harness Test 3", "[Harness Test]")
{
    //The purpose of this test is to verify possible memory leaks (via valgrind or other tools)
    //of the method yarp harness framework, involving a minimal set of libraries.
    //This specific test links to YARP_harness_no_network only.
    SECTION("Harness Test 3")
    {
        bool ok = true;
        CHECK(ok);
    }
}
