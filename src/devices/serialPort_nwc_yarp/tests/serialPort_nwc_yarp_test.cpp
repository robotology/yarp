/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ISerialDevice.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::serialPort_nwc_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeSerialPort", "device");
    YARP_REQUIRE_PLUGIN("serialPort_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("serialPort_nwc_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking serialPort_nwc_yarp device")
    {
        yarp::dev::ISerialDevice* iser=nullptr;
        PolyDriver ddnws;
        PolyDriver ddfake;
        PolyDriver ddnwc;

        {
            Property pnws_cfg;
            pnws_cfg.put("device", "serialPort_nws_yarp");
            pnws_cfg.put("name", "/serialPort_nws_yarp");
            REQUIRE(ddnws.open(pnws_cfg));

            Property pdev_cfg;
            pdev_cfg.put("device", "fakeSerialPort");
            REQUIRE(ddfake.open(pdev_cfg));

            {yarp::dev::WrapperSingle* ww_nws; ddnws.view(ww_nws);
            bool result_att = ww_nws->attach(&ddfake);
            REQUIRE(result_att); }

            Property pnwc_cfg;
            pnwc_cfg.put("device", "serialPort_nwc_yarp");
            pnwc_cfg.put("local", "/serialPort_nwc_yarp");
            pnwc_cfg.put("remote", "/serialPort_nws_yarp/rpc");
            REQUIRE(ddnwc.open(pnwc_cfg));
            REQUIRE(ddnwc.view(iser));
        }

        //tests
        {
            CHECK (iser->setDTR(true));
            yarp::os::Time::delay(0.1);
            std::string s("fake_command");
            CHECK (iser->send(s.c_str(),s.size()));
        }

        //"Close all polydrivers and check"
        {
            CHECK(ddnwc.close());
            yarp::os::Time::delay(0.1);
            CHECK(ddnws.close());
            yarp::os::Time::delay(0.1);
            CHECK(ddfake.close());
        }
    }

    Network::setLocalMode(false);
}
