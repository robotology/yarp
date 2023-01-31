/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IPOSITIONSENSOTSTEST_H
#define IPOSITIONSENSOTSTEST_H

#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_IPositionSensors_test_1(IPositionSensors* ios)
    {
        REQUIRE(ios != nullptr);

        int nrOfSensors = ios->getNrOfPositionSensors();
        CHECK(nrOfSensors == 1); // getNrOfPositionSensors of multipleanalogsensorsclient works correctly

        yarp::sig::Vector sensorMeasure(3, 0.0), clientMeasure(3, 0.0);
        double timestamp{ 0.0 }, clientTimestamp{ 0.0 };

        MAS_status status = ios->getPositionSensorStatus(0);
        CHECK(status == MAS_status::MAS_OK); // multipleanalogsensorsclient getPositionSensorStatus return value is MAS_OK

        bool result = ios->getPositionSensorMeasure(0, sensorMeasure, timestamp);
        bool resultClient = ios->getPositionSensorMeasure(0, clientMeasure, clientTimestamp);

        CHECK(result == resultClient); // getPositionSensorMeasure return value is consistent between sensor and client

        for (int i = 0; i < 3; i++) {
            CHECK(sensorMeasure[i] == Catch::Approx(clientMeasure[i])); // getPositionSensorMeasure measure is consistent between sensor and client
        }

        CHECK(std::abs(timestamp - clientTimestamp) < 0.2); // getOrientationSensorMeasureAsRollPitchYaw measure is consistent between sensor and client (up to 200 ms)
    }
}

#endif
