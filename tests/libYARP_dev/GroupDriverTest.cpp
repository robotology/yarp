/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/PolyDriver.h>

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>

#include <string>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;

TEST_CASE("dev::GroupDriver", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("group", "device");
    YARP_REQUIRE_PLUGIN("fakeMotionControl", "device");
    YARP_REQUIRE_PLUGIN("controlboardwrapper2", "device");

    Network::setLocalMode(true);

    SECTION("make sure groups of devices can be instantiated correctly")
    {
        Property p;
        p.fromConfig("\
device group\n\
\n\
[part mymotor]\n\
device fakeMotionControl\n\
GENERAL (Joints 10)\n\
\n\
[part broadcast]\n\
device controlboardwrapper2\n\
subdevice mymotor\n\
name /mymotor\n\
");
        p.put("verbose",1);
        PolyDriver dd(p);
        Bottle cmd("get axes"), reply;
        Network::write(Contact("/mymotor/rpc:i"), cmd, reply);
        CHECK(reply.get(2).asInt32() == 10); // axis count is correct
    }

    Network::setLocalMode(false);
}
