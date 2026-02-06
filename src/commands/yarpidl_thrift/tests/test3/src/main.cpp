/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>

#include <testdata.h>
#include <Settings.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::os;
using namespace yarp::os::impl;


TEST_CASE("IdlThriftTest", "[yarp::idl::thrift]")
{
    yarp::os::Network::setLocalMode(true);

    SECTION("add one")
    {

    }

    yarp::os::Network::setLocalMode(false);
}

int main(int argc, char* argv[])
{
    Network yarp;
    return Catch::Session().run(argc, argv);
}
