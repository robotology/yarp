/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IEncoders.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/ParametersTest.h>

#include <ControlBoardMsgs.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::controlBoard_nws_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("controlBoard_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking controlBoard_nws_yarp device")
    {
        PolyDriver ddnws;

        ////////"Checking opening map2DServer and map2DClient polydrivers"
        {
            Property pcfg;
            pcfg.put("device", "controlBoard_nws_yarp");
            pcfg.put("name", "/controlboard");
            REQUIRE(ddnws.open(pcfg));
            yarp::dev::tests::exec_params_test(&ddnws);
        }

        //"Close all polydrivers and check"
        {
            CHECK(ddnws.close());
        }
    }

    SECTION("Test the controlBoard_nws_yarp device with fakeMotionControl device attached")
    {
        PolyDriver dd_fake;
        PolyDriver dd_nws;
        Property p_fake;
        Property p_nws;

        //open
        p_nws.put("device", "controlBoard_nws_yarp");
        p_nws.put("name", "/controlboard");
        p_fake.put("device", "fakeMotionControl");
        REQUIRE(dd_fake.open(p_fake));
        REQUIRE(dd_nws.open(p_nws));

        yarp::os::SystemClock::delaySystem(0.5);

        //attach
        {
            yarp::dev::WrapperSingle* ww_nws; dd_nws.view(ww_nws);
            REQUIRE(ww_nws);
            bool result_att = ww_nws->attach(&dd_fake);
            REQUIRE(result_att);
        }

        yarp::os::SystemClock::delaySystem(1.0);

        //Close all polydrivers and check
        {
            CHECK(dd_nws.close());
            CHECK(dd_fake.close());
        }
    }

    SECTION("IControlMode getters are exposed on the RPC interface")
    {
        PolyDriver dd_fake;
        PolyDriver dd_nws;
        Property p_fake;
        Property p_nws;

        //open
        p_nws.put("device", "controlBoard_nws_yarp");
        p_nws.put("name", "/controlboard");
        p_fake.put("device", "fakeMotionControl");
        p_fake.addGroup("GENERAL").put("Joints", 4);
        REQUIRE(dd_fake.open(p_fake));
        REQUIRE(dd_nws.open(p_nws));

        yarp::os::SystemClock::delaySystem(0.5);

        //attach
        {
            yarp::dev::WrapperSingle* ww_nws; dd_nws.view(ww_nws);
            REQUIRE(ww_nws);
            bool result_att = ww_nws->attach(&dd_fake);
            REQUIRE(result_att);
        }

        yarp::os::SystemClock::delaySystem(1.0);

        size_t nAxes = 0;
        {
            yarp::dev::IEncoders* ienc = nullptr;
            dd_fake.view(ienc);
            REQUIRE(ienc);
            REQUIRE(ienc->getAxes(nAxes));
            REQUIRE(nAxes >= 2);
        }

        //talk to the RPC port, like `yarp rpc` does
        yarp::os::Port rpc;
        REQUIRE(rpc.open("/test/controlboard/rpc"));
        REQUIRE(yarp::os::Network::connect(rpc.getName(), "/controlboard/nws/rpc:i"));

        ControlBoardMsgs rpc_client;
        REQUIRE(rpc_client.yarp().attachAsClient(rpc));

        //set a known mode on axis 0 through the existing RPC setter
        CHECK(rpc_client.setControlModeOneRPC(0, yarp::dev::SelectableControlModeEnum::VOCAB_CM_POSITION_DIRECT) == ReturnValue_ok);

        //read it back through the new RPC getters
        {
            return_getControlModeOne reply = rpc_client.getControlModeOneRPC(0);
            CHECK(reply.ret == ReturnValue_ok);
            CHECK(reply.mode == yarp::dev::ControlModeEnum::VOCAB_CM_POSITION_DIRECT);
        }

        {
            return_getControlModeAll reply = rpc_client.getControlModeAllRPC();
            CHECK(reply.ret == ReturnValue_ok);
            CHECK(reply.modes.size() == nAxes);
            CHECK(reply.modes[0] == yarp::dev::ControlModeEnum::VOCAB_CM_POSITION_DIRECT);
        }

        {
            return_getControlModeGroup reply = rpc_client.getControlModeGroupRPC({0, 1});
            CHECK(reply.ret == ReturnValue_ok);
            CHECK(reply.modes.size() == 2);
            CHECK(reply.modes[0] == yarp::dev::ControlModeEnum::VOCAB_CM_POSITION_DIRECT);
        }

        rpc.close();

        //Close all polydrivers and check
        {
            CHECK(dd_nws.close());
            CHECK(dd_fake.close());
        }
    }

    SECTION("Test the controlBoard_nws_yarp device with a non-attachable device")
    {
        PolyDriver dd_fake2;
        PolyDriver dd_nws;
        Property p_fake2;
        Property p_nws;

        //open
        p_nws.put("device", "controlBoard_nws_yarp");
        p_nws.put("name", "/controlboard");
        p_fake2.put("device", "fakeSpeaker");
        REQUIRE(dd_fake2.open(p_fake2));
        REQUIRE(dd_nws.open(p_nws));

        yarp::os::SystemClock::delaySystem(0.5);

        //attach
        {
            yarp::dev::WrapperSingle* ww_nws; dd_nws.view(ww_nws);
            REQUIRE(ww_nws);
            bool result_att = ww_nws->attach(&dd_fake2);
            REQUIRE(!result_att);
        }

        yarp::os::SystemClock::delaySystem(1.0);

        //Close all polydrivers and check
        {
            CHECK(dd_nws.close());
            CHECK(dd_fake2.close());
        }
    }

    SECTION("Test the controlBoard_nws_yarp device with a device attached which does not implement motion control interfaces")
    {
        PolyDriver dd_fake;
        PolyDriver dd_nws;
        Property p_fake;
        Property p_nws;

        //open
        p_nws.put("device", "controlBoard_nws_yarp");
        p_nws.put("name", "/controlboard");
        p_fake.put("device", "fakeMotionControlMicro");
        REQUIRE(dd_fake.open(p_fake));
        REQUIRE(dd_nws.open(p_nws));

        yarp::os::SystemClock::delaySystem(0.5);

        //attach
        {
            yarp::dev::WrapperSingle* ww_nws; dd_nws.view(ww_nws);
            REQUIRE(ww_nws);
            bool result_att = ww_nws->attach(&dd_fake);
            REQUIRE(result_att);
        }

        yarp::os::SystemClock::delaySystem(1.0);

        //Close all polydrivers and check
        {
            CHECK(dd_nws.close());
            CHECK(dd_fake.close());
        }
    }

    Network::setLocalMode(false);
}
