/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IRANGEFINDER2DTEST_H
#define IRANGEFINDER2DTEST_H

#include <yarp/dev/IRangefinder2D.h>
#include <catch2/catch_amalgamated.hpp>
#include <yarp/dev/tests/TestUtils.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

namespace yarp::dev::tests
{
    struct ValuestoCheck
    {
        double test_scanrate = 0.02;
        double test_horizontal_res = 1.0;
        double test_min=0.1;
        double test_max=8.0;
        size_t test_lsize=360;
        double test_cartesian_x=0.5;
        double test_cartesian_y=0;
        double test_theta=0;
        double test_rho=0.5;
    };

    inline void exec_iRangefinder2D_test_1(IRangefinder2D* irf, const ValuestoCheck& v=ValuestoCheck())
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
        CHECK(!info.empty());

        double scanrate;
        b = irf->getScanRate(scanrate);
        CHECK(b);
        CHECK(scanrate==v.test_scanrate);

        double hstep;
        b = irf->getHorizontalResolution(hstep);
        CHECK(b);
        CHECK(hstep == v.test_horizontal_res);

        double min,max;
        b = irf->getDistanceRange(min,max);
        CHECK(b);
        CHECK(min == v.test_min);
        CHECK(max == v.test_max);

        std::vector<yarp::sig::LaserMeasurementData> las;
        double timestamp;
        b = irf->getLaserMeasurement(las, &timestamp);
        CHECK(b);
        CHECK(timestamp != 0);
        CHECK(las.size() == v.test_lsize);

        //check the measurement values.
        //REQUIRE is needed to prevent segfault if nothing is received.
        {
            REQUIRE(las.size() > 0);
            double r,t;
            las[0].get_polar(r, t);
            CHECK(yarp::dev::tests::check_approximate(r, v.test_rho));
            CHECK(yarp::dev::tests::check_approximate(t, v.test_theta));
        }
        {
            REQUIRE(las.size() > 0);
            double x,y;
            las[0].get_cartesian(x, y);
            CHECK(yarp::dev::tests::check_approximate(x, v.test_cartesian_x));
            CHECK(yarp::dev::tests::check_approximate(y, v.test_cartesian_y));
        }

    }
}

#endif
