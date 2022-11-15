/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/PolyDriver.h>

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/WrapperSingle.h>

#include <string>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;

TEST_CASE("dev::controlBoard_nws_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("group", "device");
    YARP_REQUIRE_PLUGIN("fakeMotionControl", "device");
    YARP_REQUIRE_PLUGIN("controlBoard_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("remote_controlboard", "device");

    Network::setLocalMode(true);

    SECTION("test the controlBoard_nws_yarp device")
    {
        PolyDriver dd_nws;
        Property p_nws;
        p_nws.put("device", "controlBoard_nws_yarp");
        p_nws.put("name", "/motor");
        bool result_nws = dd_nws.open(p_nws);
        REQUIRE(result_nws);

        PolyDriver dd_dev;
        Property p_dev;
        p_dev.put("device", "fakeMotionControl");
        auto& pg = p_dev.addGroup("GENERAL");
        pg.put("Joints", 16);
        bool result_dev = dd_dev.open(p_dev);
        REQUIRE(result_dev);

        yarp::dev::WrapperSingle* ww_nws;
        dd_nws.view(ww_nws);
        bool result_att = ww_nws->attach(&dd_dev);
        REQUIRE(result_att);

        // Check if IMultipleWrapper interface is correctly found
        yarp::dev::IMultipleWrapper * i_mwrapper=nullptr;
        bool result_imw = dd_nws.view(i_mwrapper);
        REQUIRE(result_imw); // IMultipleWrapper view reported successful
        REQUIRE(i_mwrapper != nullptr); // IMultipleWrapper pointer not null

        PolyDriver dd2;
        Property p2;
        p2.put("device","remote_controlboard");
        p2.put("remote","/motor");
        p2.put("local","/motor/client");
        p2.put("carrier","tcp");
        p2.put("ignoreProtocolCheck","true");
        bool result = dd2.open(p2);
        REQUIRE(result); // remote_controlboard open reported successful

        IPositionControl *pos = nullptr;
        result = dd2.view(pos);
        REQUIRE(result); // interface reported
        int axes = 0;
        pos->getAxes(&axes);
        CHECK(axes == 16); // interface seems functional
        CHECK(dd_nws.close()); // close dd reported successful
        CHECK(dd_dev.close()); // close dd reported successful
        CHECK(dd2.close()); // close dd2 reported successful
    }
}
