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
#include <yarp/dev/WrapperSingle.h>

using namespace yarp::os;
using namespace yarp::dev;

TEST_CASE("dev::TorqueControl", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeMotionControl", "device");
    YARP_REQUIRE_PLUGIN("controlBoard_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("remote_controlboard", "device");

    Network::setLocalMode(true);

    SECTION("test the controlBoard_nws_yarp device")
    {
        PolyDriver dd_nws;
        Property p_nws;
        p_nws.put("device","controlBoard_nws_yarp");
        p_nws.put("name", "/motor");
        bool result_nws = dd_nws.open(p_nws);
        REQUIRE(result_nws);

        PolyDriver dd_dev;
        Property p_dev;
        p_dev.put("device", "fakeMotionControl");
        auto& pg = p_dev.addGroup("GENERAL");
        pg.put("Joints", 1);
        bool result_dev = dd_dev.open(p_dev);
        REQUIRE(result_dev);

        yarp::dev::WrapperSingle* ww_nws;
        dd_nws.view(ww_nws);
        bool result_att = ww_nws->attach(&dd_dev);
        REQUIRE(result_att); // controlboard_nws_yarp open reported successful

        PolyDriver dd2;
        Property p2;
        p2.put("device","remote_controlboard");
        p2.put("remote","/motor");
        p2.put("local","/motor/client");
        p2.put("carrier","tcp");
        p2.put("ignoreProtocolCheck","true");
        bool result = dd2.open(p2);
        REQUIRE(result); // remote_controlboard open reported successful

        ITorqueControl *trq = nullptr;
        result = dd2.view(trq);
        REQUIRE(result); // interface reported

        yarp::dev::MotorTorqueParameters params;
        yarp::dev::MotorTorqueParameters res;

        //params.bemf = 0.1;
        //params.bemf_scale = 0.2;
        //params.ktau = 0.3;
        //params.ktau_scale = 0.4;
        params.viscousPos = 0.5;
        params.viscousNeg = 0.6;
        params.coulombPos = 0.7;
        params.coulombNeg = 0.8;

        trq->setMotorTorqueParams(0, params);
        trq->getMotorTorqueParams(0, &res);

        //CHECK(res.bemf == 0.1); // interface seems functional
        //CHECK(res.bemf_scale == 0.2); // interface seems functional
        //CHECK(res.ktau == 0.3); // interface seems functional
        //CHECK(res.ktau_scale == 0.4); // interface seems functional
        CHECK(res.viscousPos == 0.5); // interface seems functional
        CHECK(res.viscousNeg == 0.6); // interface seems functional
        CHECK(res.coulombPos == 0.7); // interface seems functional
        CHECK(res.coulombNeg == 0.8); // interface seems functional
        CHECK(dd_nws.close()); // close dd_nws reported successful
        CHECK(dd_dev.close()); // close dd_dev reported successful
        CHECK(dd2.close()); // close dd2 reported successful
    }
}
