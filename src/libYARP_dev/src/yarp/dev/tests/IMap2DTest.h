/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IMAP2DTEST_H
#define IMAP2DTEST_H

#include <yarp/dev/api.h>
#include <yarp/dev/IMap2D.h>
#include <catch2/catch_amalgamated.hpp>
#include <fstream>
#include <yarp/sig/ImageFile.h>
#include <filesystem>
#include <cstdlib>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iMap2D_test_1(IMap2D* imap)
    {
        {
            //////////"Checking INavigation2D methods
            std::vector<std::string> ll;
            std::vector<std::string> la;
            std::vector<std::string> lp;
            {
                Map2DLocation l1 = Map2DLocation("map_1", 10.0, 20.0, 15);
                Map2DLocation l2 = Map2DLocation("map_1", 10.2, 20.1, 15.5);
                Map2DArea a1("map_1", std::vector<Map2DLocation> {Map2DLocation("map_1", -10, -10, 0),
                    Map2DLocation("map_1", -10, +10, 0),
                    Map2DLocation("map_1", +10, +10, 0),
                    Map2DLocation("map_1", +10, -10, 0)});
                Map2DPath p1(std::vector<Map2DLocation> {l1, l2});
                bool b;
                b = imap->storeArea("area_test1", a1); CHECK(b);
                b = imap->storeLocation("loc_test1", l1); CHECK(b);
                b = imap->storeLocation("loc_test2", l2); CHECK(b);
                b = imap->storePath("path_test1", p1); CHECK(b);
                b = imap->getLocationsList(ll); CHECK(b); CHECK(ll.size() == 2);
                b = imap->getAreasList(la); CHECK(b); CHECK(la.size() == 1);
                b = imap->getPathsList(lp); CHECK(b); CHECK(lp.size() == 1);
            }

            {
                bool b;
                b = imap->saveLocationsAndExtras("locations_test.ini"); CHECK(b);
                b = imap->clearAllLocations(); CHECK(b);
                b = imap->clearAllAreas(); CHECK(b);
                b = imap->clearAllPaths(); CHECK(b);
                b = imap->getLocationsList(ll); CHECK(b); CHECK(ll.size() == 0);
                b = imap->getAreasList(la); CHECK(b); CHECK(la.size() == 0);
                b = imap->getPathsList(lp); CHECK(b); CHECK(lp.size() == 0);

                b = imap->loadLocationsAndExtras("locations_test.ini"); CHECK(b);
                Map2DLocation l1t;
                Map2DLocation l2t;
                Map2DArea a1;
                Map2DPath p1;
                b = imap->getLocationsList(ll); CHECK(b); CHECK(ll.size() == 2);
                b = imap->getAreasList(la); CHECK(b); CHECK(la.size() == 1);
                b = imap->getPathsList(lp); CHECK(b); CHECK(lp.size() == 1);

                b = imap->getLocation("loc_test1", l1t); CHECK(b);
                b = imap->getLocation("loc_test2", l2t); CHECK(b);
                b = imap->getArea("area_test1", a1); CHECK(b);
                b = imap->getPath("path_test1", p1); CHECK(b);

            }
        }
    }

    inline void exec_iMap2D_test_2(IMap2D* imap)
    {
        //////////"Checking IMap2D methods which involve usage of classes Map2DArea or Map2DLocation"
        {
            std::vector <Map2DArea>      areas;
            std::vector <Map2DLocation>  locs;
            std::vector <std::string>    loc_names;
            Map2DLocation loc;
            Map2DArea     area;
            bool ret;
            bool b1;
            ret = imap->clearAllLocations();   CHECK(ret);
            ret = imap->clearAllAreas();       CHECK(ret);
            ret = imap->getAreasList(loc_names);   CHECK(ret);
            b1 = (loc_names.size() == 0);
            CHECK(b1);
            ret = imap->getLocationsList(loc_names);   CHECK(ret);
            b1 = (loc_names.size() == 0);
            CHECK(b1);

            ret = imap->storeLocation("loc1", Map2DLocation("map1", 1, 2, 3));  CHECK(ret);
            ret = imap->storeLocation("loc2", Map2DLocation("map2", 4, 5, 6));  CHECK(ret);

            std::vector<Map2DLocation> vec1, vec2, vec3;
            Map2DLocation v;
            v.x = 1.1; v.y = 1.1; vec1.push_back(v);
            v.x = 1.2; v.y = 1.2; vec1.push_back(v);
            v.x = 1.3; v.y = 1.3; vec1.push_back(v);
            ret = imap->storeArea("area1", Map2DArea("map1", vec1));  CHECK(ret);
            v.x = 2.1; v.y = 2.1; vec2 = vec1;  vec2.push_back(v);
            ret = imap->storeArea("area2", Map2DArea("map2", vec2));  CHECK(ret);
            v.x = 3.1; v.y = 3.1; vec3 = vec2;  vec3.push_back(v);
            ret = imap->storeArea("area3", Map2DArea("map3", vec3));  CHECK(ret);

            ret = imap->getAreasList(loc_names);   CHECK(ret);
            b1 = (loc_names.size() == 3);
            CHECK(b1);
            ret = imap->getLocationsList(loc_names);  CHECK(ret);
            b1 = (loc_names.size() == 2);
            CHECK(b1);

            ret = imap->getArea("area1", area);   CHECK(ret);
            b1 = (area == Map2DArea("map1", vec1));
            CHECK(b1);
            ret = imap->getArea("area_err", area);   CHECK(ret == false);

            ret = imap->getLocation("loc1", loc);  CHECK(ret);
            b1 = (loc == Map2DLocation("map1", 1, 2, 3));
            CHECK(b1);
            ret = imap->getLocation("loc_err", loc);   CHECK(ret == false);

            ret = imap->deleteArea("area1");       CHECK(ret);
            ret = imap->deleteLocation("loc1");  CHECK(ret);
            ret = imap->deleteArea("area_err");       CHECK(ret == false);
            ret = imap->deleteLocation("loc_err");  CHECK(ret == false);
            ret = imap->getArea("area1", area);    CHECK(ret == false);
            ret = imap->getLocation("loc1", loc);  CHECK(ret == false);

            ret = imap->getArea("area2", area);  CHECK(ret);
            b1 = (area == Map2DArea("map2", vec2));
            CHECK(b1);
            ret = imap->getLocation("loc2", loc);   CHECK(ret);
            b1 = (loc == Map2DLocation("map2", 4, 5, 6));
            CHECK(b1);

            ret = imap->clearAllLocations();  CHECK(ret);
            ret = imap->clearAllAreas();  CHECK(ret);
            ret = imap->getAreasList(loc_names);  CHECK(ret);
            b1 = (loc_names.size() == 0);
            CHECK(b1);
            imap->getLocationsList(loc_names);
            b1 = (loc_names.size() == 0);
            CHECK(b1);

            ret = imap->storeArea("area", Map2DArea("map1", vec1));  CHECK(ret);
            ret = imap->storeLocation("loc", Map2DLocation("map2", 4, 5, 6));  CHECK(ret);
            ret = imap->renameArea("area_fail", "area_new");  CHECK(ret == false);
            ret = imap->renameLocation("loc_fail", "loc_new");  CHECK(ret == false);
            ret = imap->renameArea("area", "area_new");  CHECK(ret);
            ret = imap->renameLocation("loc", "loc_new");  CHECK(ret);
            ret = imap->getArea("area", area);         CHECK(ret == false);
            ret = imap->getArea("area_new", area);      CHECK(ret);
            ret = imap->getLocation("loc", loc);       CHECK(ret == false);
            ret = imap->getLocation("loc_new", loc);    CHECK(ret);

            //final cleanup, already tested
            ret = imap->clearAllLocations();  CHECK(ret);
            ret = imap->clearAllAreas();  CHECK(ret);
        }

        //////////"Checking IMap2D methods which involve usage of classes Map2DPath/Map2DLocation"
        {
            std::vector <Map2DPath>      paths;
            std::vector <Map2DLocation>  locs;
            std::vector <std::string>    loc_names;
            std::vector <std::string>    path_names;
            Map2DLocation loc;
            Map2DPath     path;
            bool ret;
            bool b1;
            ret = imap->clearAllLocations();   CHECK(ret);
            ret = imap->clearAllPaths();       CHECK(ret);
            ret = imap->getPathsList(path_names);   CHECK(ret);
            b1 = (path_names.size() == 0);
            CHECK(b1);
            ret = imap->getLocationsList(loc_names);   CHECK(ret);
            b1 = (loc_names.size() == 0);
            CHECK(b1);

            ret = imap->storeLocation("loc1", Map2DLocation("map1", 1, 2, 3));  CHECK(ret);
            ret = imap->storeLocation("loc2", Map2DLocation("map2", 4, 5, 6));  CHECK(ret);

            std::vector<Map2DLocation> vec1, vec2, vec3;
            Map2DLocation v;
            v.x = 1.1; v.y = 1.1; vec1.push_back(v);
            v.x = 1.2; v.y = 1.2; vec1.push_back(v);
            v.x = 1.3; v.y = 1.3; vec1.push_back(v);
            ret = imap->storePath("path1", Map2DPath(vec1));  CHECK(ret);
            v.x = 2.1; v.y = 2.1; vec2 = vec1;  vec2.push_back(v);
            ret = imap->storePath("path2", Map2DPath(vec2));  CHECK(ret);
            v.x = 3.1; v.y = 3.1; vec3 = vec2;  vec3.push_back(v);
            ret = imap->storePath("path3", Map2DPath(vec3));  CHECK(ret);

            ret = imap->getPathsList(path_names);   CHECK(ret);
            b1 = (path_names.size() == 3);
            CHECK(b1);
            ret = imap->getLocationsList(loc_names);  CHECK(ret);
            b1 = (loc_names.size() == 2);
            CHECK(b1);

            ret = imap->getPath("path1", path);   CHECK(ret);
            b1 = (path == Map2DPath(vec1));
            CHECK(b1);
            ret = imap->getPath("path_err", path);   CHECK(ret == false);

            ret = imap->getLocation("loc1", loc);  CHECK(ret);
            b1 = (loc == Map2DLocation("map1", 1, 2, 3));
            CHECK(b1);
            ret = imap->getLocation("loc_err", loc);   CHECK(ret == false);

            ret = imap->deletePath("path1");       CHECK(ret);
            ret = imap->deleteLocation("loc1");  CHECK(ret);
            ret = imap->deletePath("path_err");       CHECK(ret == false);
            ret = imap->deleteLocation("loc_err");  CHECK(ret == false);
            ret = imap->getPath("path1", path);    CHECK(ret == false);
            ret = imap->getLocation("loc1", loc);  CHECK(ret == false);

            ret = imap->getPath("path2", path);  CHECK(ret);
            b1 = (path == Map2DPath(vec2));
            CHECK(b1);
            ret = imap->getLocation("loc2", loc);   CHECK(ret);
            b1 = (loc == Map2DLocation("map2", 4, 5, 6));
            CHECK(b1);

            ret = imap->clearAllLocations();  CHECK(ret);
            ret = imap->clearAllPaths();  CHECK(ret);
            ret = imap->getPathsList(path_names);  CHECK(ret);
            b1 = (path_names.size() == 0);
            CHECK(b1);
            imap->getLocationsList(loc_names);
            b1 = (loc_names.size() == 0);
            CHECK(b1);

            ret = imap->storePath("path", Map2DPath(vec1));  CHECK(ret);
            ret = imap->storeLocation("loc", Map2DLocation("map2", 4, 5, 6));  CHECK(ret);
            ret = imap->renamePath("path_fail", "path_new");  CHECK(ret == false);
            ret = imap->renameLocation("loc_fail", "loc_new");  CHECK(ret == false);
            ret = imap->renamePath("path", "path_new");  CHECK(ret);
            ret = imap->renameLocation("loc", "loc_new");  CHECK(ret);
            ret = imap->getPath("path", path);         CHECK(ret == false);
            ret = imap->getPath("path_new", path);      CHECK(ret);
            ret = imap->getLocation("loc", loc);       CHECK(ret == false);
            ret = imap->getLocation("loc_new", loc);    CHECK(ret);

            std::string s = path.toString();

            //final cleanup, already tested
            ret = imap->clearAllLocations();  CHECK(ret);
            ret = imap->clearAllPaths();  CHECK(ret);
        }

        //////////"Checking IMap2D methods which involve usage of classes MapGrid2D"
        {
            Nav2D::MapGrid2D test_store_map1;
            Nav2D::MapGrid2D test_store_map2;
            Nav2D::MapGrid2D test_get_map;

            test_store_map1.setMapName("test_map1");
            test_store_map2.setMapName("test_map2");
            std::vector<std::string> map_names;

            imap->store_map(test_store_map1);
            imap->store_map(test_store_map2);
            imap->get_map("test_map1", test_get_map);
            CHECK(test_store_map1.isIdenticalTo(test_get_map)); // IMap2D store/get operation successful

            imap->get_map_names(map_names);
            bool b1 = (map_names.size() == 2);
            bool b2 = false;
            if (b1) {
                b2 = (map_names[0] == "test_map1" && map_names[1] == "test_map2");
            }
            CHECK(b1);
            CHECK(b2);
            // IMap2D get_map_names operation successful")

            imap->remove_map("test_map1");
            imap->get_map_names(map_names);
            b1 = (map_names.size() == 1);
            CHECK(b1); // IMap2D remove_map operation successful

            imap->clearAllMaps();
            imap->get_map_names(map_names);
            b1 = (map_names.size() == 0);
            CHECK(b1); // IMap2D clear operation successful
        }
    }

    // Additional tests to cover remaining IMap2D API (objects, getAll*, map persistence, compression, temp flags)
    inline void exec_iMap2D_test_3(IMap2D* imap)
    {
        REQUIRE(imap != nullptr);

        // 1. Object API: store/get/list/all/rename/delete/clear
        {
            bool ret = false;
            // start clean
            ret = imap->clearAllObjects(); CHECK(ret);
            std::vector<std::string> obj_names;
            ret = imap->getObjectsList(obj_names); CHECK(ret); CHECK(obj_names.empty());

            // Campi validi: map_id, x, y, z, roll, pitch, yaw, description
            Map2DObject obj1; obj1.map_id = "map_obj"; obj1.x = 1.0; obj1.y = 2.0; obj1.z = 0.3; obj1.roll = 0.1; obj1.pitch = 0.2; obj1.yaw = 0.5; obj1.description = "an object";
            Map2DObject obj2; obj2.map_id = "map_obj"; obj2.x = 3.0; obj2.y = 4.0; obj2.z = 0.6; obj2.roll = 0.4; obj2.pitch = 0.5; obj2.yaw = 1.5; obj2.description = "another object";
            ret = imap->storeObject("obj1", obj1); CHECK(ret);
            ret = imap->storeObject("obj2", obj2); CHECK(ret);

            ret = imap->getObjectsList(obj_names); CHECK(ret); CHECK(obj_names.size() == 2);

            Map2DObject obj1_r, obj2_r;
            ret = imap->getObject("obj1", obj1_r); CHECK(ret);
            ret = imap->getObject("obj2", obj2_r); CHECK(ret);
            CHECK(obj1_r.x == Catch::Approx(1.0));
            CHECK(obj2_r.x == Catch::Approx(3.0));
            CHECK(obj1_r.yaw == Catch::Approx(0.5));
            CHECK(obj2_r.yaw == Catch::Approx(1.5));

            // getAllObjects
            std::vector<Map2DObject> all_objs;
            ret = imap->getAllObjects(all_objs); CHECK(ret); CHECK(all_objs.size() == 2);

            // renameObject (success)
            ret = imap->renameObject("obj1", "obj1_new"); CHECK(ret);
            ret = imap->getObject("obj1", obj1_r); CHECK_FALSE(ret);
            ret = imap->getObject("obj1_new", obj1_r); CHECK(ret);

            // failing rename
            ret = imap->renameObject("obj_not_exists", "whatever"); CHECK_FALSE(ret);

            // deleteObject success + failure
            ret = imap->deleteObject("obj2"); CHECK(ret);
            ret = imap->deleteObject("obj_unknown"); CHECK_FALSE(ret);
            ret = imap->getObject("obj2", obj2_r); CHECK_FALSE(ret);

            // clearAllObjects finale
            ret = imap->clearAllObjects(); CHECK(ret);
            ret = imap->getObjectsList(obj_names); CHECK(ret); CHECK(obj_names.empty());
        }

    // // 2. Map persistence: single map save + optional collection reload (best-effort)
    //     {
    //         bool ret = false;
    //         Nav2D::MapGrid2D m1; m1.setMapName("zz_unit_persist_map1");
    //         Nav2D::MapGrid2D m2; m2.setMapName("zz_unit_persist_map2");
    //         ret = imap->clearAllMaps(); CHECK(ret);
    //         ret = imap->store_map(m1); CHECK(ret);
    //         ret = imap->store_map(m2); CHECK(ret);

    //         // save individual map (filename relative - depending on device may need to be writable)
    //         ret = imap->saveMapToDisk("zz_unit_persist_map1", "zz_unit_persist_map1.map"); CHECK(ret);
    //         CHECK(std::filesystem::exists("zz_unit_persist_map1.map"));

    //         // save collection
    //         ret = imap->saveMapsCollection("maps_collection.mapset"); CHECK(ret);
    //         CHECK(std::filesystem::exists("maps_collection.mapset"));


    //         // clear and attempt reload collection (do not fail entire test suite if reload fails due to RF path issues)
    //         ret = imap->clearAllMaps(); CHECK(ret);
    //         std::vector<std::string> names; ret = imap->get_map_names(names); CHECK(ret); CHECK(names.empty());
    //         ReturnValue rv_load = imap->loadMapsCollection("maps_collection.mapset");
    //         if (rv_load != 0) {
    //             // Provo ad aggiungere dinamicamente la build dir a YARP_DATA_DIRS e ritentare (ResourceFinder)
    //             std::string cwd = std::filesystem::current_path().string();
    //             setenv("YARP_DATA_DIRS", cwd.c_str(), 1);
    //             rv_load = imap->loadMapsCollection("maps_collection.mapset");
    //         }
    //         CHECK(rv_load == 0); // IMap2D loadMapsCollection operation successful (dopo eventuale retry)
    //         bool ret_names = imap->get_map_names(names); CHECK(ret_names);
    //         if (names.size() < 2) {
    //             // diagnostica d'appoggio: elenco file .map presenti
    //             size_t count_maps = 0;
    //             for (auto& p : std::filesystem::directory_iterator(std::filesystem::current_path())) {
    //                 if (p.path().extension() == ".map") { count_maps++; }
    //             }
    //             INFO("Maps found on disk: " << count_maps);
    //         }
    //         CHECK(names.size() >= 2);
    //         // remove a map then attempt to load it back from single map file
    //         bool ret_rm = imap->remove_map("zz_unit_persist_map1"); CHECK(ret_rm);
    //         ReturnValue rv_single = imap->loadMapFromDisk("zz_unit_persist_map1.map");
    //         CHECK(rv_single == 0); // IMap2D loadMapFromDisk operation successful

    //         // edge case: remove non-existing map (should fail regardless)
    //         bool ret_non = imap->remove_map("no_such_map"); CHECK_FALSE(ret_non);
    //     }

        // 3. Temporary flags clearing (cannot easily set flags here, just call and expect success)
        {
            bool ret = false;
            ret = imap->clearAllMapsTemporaryFlags(); CHECK(ret);
            // ensure map exists first
            Nav2D::MapGrid2D tmpMap; tmpMap.setMapName("temp_flag_map");
            ret = imap->store_map(tmpMap); CHECK(ret);
            ret = imap->clearMapTemporaryFlags("temp_flag_map"); CHECK(ret);
        }

        // 4. Compression toggle
        {
            bool ret = false;
            ret = imap->enableMapsCompression(true); CHECK(ret);
            ret = imap->enableMapsCompression(false); CHECK(ret);
        }

    }

    // Failure paths and legacy formats
    inline void exec_iMap2D_test_4(IMap2D* imap)
    {
        REQUIRE(imap != nullptr);

        // 1. saveMapsCollection with empty storage should fail
        {
            bool ret = imap->clearAllMaps(); CHECK(ret);
            ret = imap->saveMapsCollection("empty_collection.mapset");
            CHECK_FALSE(ret);
        }

        // 2. saveMapToDisk on non existing map -> fail
        {
            bool ret = imap->saveMapToDisk("ghost_map", "ghost.map");
            CHECK_FALSE(ret);
        }

        // 3. loadMapsCollection missing file -> fail
        {
            bool ret = imap->loadMapsCollection("no_such_collection.mapset");
            CHECK_FALSE(ret);
        }

        // 4. loadMapFromDisk missing file -> fail
        {
            bool ret = imap->loadMapFromDisk("no_such_single.map");
            CHECK_FALSE(ret);
        }

        // 5. clearMapTemporaryFlags su mappa inesistente -> fail
        {
            bool ret = imap->clearMapTemporaryFlags("definitely_missing_map");
            CHECK_FALSE(ret);
        }

        // 6. loadLocationsAndExtras legacy versions 1,2,3 + bad version
        {
            // Version 1 file
            {
                std::ofstream f("loc_v1_test.ini");
                f << "Version:\n";
                f << "1\n";
                f << "Locations:\n";
                f << "loc1 mapX 1 2 0\n"; // name map x y theta
                f << "Areas:\n";
                f << "area1 mapX 1 0 0\n"; // name map size x y
                f.close();
                bool ret = imap->loadLocationsAndExtras("loc_v1_test.ini");
                CHECK(ret);
            }
            // Version 2 file
            {
                std::ofstream f("loc_v2_test.ini");
                f << "Version:\n";
                f << "2\n";
                f << "Locations:\n";
                f << "loc2 mapY 3 4 0\n";
                f << "Areas:\n";
                f << "area2 mapY 1 1 1\n";
                f << "Paths:\n";
                f << "path1\n"; // empty path line
                f.close();
                bool ret = imap->loadLocationsAndExtras("loc_v2_test.ini");
                CHECK(ret);
            }
            // Version 3 file (with descriptions)
            {
                std::ofstream f("loc_v3_test.ini");
                f << "Version:\n";
                f << "3\n";
                f << "Locations:\n";
                f << "loc3 mapZ 5 6 0 \"descL\"\n";
                f << "Areas:\n";
                f << "area3 mapZ 1 2 2 \"descA\"\n";
                f << "Paths:\n";
                f << "path3 ( mapZ 0 0 0 ) \"descP\"\n";
                f.close();
                bool ret = imap->loadLocationsAndExtras("loc_v3_test.ini");
                CHECK(ret);
            }
            // Bad version file
            {
                std::ofstream f("loc_bad_version.ini");
                f << "Version:\n";
                f << "99\n"; // unsupported
                f.close();
                bool ret = imap->loadLocationsAndExtras("loc_bad_version.ini");
                CHECK_FALSE(ret);
            }
        }
    }
}

#endif
