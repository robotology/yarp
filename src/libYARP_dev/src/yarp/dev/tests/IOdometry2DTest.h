/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IODOMETRY2DTEST_H
#define IODOMETRY2DTEST_H

#include <yarp/dev/IOdometry2D.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iOdometry2D_test_1(yarp::dev::Nav2D::IOdometry2D* iodom)
    {
        REQUIRE(iodom != nullptr);

        bool b;

        yarp::dev::OdometryData odomdata;
        b = iodom->getOdometry(odomdata);     CHECK(b);
        b = iodom->resetOdometry();           CHECK(b);
    }
}

#endif
