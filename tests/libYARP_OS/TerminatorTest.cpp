/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Terminator.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#if defined(USE_SYSTEM_CATCH)
#include <catch.hpp>
#else
#include "catch.hpp"
#endif

using namespace yarp::os;

TEST_CASE("OS::TerminatorTest", "[yarp::os]") {

    SECTION("checking terminator connection") {
        Network::setLocalMode(true);
        Terminee terminee("/tmp/quit");
        INFO("checking terminator socket");
        CHECK(terminee.isOk());
        INFO("checking termination connection");
        CHECK(Terminator::terminateByName("/tmp/quit"));
        INFO("checking the receive of the quit message");
        CHECK(terminee.mustQuit());
    }
}
