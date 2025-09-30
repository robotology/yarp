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

TEST_CASE("dev::Map2DStorageTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("map2DStorage", "device");

    Network::setLocalMode(true);

    SECTION("Checking map2DStorage device without loading files")
    {
        PolyDriver ddmapstorage;
        IMap2D* imap = nullptr;

        ////////"Checking opening polydriver"
        {
            Property pmapstorage_cfg;
            pmapstorage_cfg.put("device", "map2DStorage");
            REQUIRE(ddmapstorage.open(pmapstorage_cfg));
            REQUIRE(ddmapstorage.view(imap));
        }

        //execute tests
        yarp::dev::tests::exec_iMap2D_test_1 (imap);
        yarp::dev::tests::exec_iMap2D_test_2 (imap);
        yarp::dev::tests::exec_iMap2D_test_3 (imap);
        yarp::dev::tests::exec_iMap2D_test_4 (imap);

        //"Close all polydrivers and check"
        {
            CHECK(ddmapstorage.close());
        }
    }

    SECTION("Checking map2DStorage device loading files (1/2)")
    {
        PolyDriver ddmapstorage;
        IMap2D* imap = nullptr;

        ////////"Checking opening polydriver"
        {
            Property pmapstorage_cfg;
            pmapstorage_cfg.put("device", "map2DStorage");
            pmapstorage_cfg.put("mapCollectionContext", "erraticContext");
            REQUIRE(!ddmapstorage.open(pmapstorage_cfg));
        }
    }

    /*
    SECTION("Checking map2DStorage device loading files (2/2)")
    {
        PolyDriver ddmapstorage;
        IMap2D* imap = nullptr;

        ////////"Checking opening polydriver"
        {
            Property pmapstorage_cfg;
            pmapstorage_cfg.put("device", "map2DStorage");
            pmapstorage_cfg.put("mapCollectionContext", "exampleContext");
            REQUIRE(ddmapstorage.open(pmapstorage_cfg));
            REQUIRE(ddmapstorage.view(imap));
        }

        //execute tests
        yarp::dev::tests::exec_iMap2D_test_1(imap);
        yarp::dev::tests::exec_iMap2D_test_2(imap);

        //"Close all polydrivers and check"
        {
            CHECK(ddmapstorage.close());
        }
    }
    */

    Network::setLocalMode(false);
}
