/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>

#include <TestMultipleContainers.h>

#include <catch2/catch_amalgamated.hpp>

using namespace yarp::os;
using namespace yarp::os::impl;


TEST_CASE("IdlThriftTest3", "[yarp::idl::thrift3]")
{
    yarp::os::Network::setLocalMode(true);

    SECTION("Test multiple containers")
    {
        std::vector<int> vec;
        TestMultipleContainers server;

        // testing just the call, the implementation of the method is missing and
        // it is not relevant for this test
        bool ret = server.testDoubleList(vec);
        CHECK(ret == false);
    }

    yarp::os::Network::setLocalMode(false);
}

int main(int argc, char* argv[])
{
    Network yarp;
    return Catch::Session().run(argc, argv);
}
