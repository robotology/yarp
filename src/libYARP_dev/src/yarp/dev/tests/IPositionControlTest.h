/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IPOSITIONCONTROLTEST_H
#define IPOSITIONCONTROLTEST_H

#include <yarp/dev/IPositionControl.h>
#include <catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iPositionControl_test_1(IPositionControl* ipos)
    {
        REQUIRE(ipos!=nullptr);

        bool b;

        int ax;
        b = ipos->getAxes(&ax);
        CHECK(b);
        CHECK(ax != 0);
    }
}

#endif
