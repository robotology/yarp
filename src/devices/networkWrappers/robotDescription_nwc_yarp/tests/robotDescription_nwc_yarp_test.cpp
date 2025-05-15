/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <yarp/dev/IRobotDescription.h>

#include <yarp/dev/IWrapper.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>

#include <algorithm>
#include <vector>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;

TEST_CASE("dev::robotDescriptionClientTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("robotDescription_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("robotDescription_nwc_yarp", "device");
    YARP_REQUIRE_PLUGIN("robotDescriptionStorage", "device");
    YARP_REQUIRE_PLUGIN("controlBoard_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Test the robotDescriptionClient device")
    {
        PolyDriver ddstorage;
        Property pstorage_cfg;
        pstorage_cfg.put("device", "robotDescriptionStorage");
        REQUIRE(ddstorage.open(pstorage_cfg)); // robotDescriptionStorage open reported successful

        PolyDriver ddnws;
        Property pserver_cfg;
        pserver_cfg.put("device", "robotDescription_nws_yarp");
        pserver_cfg.put("local", "/robotDescription_nws_yarp/rpc");
        REQUIRE(ddnws.open(pserver_cfg)); // robotDescription_nws_yarp open reported successful

        yarp::dev::IWrapper* ww_nws = nullptr; ddnws.view(ww_nws);
        REQUIRE(ww_nws);
        bool result_att = ww_nws->attach(&ddstorage);
        REQUIRE(result_att);

        IRobotDescription* idesc = nullptr;
        PolyDriver ddnwc;
        Property pclient_cfg;
        pclient_cfg.put("device", "robotDescription_nwc_yarp");
        pclient_cfg.put("local",  "/robotDescription_nwc_yarp/rpc");
        pclient_cfg.put("remote", "/robotDescription_nws_yarp/rpc");
        REQUIRE(ddnwc.open(pclient_cfg)); // robotDescription_nwc_yarp open reported successful

        REQUIRE(ddnwc.view(idesc)); // IRobotDescription interface open reported successful
        REQUIRE(idesc);

        DeviceDescription dev1; dev1.device_name = "/icubTest/left_arm"; dev1.device_type = "controlBoard_nws_yarp";
        DeviceDescription dev2; dev2.device_name = "/icubTest/left_leg"; dev2.device_type = "controlBoard_nws_yarp";
        DeviceDescription dev3; dev3.device_name = "/icubTest/test";     dev3.device_type = "testDevice";
        auto r1 = idesc->registerDevice(dev1);
        auto r2 = idesc->registerDevice(dev2);
        auto r3 = idesc->registerDevice(dev3);
        CHECK(r1);
        CHECK(r2);
        CHECK(r3);
        std::vector<DeviceDescription> list1;
        std::vector<DeviceDescription> list2;

        auto r4 = idesc->getAllDevices(list1);
        CHECK(r4);
        CHECK(list1.size() == 3);
        CHECK(std::find(list1.begin(), list1.end(), dev1) != list1.end());
        CHECK(std::find(list1.begin(), list1.end(), dev2) != list1.end());
        CHECK(std::find(list1.begin(), list1.end(), dev3) != list1.end());
        // IRobotDescription::getAllDevices() successfully tested

        auto r5 = idesc->getAllDevicesByType("controlBoard_nws_yarp", list2);
        CHECK(r5);
        CHECK(list2.size() == 2);
        CHECK(std::find(list2.begin(), list2.end(), dev1) != list2.end());
        CHECK(std::find(list2.begin(), list2.end(), dev2) != list2.end());
        CHECK_FALSE(std::find(list2.begin(), list2.end(), dev3) != list2.end());
        // IRobotDescription::getControlBoardWrapperDevices() successfully tested

        // Test unregister device
        auto r6 = idesc->unregisterDevice("/icubTest/test");
        auto r7 = idesc->getAllDevicesByType("testDevice", list2);
        CHECK(r6);
        CHECK(r7);
        CHECK(list2.size() == 0); // IRobotDescription::unregisterDevice() successfully tested

        // Test clear all
        auto r8 = idesc->unregisterAll();
        CHECK(r8);

        // Close devices
        CHECK(ddnwc.close());     // robotDescription_nwc_yarp successfully closed
        CHECK(ddnws.close());     // robotDescription_nws_yarp successfully closed
        CHECK(ddstorage.close()); // robotDescriptionStorage successfully closed
    }

    Network::setLocalMode(false);
}
