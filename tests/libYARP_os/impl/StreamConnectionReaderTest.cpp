/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/StringOutputStream.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;

TEST_CASE("os::impl::StreamConnectionReaderTest", "[yarp::os][yarp::os::impl]")
{
    SECTION("testing reading")
    {
        StringInputStream sis;
        StringOutputStream sos;
        sis.add("Hello\ngood evening and welcome");
        StreamConnectionReader sbr;
        Route route;
        sbr.reset(sis, nullptr, route, 10, true);
        std::string line = sbr.expectLine();
        CHECK(line == "Hello"); // one line
    }
}
