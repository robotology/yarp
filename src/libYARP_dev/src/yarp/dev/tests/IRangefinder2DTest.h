/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IRANGEFINDER2DTEST_H
#define IRANGEFINDER2DTEST_H

#include <yarp/dev/IRangefinder2D.h>
#include <catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iRangefinder2D_test_1(IRangefinder2D* irf)
    {
        REQUIRE(irf != nullptr);

        bool b;

        yarp::dev::IRangefinder2D::Device_status status;
        for (size_t counter = 0; counter<10; counter++)
        {
            b = irf->getDeviceStatus(status);
            CHECK(b);
            if (status == yarp::dev::IRangefinder2D::Device_status::DEVICE_OK_IN_USE) break;
            yarp::os::Time::delay(0.5);
        }
        CHECK(status == yarp::dev::IRangefinder2D::Device_status::DEVICE_OK_IN_USE);

        std::string info;
        b = irf->getDeviceInfo(info);
        CHECK(b);
        CHECK(info==std::string("Fake Laser device for test/debugging"));

        double scanrate;
        b = irf->getScanRate(scanrate);
        CHECK(b);
        CHECK(scanrate==0.02);

        double hstep;
        b = irf->getHorizontalResolution(hstep);
        CHECK(b);
        CHECK(hstep==1.0);

        double min,max;
        b = irf->getDistanceRange(min,max);
        CHECK(b);
        CHECK(min == 0.1);
        CHECK(max == 8.0);

        std::vector<yarp::dev::LaserMeasurementData> las;
        double timestamp;
        b = irf->getLaserMeasurement(las, &timestamp);
        CHECK(b);
        CHECK(timestamp != 0);
        CHECK(las.size() == 360);

        //check the measurement values.
        //REQUIRE is needed to prevent segfault if nothing is received.
        {
            REQUIRE(las.size() > 0);
            double r,t;
            las[0].get_polar(r, t);
            CHECK(r == 0.5);
            CHECK(t == 0);
        }
        {
            REQUIRE(las.size() > 0);
            double x,y;
            las[0].get_cartesian(x, y);
            CHECK(x == 0.5);
            CHECK(y == 0);
        }

    }
}

#endif
