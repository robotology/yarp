/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>
#include <cstdlib>

#include <yarp/os/Contact.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/impl/NameClient.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;

/**
 * Name server regression tests.
 */
TEST_CASE("serversql::ServerTest", "[yarp::serversql]")
{
    Network::setLocalMode(true);

    SECTION("check register free")
    {
        INFO("checking free register command...");
        NameClient& nic = NameClient::getNameClient();
        nic.registerName("/check/register1");
        Contact addr1 = nic.queryName("/check/register1");
        CHECK(addr1.isValid()); // got an address
        CHECK(addr1.getCarrier() == "tcp"); // correct carrier
    }

    Network::setLocalMode(false);
}
