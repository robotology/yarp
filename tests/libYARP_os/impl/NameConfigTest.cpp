/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
