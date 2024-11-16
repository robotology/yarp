/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DArea.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IMap2DTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::Map2DnwcTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("map2D_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("map2D_nwc_yarp", "device");
    YARP_REQUIRE_PLUGIN("map2DStorage", "device");

    Network::setLocalMode(true);

    SECTION("Checking map2D_nwc_yarp <-> map2D_nws_yarp communication and yarp::dev::Nav2D::IMap2D methods")
    {
        PolyDriver ddmapserver;
        PolyDriver ddmapstorage;
        PolyDriver ddmapclient;
        IMap2D* imap = nullptr;

        ////////"Checking opening map2DServer and map2DClient polydrivers"
        {
            Property pmapserver_cfg;
            pmapserver_cfg.put("device", "map2D_nws_yarp");
            REQUIRE(ddmapserver.open(pmapserver_cfg));

            Property pmapstorage_cfg;
            pmapstorage_cfg.put("device", "map2DStorage");
            REQUIRE(ddmapstorage.open(pmapstorage_cfg));

            {yarp::dev::WrapperSingle* ww_nws=nullptr; ddmapserver.view(ww_nws);
            REQUIRE(ww_nws);
            bool result_att = ww_nws->attach(&ddmapstorage);
            REQUIRE(result_att); }

            Property pmapclient_cfg;
            pmapclient_cfg.put("device", "map2D_nwc_yarp");
            pmapclient_cfg.put("local", "/mapClientTest");
            pmapclient_cfg.put("remote", "/map2D_nws_yarp");
            REQUIRE(ddmapclient.open(pmapclient_cfg));
            REQUIRE(ddmapclient.view(imap));
        }

        //execute tests
        yarp::dev::tests::exec_iMap2D_test_1 (imap);
        yarp::dev::tests::exec_iMap2D_test_2 (imap);

        //"Close all polydrivers and check"
        {
            CHECK(ddmapclient.close());
            yarp::os::Time::delay(0.1);
            CHECK(ddmapserver.close());
            yarp::os::Time::delay(0.1);
            CHECK(ddmapstorage.close());
        }
    }

    Network::setLocalMode(false);
}
