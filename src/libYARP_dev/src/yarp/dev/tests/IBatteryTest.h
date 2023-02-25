/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IBATTEYTEST_H
#define IBATTERYTEST_H

#include <yarp/dev/IBattery.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iBattery_test_1(IBattery* ibat)
    {
        REQUIRE(ibat != nullptr);

        bool b;
        double voltage, charge, curr, temp;
        std::string info;
        yarp::dev::IBattery::Battery_status status;
        b = ibat->getBatteryVoltage(voltage);  CHECK(b);
        b = ibat->getBatteryCharge(charge);    CHECK(b);
        b = ibat->getBatteryCurrent(curr);     CHECK(b);
        b = ibat->getBatteryTemperature(temp); CHECK(b);
        b = ibat->getBatteryStatus(status);    CHECK(b);
        b = ibat->getBatteryInfo(info);        CHECK(b);
    }
}

#endif
