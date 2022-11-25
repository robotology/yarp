/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/tests/IOrientationSensorsTest.h>
#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>

#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <cmath>
#include <chrono>
#include <thread>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;


TEST_CASE("dev::MultipleAnalogSensorsServerTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeIMU", "device");
    YARP_REQUIRE_PLUGIN("multipleanalogsensorsserver", "device");

//#if defined(DISABLE_FAILING_TESTS)
//    YARP_SKIP_TEST("Skipping failing tests")
//#endif

    Network::setLocalMode(true);

    SECTION("Test the nws alone")
    {
        PolyDriver wrapper;

        Property pWrapper;
        pWrapper.put("device", "multipleanalogsensorsserver");
        std::string serverPrefix = "/test/mas/server";
        pWrapper.put("name", serverPrefix);
        pWrapper.put("period", 10);
        REQUIRE(wrapper.open(pWrapper)); // multipleanalogsensorsserver open reported successful

        // Close devices
        wrapper.close();
    }

    SECTION("Test the multiple analog sensors device on a single IMU (deferred attach)")
    {
        // We first allocate a single fakeImu
        PolyDriver imuSensor;
        PolyDriver wrapper;

        Property p;
        p.put("device", "fakeIMU");
        p.put("constantValue", 1);

        REQUIRE(imuSensor.open(p)); // sensor open reported successful

        yarp::dev::IOrientationSensors* orientSens;
        REQUIRE(imuSensor.view(orientSens)); // IOrientationSensors of fakeIMU correctly opened
        int nrOfSensors = orientSens->getNrOfOrientationSensors();
        CHECK(nrOfSensors == 1); // getNrOfOrientationSensors of fakeIMU works correctly

        Property pWrapper;
        pWrapper.put("device", "multipleanalogsensorsserver");
        std::string serverPrefix = "/test/mas/server";
        pWrapper.put("name", serverPrefix);
        pWrapper.put("period", 10);
        REQUIRE(wrapper.open(pWrapper)); // multipleanalogsensorsserver open reported successful

        yarp::dev::IMultipleWrapper *iwrap = nullptr;
        REQUIRE(wrapper.view(iwrap)); // IMultipleWrapper interface correctly opened for the multipleanalogsensorsserver

        PolyDriverList pdList;
        pdList.push(&imuSensor, "pdlist_key");
        REQUIRE(iwrap->attachAll(pdList)); // multipleanalogsensorsserver attached successfully to the device

        // Close devices
        iwrap->detachAll();
        wrapper.close();
        imuSensor.close();
    }

    Network::setLocalMode(false);
}
