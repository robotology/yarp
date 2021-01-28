/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/NameConfig.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;

TEST_CASE("os::impl::NameConfig", "[yarp::os][yarp::os::impl]")
{
    SECTION("testing compilation")
    {
        NameConfig nc;
        std::string fname = nc.getConfigFileName();
        INFO(fname);
        CHECK(true);
    }
}
