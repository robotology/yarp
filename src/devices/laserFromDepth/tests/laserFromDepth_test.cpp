/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IRangefinder2D.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IRangefinder2DTest.h>
#include <yarp/dev/tests/ParametersTest.h>
#include <yarp/dev/tests/TestUtils.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::laserFromDepth", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("laserFromDepth", "device");
    YARP_REQUIRE_PLUGIN("fakeDepthCamera", "device");
    YARP_REQUIRE_PLUGIN("RGBDSensor_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking laserFromDepth device")
    {
        PolyDriver dd_rgbd_fake;
        PolyDriver dd_rgbd_nws;
        Property p_rgbd_fake;
        Property p_rgbd_nws;

        //open a fake source of rgbd images
        {
            p_rgbd_fake.put("device", "fakeDepthCamera");
            // small values to improve valgrind speed
            p_rgbd_fake.put("rgb_w", 32);
            p_rgbd_fake.put("rgb_h", 24);
            p_rgbd_fake.put("dep_w", 32);
            p_rgbd_fake.put("dep_h", 24);
            REQUIRE(dd_rgbd_fake.open(p_rgbd_fake));

            p_rgbd_nws.put("device", "RGBDSensor_nws_yarp");
            p_rgbd_nws.put("name", "/rgbd_nws");
            REQUIRE(dd_rgbd_nws.open(p_rgbd_nws));

            yarp::dev::tests::wait_safe();

            // attach
            {
                yarp::dev::WrapperSingle* ww_nws;
                dd_rgbd_nws.view(ww_nws);
                REQUIRE(ww_nws);
                bool result_att = ww_nws->attach(&dd_rgbd_fake);
                REQUIRE(result_att);
            }
        }

        PolyDriver laserdev;
        IRangefinder2D* irng = nullptr;

        ////////"Checking opening polydriver"
        {
            Property las_cfg;
            las_cfg.put("device", "laserFromDepth");

            auto& rgbdprop = las_cfg.addGroup("RGBD_SENSOR_CLIENT");
            rgbdprop.put("localImagePort", "/rgbd_nwc/rgbImage:i");
            rgbdprop.put("remoteImagePort", "/rgbd_nws/rgbImage:o");
            rgbdprop.put("localDepthPort", "/rgbd_nwc/depthImage:i");
            rgbdprop.put("remoteDepthPort", "/rgbd_nws/depthImage:o");
            rgbdprop.put("localRpcPort", "/rgbd_nwc/rpc:o");
            rgbdprop.put("remoteRpcPort", "/rgbd_nws/rpc:i");

            REQUIRE(laserdev.open(las_cfg));
            REQUIRE(laserdev.view(irng));
        }
        //@@@not available yet
        //yarp::dev::tests::exec_params_test(&laserdev);

        //execute tests
        yarp::dev::tests::ValuestoCheck vals;
        vals.test_scanrate=0;
        vals.test_min=0.1;
        vals.test_max=2.5;
        vals.test_horizontal_res=1.125;
        vals.test_lsize=32;
        vals.test_rho = 0.56541570060620461;
        vals.test_theta=-0.31415926535897931;
        vals.test_cartesian_x=0.537742;
        vals.test_cartesian_y=-0.174723;
        //yarp::dev::tests::exec_iRangefinder2D_test_1(irng, vals);

        //"Close all polydrivers and check"
        CHECK(laserdev.close());
    }

    Network::setLocalMode(false);
}
