/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IRobotDescription.h>
#include <yarp/dev/WrapperMultiple.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>

#include <algorithm>
#include <vector>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;

TEST_CASE("dev::robotDescriptionStorage_Test", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("robotDescriptionStorage", "device");
    YARP_REQUIRE_PLUGIN("fakeDeviceUnwrapped", "device");

    Network::setLocalMode(true);

    SECTION("Test the robotDescriptionStorage device")
    {
        PolyDriver ddstor;
        Property pcfg;
        IRobotDescription* idesc = nullptr;
        pcfg.put("device", "robotDescriptionStorage");
        REQUIRE(ddstor.open(pcfg)); // robotDescriptionStorage open reported successful

        REQUIRE(ddstor.view(idesc)); // IRobotDescription interface open reported successful
        REQUIRE(idesc);

        DeviceDescription devc;
        devc.device_name = "test_dev_name";
        devc.device_type = "device_type";
        yarp::dev::ReturnValue r = idesc->registerDevice(devc);
        CHECK(r);
        std::vector<DeviceDescription> list;
        r = idesc->getAllDevices(list);
        CHECK(r);
        CHECK(list.size() == 1);
        CHECK(list[0].device_name == "test_dev_name");
        CHECK(list[0].device_type == "device_type");

        // Close devices
        CHECK(ddstor.close()); // robotDescriptionStorage successfully closed
    }

    SECTION("Test the robotDescriptionStorage device with attach")
    {
        PolyDriver dd_adevice;
        Property p_cfg_adevice;
        p_cfg_adevice.put("device", "fakeDeviceUnwrapped");
        REQUIRE(dd_adevice.open(p_cfg_adevice));

        PolyDriver ddstor;
        Property pcfg;
        pcfg.put("device", "robotDescriptionStorage");
        REQUIRE(ddstor.open(pcfg)); // robotDescription_nws_yarp open reported successful

        yarp::dev::IMultipleWrapper* ww_nws = nullptr; ddstor.view(ww_nws);
        REQUIRE(ww_nws);
        yarp::dev::PolyDriverList drivers_list;
        drivers_list.push(&dd_adevice,"fakedev");
        bool result_att = ww_nws->attachAll(drivers_list);
        REQUIRE(result_att);

        yarp::os::Time::delay(1.0);

        // Close devices
        CHECK(ddstor.close()); // robotDescription_nws_yarp successfully closed
        CHECK(dd_adevice.close());
    }

    Network::setLocalMode(false);
}
