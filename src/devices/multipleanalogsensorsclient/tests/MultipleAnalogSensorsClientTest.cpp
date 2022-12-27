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

#include <catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;


TEST_CASE("dev::MultipleAnalogSensorsClientTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeIMU", "device");
    YARP_REQUIRE_PLUGIN("multipleanalogsensorsserver", "device");
    YARP_REQUIRE_PLUGIN("multipleanalogsensorsclient", "device");

//#if defined(DISABLE_FAILING_TESTS)
//    YARP_SKIP_TEST("Skipping failing tests")
//#endif

    Network::setLocalMode(true);

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

        // Open the client
        Property pClient;
        pClient.put("device", "multipleanalogsensorsclient");
        pClient.put("remote", serverPrefix);
        pClient.put("local", "/test/mas/client");
        // Increase timeout time because we don't know the load of the machine on which the test will run
        pClient.put("timeout", 1.0);

        PolyDriver client;
        REQUIRE(client.open(pClient)); // multipleanalogsensorsclient open reported successful

        // Let make sure that the data get read by the client
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Compare the readings of actual device and client device
        yarp::dev::IOrientationSensors* clientOrientSens;
        REQUIRE(client.view(clientOrientSens)); // IOrientationSensors of multipleanalogsensorsclient correctly opened

        yarp::dev::tests::exec_IOrientationSensors_test_1(clientOrientSens);

        // Close devices
        client.close();
        INFO("multipleanalogsensorsclient closed");
        yarp::os::Time::delay(0.1);

        iwrap->detachAll();
        wrapper.close();
        INFO("multipleanalogsensorsserver closed");
        yarp::os::Time::delay(0.1);

        imuSensor.close();
        INFO("fakeIMU closed");
        yarp::os::Time::delay(0.1);

        INFO("Test complete");
    }

    Network::setLocalMode(false);
}
