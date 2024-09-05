/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IRangefinder2D.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IRangefinder2DTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::Rangefinder2DTransformerTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeLaser", "device");
    YARP_REQUIRE_PLUGIN("rangefinder2DTransformer", "device");

    Network::setLocalMode(true);

    SECTION("Checking rangefinder2DTransformer device")
    {
        PolyDriver ddlas;
        PolyDriver ddtrf;
        IRangefinder2D* irng = nullptr;

        ////////"Checking opening polydriver"
        {
            Property plas_cfg;
            plas_cfg.put("device", "fakeLaser");
            plas_cfg.put("test", "use_constant");
            Property& cm_cfg = plas_cfg.addGroup("CONSTANT_MODE");
            cm_cfg.put("const_distance", 0.5);
            REQUIRE(ddlas.open(plas_cfg));
        }
        {
            Property ptrf_cfg;
            ptrf_cfg.put("device", "rangefinder2DTransformer");
            REQUIRE(ddtrf.open(ptrf_cfg));
        }

        //attach the nws to the fakelaser device
        {yarp::dev::WrapperSingle* ww_trf = nullptr; ddtrf.view(ww_trf);
        REQUIRE(ww_trf);
        bool result_att = ww_trf->attach(&ddlas);
        REQUIRE(result_att); }

        REQUIRE(ddtrf.view(irng));

        //execute tests
        yarp::dev::tests::exec_iRangefinder2D_test_1 (irng);

        //Close all polydrivers and check
        CHECK(ddtrf.close());
        yarp::os::Time::delay(0.1);
        INFO("rangefinder2DTransformer closed");

        CHECK(ddlas.close());
        yarp::os::Time::delay(0.1);
        INFO("fakeLaser closed");

        INFO("Test complete");
    }

    Network::setLocalMode(false);
}
