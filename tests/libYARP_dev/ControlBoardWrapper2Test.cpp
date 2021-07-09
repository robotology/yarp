/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/PolyDriver.h>

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <string>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;

TEST_CASE("dev::ControlBoardWrapper2", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("group", "device");
    YARP_REQUIRE_PLUGIN("fakeMotionControl", "device");
    YARP_REQUIRE_PLUGIN("controlboardwrapper2", "device");
    YARP_REQUIRE_PLUGIN("remote_controlboard", "device");

    Network::setLocalMode(true);

    SECTION("test the controlboardwrapper2 device")
    {
        PolyDriver dd;
        Property p;
        p.put("device","controlboardwrapper2");
        p.put("subdevice","fakeMotionControl");
        p.put("name","/motor");
        auto& pg = p.addGroup("GENERAL");
        pg.put("Joints", 16);
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
