/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

#include <yarp/sig/Sound.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::speechSynthesizer_nws_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeSpeechSynthesizer", "device");
    YARP_REQUIRE_PLUGIN("speechSynthesizer_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking speechSynthesizer_nws_yarp device alone")
    {
        PolyDriver dd;

        ////////"Checking opening polydriver with no attached device"
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "speechSynthesizer_nws_yarp");
            pnws_cfg.put("name", "/speechSynthesizer_nws");
            REQUIRE(dd.open(pnws_cfg));
        }

        yarp::os::Time::delay(1.0);

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    SECTION("Checking speechSynthesizer_nws_yarp device attached to fake device")
    {
        PolyDriver ddnws;
        PolyDriver ddfake;

        ////////"Checking opening polydriver and attach device"
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "speechSynthesizer_nws_yarp");
            pnws_cfg.put("name", "/speechSynthesizer_nws");
            REQUIRE(ddnws.open(pnws_cfg));

            Property pdev_cfg;
            pdev_cfg.put("device", "fakeSpeechSynthesizer");
            REQUIRE(ddfake.open(pdev_cfg));

            {yarp::dev::WrapperSingle* ww_nws=nullptr; ddnws.view(ww_nws);
            REQUIRE(ww_nws);
            bool result_att = ww_nws->attach(&ddfake);
            REQUIRE(result_att); }
        }

        //tests
        {
            Port po;
            Port pi;
            CHECK(po.open("/test:o"));
            CHECK(Network::connect("/test:o","/speechSynthesizer_nws/text:i"));
            CHECK(pi.open("/test:i"));
            CHECK(Network::connect("/speechSynthesizer_nws/sound:o", "/test:i"));
            yarp::os::Time::delay(0.5);

            yarp::os::Bottle b;
            b.addString("hello world");
            CHECK(po.write(b));
            yarp::os::Time::delay(0.5);
            yarp::sig::Sound ss;
            CHECK(pi.read(ss));
            CHECK(ss.getChannels() == 2);
            CHECK(ss.getSamples() == 100);

            po.close();
            pi.close();
        }
        yarp::os::Time::delay(0.5);

        //"Close all polydrivers and check"
        {
            CHECK(ddnws.close());
            CHECK(ddfake.close());
        }
    }

    Network::setLocalMode(false);
}
