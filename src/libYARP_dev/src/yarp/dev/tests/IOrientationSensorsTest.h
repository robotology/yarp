/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IORIENTATIONSENSOTSTEST_H
#define IORIENTATIONSENSOTSTEST_H

#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_IOrientationSensors_test_1(IOrientationSensors* ios)
    {
        REQUIRE(ios != nullptr);

        int nrOfSensors = ios->getNrOfOrientationSensors();
        CHECK(nrOfSensors > 0); // getNrOfOrientationSensors of multipleanalogsensorsclient works correctly

        yarp::sig::Vector sensorMeasure(3, 0.0), clientMeasure(3, 0.0);
        double timestamp{ 0.0 }, clientTimestamp{ 0.0 };

        MAS_status status = ios->getOrientationSensorStatus(0);
        CHECK(status == MAS_status::MAS_OK); // multipleanalogsensorsclient getOrientationSensorStatus return value is MAS_OK

        bool result = ios->getOrientationSensorMeasureAsRollPitchYaw(0, sensorMeasure, timestamp);
        bool resultClient = ios->getOrientationSensorMeasureAsRollPitchYaw(0, clientMeasure, clientTimestamp);

        CHECK(result == resultClient); // getOrientationSensorMeasureAsRollPitchYaw return value is consistent between sensor and client

        for (int i = 0; i < 3; i++) {
            CHECK(sensorMeasure[i] == Catch::Approx(clientMeasure[i])); // getOrientationSensorMeasureAsRollPitchYaw measure is consistent between sensor and client
        }

        CHECK(std::abs(timestamp - clientTimestamp) < 0.2); // getOrientationSensorMeasureAsRollPitchYaw measure is consistent between sensor and client (up to 200 ms)
    }
}

#endif
