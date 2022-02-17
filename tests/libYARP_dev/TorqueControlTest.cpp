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

TEST_CASE("dev::TorqueControl", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("group", "device");
    YARP_REQUIRE_PLUGIN("fakeMotionControl", "device");
    YARP_REQUIRE_PLUGIN("controlBoard_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("remote_controlboard", "device");

    Network::setLocalMode(true);

    SECTION("test the controlBoard_nws_yarp device")
    {
        PolyDriver dd;
        Property p;
        p.put("device","controlBoard_nws_yarp");
        p.put("subdevice","fakeMotionControl");
        p.put("name","/motor");
        auto& pg = p.addGroup("GENERAL");
        pg.put("Joints", 1);
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

        ITorqueControl *trq = nullptr;
        result = dd2.view(trq);
        REQUIRE(result); // interface reported

        yarp::dev::MotorTorqueParameters params;
        yarp::dev::MotorTorqueParameters res;

        //params.bemf = 0.1;
        //params.bemf_scale = 0.2;
        //params.ktau = 0.3;
        //params.ktau_scale = 0.4;
        params.viscousUp = 0.5;
        params.viscousDown = 0.6;
        params.coulombUp = 0.7;
        params.coulombDown = 0.8;

        trq->setMotorTorqueParams(0, params);
        trq->getMotorTorqueParams(0, &res);
        
        //CHECK(res.bemf == 0.1); // interface seems functional
        //CHECK(res.bemf_scale == 0.2); // interface seems functional
        //CHECK(res.ktau == 0.3); // interface seems functional
        //CHECK(res.ktau_scale == 0.4); // interface seems functional
        CHECK(res.viscousUp == 0.5); // interface seems functional
        CHECK(res.viscousDown == 0.6); // interface seems functional
        CHECK(res.coulombUp == 0.7); // interface seems functional
        CHECK(res.coulombDown == 0.8); // interface seems functional
        CHECK(dd.close()); // close dd reported successful
        CHECK(dd2.close()); // close dd2 reported successful
    }
}
