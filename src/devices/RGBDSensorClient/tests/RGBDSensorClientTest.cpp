/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/tests/IRGBDSensorTest.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::RGBDSensorClientTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeDepthCamera", "device");
    YARP_REQUIRE_PLUGIN("rgbdSensor_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("RGBDSensorClient", "device");

    Network::setLocalMode(true);

    SECTION("Checking RGBDSensorClient device")
    {
        PolyDriver dddepth;
        PolyDriver ddnws;
        PolyDriver ddnwc;
        IRGBDSensor* irgbd = nullptr;

        ////////"Checking opening polydriver"
        {
            Property pdepth_cfg;
            pdepth_cfg.put("device", "fakeDepthCamera");
            REQUIRE(dddepth.open(pdepth_cfg));
        }
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "rgbdSensor_nws_yarp");
            pnws_cfg.put("name",   "/rgbd_nws");
            REQUIRE(ddnws.open(pnws_cfg));
        }

        //attach the nws to the fakeDepthCamera device
        {yarp::dev::WrapperSingle* ww_nws; ddnws.view(ww_nws);
        bool result_att = ww_nws->attach(&dddepth);
        REQUIRE(result_att); }

        //wait some time
        yarp::os::Time::delay(0.1);

        //create the client
        {
            Property pnwc_cfg;
            pnwc_cfg.put("device", "RGBDSensorClient");
            pnwc_cfg.put("localImagePort",  "/rgbd_nwc/rgbImage:i");
            pnwc_cfg.put("remoteImagePort", "/rgbd_nws/rgbImage:o");

            pnwc_cfg.put("localDepthPort",  "/rgbd_nwc/depthImage:i");
            pnwc_cfg.put("remoteDepthPort", "/rgbd_nws/depthImage:o");

            pnwc_cfg.put("localRpcPort",    "/rgbd_nwc/rpc:o");
            pnwc_cfg.put("remoteRpcPort",   "/rgbd_nws/rpc:i");
            REQUIRE(ddnwc.open(pnwc_cfg));
        }
        REQUIRE(ddnwc.view(irgbd));

        //execute tests
        yarp::dev::tests::exec_iRGBDSensor_test_1(irgbd);

        //Close all polydrivers and check
        CHECK(ddnwc.close());
        yarp::os::Time::delay(0.1);
        INFO("RGBDSensorClient closed");

        CHECK(ddnws.close());
        yarp::os::Time::delay(0.1);
        INFO("rgbdSensor_nws_yarp closed");

        CHECK(dddepth.close());
        yarp::os::Time::delay(0.1);
        INFO("fakeDepthCamera closed");

        INFO("Test complete");
    }

    Network::setLocalMode(false);
}
