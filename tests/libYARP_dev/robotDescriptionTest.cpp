/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <yarp/dev/IRobotDescription.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>

#include <algorithm>
#include <vector>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;

TEST_CASE("dev::robotDescriptionTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("robotDescriptionServer", "device");
    YARP_REQUIRE_PLUGIN("robotDescriptionClient", "device");
    YARP_REQUIRE_PLUGIN("controlboardwrapper2", "device");

    Network::setLocalMode(true);

    SECTION("Test the RobotDescription client/server")
    {
        PolyDriver ddserver;
        Property pserver_cfg;
        pserver_cfg.put("device", "robotDescriptionServer");
        pserver_cfg.put("local", "/robotDescriptionServerPort");
        REQUIRE(ddserver.open(pserver_cfg)); // robotDescriptionServer open reported successful

        IRobotDescription* idesc = nullptr;
        PolyDriver ddclient;
        Property pclient_cfg;
        pclient_cfg.put("device", "robotDescriptionClient");
        pclient_cfg.put("local",  "/robotDescriptionClientPort");
        pclient_cfg.put("remote", "/robotDescriptionServerPort");
        REQUIRE(ddclient.open(pclient_cfg)); // robotDescriptionClient open reported successful

        REQUIRE(ddclient.view(idesc)); // IRobotDescription interface open reported successful

        DeviceDescription dev1; dev1.device_name = "/icubTest/left_arm"; dev1.device_type = "controlboardwrapper2";
        DeviceDescription dev2; dev2.device_name = "/icubTest/left_leg"; dev2.device_type = "controlboardwrapper2";
        DeviceDescription dev3; dev3.device_name = "/icubTest/test";     dev3.device_type = "testDevice";
        idesc->registerDevice(dev1);
        idesc->registerDevice(dev2);
        idesc->registerDevice(dev3);
        std::vector<DeviceDescription> list1;
        std::vector<DeviceDescription> list2;

        idesc->getAllDevices(list1);
        CHECK(list1.size() == 3);
        CHECK(std::find(list1.begin(), list1.end(), dev1) != list1.end());
        CHECK(std::find(list1.begin(), list1.end(), dev2) != list1.end());
        CHECK(std::find(list1.begin(), list1.end(), dev3) != list1.end());
        // IRobotDescription::getAllDevices() successfully tested

        idesc->getAllDevicesByType("controlboardwrapper2", list2);
        CHECK(list2.size() == 2);
        CHECK(std::find(list2.begin(), list2.end(), dev1) != list2.end());
        CHECK(std::find(list2.begin(), list2.end(), dev2) != list2.end());
        CHECK_FALSE(std::find(list2.begin(), list2.end(), dev3) != list2.end());
        // IRobotDescription::getControlBoardWrapperDevices() successfully tested

        // Test unregister device
        idesc->unregisterDevice("/icubTest/test");
        idesc->getAllDevicesByType("testDevice", list2);
        CHECK(list2.size() == 0); // IRobotDescription::unregisterDevice() successfully tested

        // Close devices
        CHECK(ddclient.close()); // robotDescriptionClient successfully closed
        CHECK(ddserver.close()); // robotDescriptionServer successfully closed
    }

    Network::setLocalMode(false);
}
