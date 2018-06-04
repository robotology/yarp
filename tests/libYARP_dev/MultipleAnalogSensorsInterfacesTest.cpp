/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cmath>

#include <vector>
#include <thread>

#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/Time.h>

#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/dev/Wrapper.h>

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::dev;

class MultipleAnalogSensorsInterfacesTest : public UnitTest
{
public:
    virtual std::string getName() const override { return "MultipleAnalogSensorsInterfacesTest"; }

    void checkRemapperOnSingleIMU()
    {

    }

    void testServerClientOnSingleIMU() {
        report(0,"\ntest the multiple analog sensors device on a single IMU");

        // We first allocate a single fakeImu
        PolyDriver imuSensor;
        PolyDriver wrapper;

        Property p;
        p.put("device", "fakeIMU");
        p.put("constantValue", 1);

        bool result;
        result = imuSensor.open(p);
        checkTrue(result, "sensor open reported successful");

        yarp::dev::IOrientationSensors* orientSens;
        result = imuSensor.view(orientSens);
        checkTrue(result, "IOrientationSensors of fakeIMU correctly opened");
        int nrOfSensors = orientSens->getNrOfOrientationSensors();
        checkEqual(nrOfSensors, 1, "getNrOfOrientationSensors of fakeIMU works correctly");

        Property pWrapper;
        pWrapper.put("device", "multipleanalogsensorsserver");
        std::string serverPrefix = "/test/mas/server";
        pWrapper.put("name", serverPrefix);
        pWrapper.put("period", 10);
        result = wrapper.open(pWrapper);
        checkTrue(result, "multipleanalogsensorsserver open reported successful");

        yarp::dev::IMultipleWrapper *iwrap = nullptr;
        result = wrapper.view(iwrap);
        checkTrue(result, "IMultipleWrapper interface correctly opened for the multipleanalogsensorsserver");

        PolyDriverList pdList;
        pdList.push(&imuSensor, "pdlist_key");

        result = iwrap->attachAll(pdList);
        checkTrue(result, "multipleanalogsensorsserver attached successfully to the device");

        // Open the client
        Property pClient;
        pClient.put("device", "multipleanalogsensorsclient");
        pClient.put("remote", serverPrefix);
        pClient.put("local", "/test/mas/client");
        // Increase timeout time because we don't know the load of the machine on which the test will run
        pClient.put("timeout", 1.0);

        PolyDriver client;
        result = client.open(pClient);
        checkTrue(result, "multipleanalogsensorsclient open reported successful");

        // Let make sure that the data get read by the client
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Compare the readings of actual device and client device
        yarp::dev::IOrientationSensors* clientOrientSens;
        result = client.view(clientOrientSens);
        checkTrue(result, "IOrientationSensors of multipleanalogsensorsclient correctly opened");
        nrOfSensors = clientOrientSens->getNrOfOrientationSensors();
        checkEqual(nrOfSensors, 1, "getNrOfOrientationSensors of multipleanalogsensorsclient works correctly");

        yarp::sig::Vector sensorMeasure(3, 0.0), clientMeasure(3, 0.0);
        double timestamp{0.0}, clientTimestamp{0.0};

        MAS_status status = clientOrientSens->getOrientationSensorStatus(0);
        checkEqual(status, MAS_status::MAS_OK, "multipleanalogsensorsclient getOrientationSensorStatus return value is MAS_OK");

        result = orientSens->getOrientationSensorMeasureAsRollPitchYaw(0, sensorMeasure, timestamp);
        bool resultClient = clientOrientSens->getOrientationSensorMeasureAsRollPitchYaw(0, clientMeasure, clientTimestamp);

        checkEqual(result, resultClient, "getOrientationSensorMeasureAsRollPitchYaw return value is consistent between sensor and client");

        for(int i=0; i < 3; i++) {
            checkEqualish(sensorMeasure[i], clientMeasure[i], "getOrientationSensorMeasureAsRollPitchYaw measure is consistent between sensor and client");
        }

        checkTrue(std::abs(timestamp-clientTimestamp) < 0.2, "getOrientationSensorMeasureAsRollPitchYaw measure is consistent between sensor and client (up to 200 ms)");

        // Close devices
        client.close();
        iwrap->detachAll();
        wrapper.close();
        imuSensor.close();
    }

    virtual void runTests() override {
        Network::setLocalMode(true);
        testServerClientOnSingleIMU();
        Network::setLocalMode(false);
    }
};

static MultipleAnalogSensorsInterfacesTest theMultipleAnalogSensorsInterfacesTest;

UnitTest& getMultipleAnalogSensorsInterfacesTest() {
    return theMultipleAnalogSensorsInterfacesTest;
}
