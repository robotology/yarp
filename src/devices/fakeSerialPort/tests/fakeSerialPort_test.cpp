/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ISerialDevice.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
//#include <yarp/dev/tests/ILocalization2DTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeSerialPort", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeSerialPort", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeSerialPort device")
    {
        yarp::dev::ISerialDevice* iser=nullptr;
        PolyDriver ddfake;

        //open the device
        {

            Property pdev_cfg;
            pdev_cfg.put("device", "fakeSerialPort");
            REQUIRE(ddfake.open(pdev_cfg));
            REQUIRE(ddfake.view(iser));
            REQUIRE(iser);
        }

        iser->flush();
        //yarp::dev::tests::exec_iLocalization2D_test_1(iser);

        //"Close all polydrivers and check"
        {
            yarp::os::Time::delay(0.1);
            CHECK(ddfake.close());
        }
    }

    Network::setLocalMode(false);
}
