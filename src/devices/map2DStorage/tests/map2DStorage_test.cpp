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

    SECTION("Checking map2DStorage device loading files (1/3)")
    {
        PolyDriver ddmapstorage;
        IMap2D* imap = nullptr;

        ////////"Checking opening polydriver"
        // This will fail because map2DStorage cannot find a
        //`maps_collection.ini` file in `erraticContext`.
        {
            Property pmapstorage_cfg;
            pmapstorage_cfg.put("device", "map2DStorage");
            pmapstorage_cfg.put("mapCollectionContext", "erraticContext");
            REQUIRE(!ddmapstorage.open(pmapstorage_cfg));
        }
    }

    SECTION("Checking map2DStorage device loading files (2/3)")
    {
        PolyDriver ddmapstorage;
        IMap2D* imap = nullptr;

        //open the poly loading a previously saved collection
        {
            Property pmapstorage_cfg;
            pmapstorage_cfg.put("device", "map2DStorage");
            pmapstorage_cfg.put("mapCollectionContext", "mapGrid2DTest");
            REQUIRE(ddmapstorage.open(pmapstorage_cfg));
            REQUIRE(ddmapstorage.view(imap));

            bool b;
            std::vector<Nav2D::Map2DArea> veca;
            std::vector<Nav2D::Map2DLocation> vecl;
            std::vector<Nav2D::Map2DObject> veco;
            std::vector<Nav2D::Map2DPath> vecp;
            b = imap->getAllAreas(veca); CHECK(b); CHECK(veca.size()==3);
            b = imap->getAllLocations(vecl); CHECK(b); CHECK(vecl.size()==3);
            b = imap->getAllObjects(veco); CHECK(b); CHECK(veco.size()==3);
            b = imap->getAllPaths(vecp); CHECK(b); CHECK(vecp.size()==3);
            //Cleanup of vectors which contain elements allocated inside a plugin.
            //beware! Forgetting this might cause segfault
            veca.clear();
            vecl.clear();
            veco.clear();
            vecp.clear();

            //"Close all polydrivers and check"
            CHECK(ddmapstorage.close());
        }
    }

    SECTION("Checking map2DStorage device loading files (3/3)")
    {
        PolyDriver ddmapstorage;
        IMap2D* imap = nullptr;

        //open a poly, creates a collection from scratch, close the poly
        {
            Property pmapstorage_cfg;
            pmapstorage_cfg.put("device", "map2DStorage");
            REQUIRE(ddmapstorage.open(pmapstorage_cfg));
            REQUIRE(ddmapstorage.view(imap));
            yarp::dev::tests::create_test_collection(imap);
            CHECK(ddmapstorage.close());
        }

        //open the poly loading a previously saved collection
        {
            Property pmapstorage_cfg;
            pmapstorage_cfg.put("device", "map2DStorage");
            pmapstorage_cfg.put("mapCollectionContext", "exampleContext");
            REQUIRE(ddmapstorage.open(pmapstorage_cfg));
            REQUIRE(ddmapstorage.view(imap));

            //execute tests
            yarp::dev::tests::exec_iMap2D_test_1(imap);
            yarp::dev::tests::exec_iMap2D_test_2(imap);

            //"Close all polydrivers and check"
            CHECK(ddmapstorage.close());
        }

        //remove the collection
        yarp::dev::tests::remove_test_collection();
    }

    Network::setLocalMode(false);
}
