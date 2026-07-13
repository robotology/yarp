/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/SystemClock.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/SensorStreamingData.h>

#include <yarp/conf/environment.h>

#include <array>
#include <thread>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

TEST_CASE("pm::sensorMeasurements_to_vectorTest", "[yarp::pm]")
{
    YARP_REQUIRE_PLUGIN("sensorMeasurements_to_vector", "portmonitor")

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);
    yarp::os::Time::delay(1.0);

    struct TestCase {
        std::string carrier;
        bool use_converter;
    };

    auto tc = GENERATE(
        TestCase {"fast_tcp", false},
        TestCase {"fast_tcp+recv.portmonitor+file.sensorMeasurements_to_vector+type.dll", true}
    );

    SECTION("Test SensorStreamingData to Vector conversion")
    {
        yarp::os::BufferedPort<yarp::dev::SensorStreamingData> sender;
        yarp::os::Port receiver;

        sender.open("/send");
        receiver.open("/recv");

        bool b = yarp::os::Network::connect("/send", "/recv", tc.carrier);
        REQUIRE(b);

        // Create test sensor data
        yarp::dev::SensorStreamingData& ssd = sender.prepare();

        // Add position sensor measurement (3 values)
        yarp::dev::SensorMeasurement pos_meas;
        pos_meas.measurement.resize(3);
        pos_meas.measurement[0] = 1.0;
        pos_meas.measurement[1] = 2.0;
        pos_meas.measurement[2] = 3.0;
        ssd.PositionSensors.measurements.resize(1);
        ssd.PositionSensors.measurements[0] = pos_meas;

        // Add orientation sensor measurement (3 values)
        yarp::dev::SensorMeasurement ori_meas;
        ori_meas.measurement.resize(3);
        ori_meas.measurement[0] = 4.0;
        ori_meas.measurement[1] = 5.0;
        ori_meas.measurement[2] = 6.0;
        ssd.OrientationSensors.measurements.resize(1);
        ssd.OrientationSensors.measurements[0] = ori_meas;

        sender.write();

        yarp::os::Time::delay(0.5);

        if (tc.use_converter) {
            // Should receive Vector
            yarp::sig::Vector vec;
            receiver.read(vec);

            REQUIRE(vec.size() == 6);
            CHECK(vec[0] == 1.0);
            CHECK(vec[1] == 2.0);
            CHECK(vec[2] == 3.0);
            CHECK(vec[3] == 4.0);
            CHECK(vec[4] == 5.0);
            CHECK(vec[5] == 6.0);
        } else {
            // Should receive SensorStreamingData
            yarp::dev::SensorStreamingData received_ssd;
            receiver.read(received_ssd);

            REQUIRE(received_ssd.PositionSensors.measurements.size() == 1);
            REQUIRE(received_ssd.OrientationSensors.measurements.size() == 1);
            CHECK(received_ssd.PositionSensors.measurements[0].measurement[0] == 1.0);
            CHECK(received_ssd.OrientationSensors.measurements[0].measurement[2] == 6.0);
        }

        receiver.close();
        sender.close();
    }

    yarp::os::NetworkBase::setLocalMode(false);
}
