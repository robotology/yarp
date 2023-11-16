/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IMOTORCONTROLTEST_H
#define IMOTORCONTROLTEST_H

#include <yarp/dev/IMotor.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iMotor_test_1(IMotor* imot)
    {
        REQUIRE(imot != nullptr);

        int axis=0;
        bool b = false;
        double val=0;
        double res=0;
        double vals[2] = {0,0};

        b = imot->getNumberOfMotors(&axis);
        CHECK(b);

        b = imot->getGearboxRatio(0, &val);
        CHECK(b);

        b = imot->setGearboxRatio(0,100);
        CHECK(b);

        b = imot->getGearboxRatio(0, &val);
        CHECK(b);

        b = imot->getTemperature(0, &val);
        CHECK(b);

        b = imot->getTemperatureLimit(0, &val);
        CHECK(b);

        b = imot->setTemperatureLimit(0, 80);
        CHECK(b);

        b = imot->getTemperatureLimit(0, &val);
        CHECK(b);

        b = imot->getTemperatures(vals);
        CHECK(b);
    }
}

#endif
