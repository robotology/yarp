/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IRGBDSENSORTEST_H
#define IRGBDSENSORTEST_H

#include <yarp/dev/IRGBDSensor.h>
#include <catch.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iRGBDSensor_test_1(IRGBDSensor* irgbd)
    {
        REQUIRE(irgbd != nullptr);

        int w=0;
        int h=0;
        w = irgbd->getDepthWidth();
        h = irgbd->getDepthHeight();
        CHECK(w > 0);
        CHECK(h > 0);
    }
}

#endif
