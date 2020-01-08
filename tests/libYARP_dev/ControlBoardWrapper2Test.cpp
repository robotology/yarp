/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/PolyDriver.h>

#include <yarp/os/Network.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <string>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

TEST_CASE("dev::ControlBoardWrapper2", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("group", "device");
    YARP_REQUIRE_PLUGIN("test_motor", "device");
    YARP_REQUIRE_PLUGIN("remote_controlboard", "device");

    Network::setLocalMode(true);

    SECTION("test the controlboard wrapper 2")
    {
        PolyDriver dd;
        Property p;
        p.put("device","controlboardwrapper2");
        p.put("subdevice","test_motor");
        p.put("name","/motor");
        p.put("axes",16);
        bool result;
        result = dd.open(p);
        REQUIRE(result); // controlboardwrapper open reported successful

        // Check if IMultipleWrapper interface is correctly found
        yarp::dev::IMultipleWrapper * i_mwrapper=nullptr;
        result = dd.view(i_mwrapper);
        REQUIRE(result); // IMultipleWrapper view reported successful
        REQUIRE(i_mwrapper != nullptr); // IMultipleWrapper pointer not null

        PolyDriver dd2;
        Property p2;
        p2.put("device","remote_controlboard");
        p2.put("remote","/motor");
        p2.put("local","/motor/client");
        p2.put("carrier","tcp");
        p2.put("ignoreProtocolCheck","true");
        result = dd2.open(p2);
        REQUIRE(result); // remote_controlboard open reported successful

        IPositionControl *pos = nullptr;
        result = dd2.view(pos);
        REQUIRE(result); // interface reported
        int axes = 0;
        pos->getAxes(&axes);
        CHECK(axes == 16); // interface seems functional
        CHECK(dd.close()); // close dd reported successful
        CHECK(dd2.close()); // close dd2 reported successful
    }
}
