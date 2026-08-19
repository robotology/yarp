/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Random.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;


TEST_CASE("os::RandomTest", "[yarp::os]")
{
    SECTION("checking Random can generate numbers")
    {
       Random::seed(123456);

       // checking uniform distribution
       double uni = 0;
       uni =Random::uniform();
       CHECK(uni != 0.0);
       int uniInt = Random::uniformInt(0, 32000);
       CHECK(uniInt != 0);

       // checking normal distribution
       double norm = 0;
       norm = Random::normal();
       CHECK(norm != 0.0);
       norm = Random::normal(0.5,0.5);
       CHECK(norm != 0.0);
    }
}
