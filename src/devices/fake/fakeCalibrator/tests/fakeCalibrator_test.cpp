/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/tests/IRemoteCalibratorTest.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeCalibrator", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeCalibrator", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeCalibrator device")
    {
        PolyDriver dd;
        yarp::dev::IRemoteCalibrator* icalib=nullptr;

        ////////"Checking opening device polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeCalibrator");
            REQUIRE(dd.open(p_cfg));
        }

        dd.view(icalib);
        yarp::dev::tests::exec_iRemoteCalibrator_test_1(icalib);

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    Network::setLocalMode(false);
}
