/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>

#include <TestMultipleContainers.h>

#include <catch2/catch_amalgamated.hpp>

using namespace yarp::os;


TEST_CASE("IdlThriftTest3", "[yarp::idl::thrift3]")
{
    yarp::os::Network::setLocalMode(true);

    SECTION("Test multiple containers")
    {
        std::vector<int> vec1;
        std::set<int> set1;
        std::map<int,std::string> map1;

        TestMultipleContainers server;

        // testing just the call, the implementation of the method is missing and
        // it is not relevant for this test
        bool ret;
        ret = server.testList1(vec1);
        CHECK(ret == false);
        ret = server.testList2(vec1,vec1);
        CHECK(ret == false);

        ret = server.testMap1(map1);
        CHECK(ret == false);
        ret = server.testMap2(map1, map1);
        CHECK(ret == false);

        ret = server.testSet1(set1);
        CHECK(ret == false);
        ret = server.testSet2(set1, set1);
        CHECK(ret == false);
    }

    yarp::os::Network::setLocalMode(false);
}

int main(int argc, char* argv[])
{
    Network yarp;
    return Catch::Session().run(argc, argv);
}
