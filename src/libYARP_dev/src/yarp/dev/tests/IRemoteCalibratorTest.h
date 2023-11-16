/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IREMOTECALIBRATORTEST_H
#define IREMOTECALIBRATORTEST_H

#include <yarp/dev/IRemoteCalibrator.h>
#include <yarp/dev/IControlMode.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iRemoteCalibrator_test_1(IRemoteCalibrator* icalib)
    {
        REQUIRE(icalib != nullptr);

        bool b =false;
        b = icalib->calibrateSingleJoint(0);
        CHECK(b);

        b = icalib->calibrateWholePart();
        CHECK(b);

        b = icalib->homingSingleJoint(0);
        CHECK(b);

        b = icalib->homingWholePart();
        CHECK(b);

        b = icalib->parkSingleJoint(0);
        CHECK(b);

        b = icalib->parkWholePart();
        CHECK(b);

        b = icalib->quitCalibrate();
        CHECK(b);

        b = icalib->quitPark();
        CHECK(b);
    }
}

#endif
