/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Terminator.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;

TEST_CASE("os::TerminatorTest", "[yarp::os]")
{
    Network::setLocalMode(true);
    SECTION("checking terminator connection")
    {
        Terminee terminee("/tmp/quit");
        INFO("checking terminator socket");
        CHECK(terminee.isOk());
        INFO("checking termination connection");
        CHECK(Terminator::terminateByName("/tmp/quit"));
        INFO("checking the receive of the quit message");
        CHECK(terminee.mustQuit());
    }
    Network::setLocalMode(false);
}
