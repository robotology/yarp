/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ICURRENTCONTROLTEST_H
#define ICURRENTCONTROLTEST_H

#include <yarp/dev/ICurrentControl.h>
#include <yarp/dev/IControlMode.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iCurrentControl_test_1(ICurrentControl* icurr, IControlMode* icmd)
    {
        REQUIRE(icurr != nullptr);
        REQUIRE(icmd != nullptr);

        bool b = false;
        int axis = 0;
        double val = 0;
        double min = 0;
        double max = 0;

        b = icurr->getNumberOfMotors(&axis);
        CHECK (b);

        b = icurr->getCurrent(0, &val);
        CHECK(b);

        b = icurr->getRefCurrent(0, &val);
        CHECK(b);

        b = icurr->setRefCurrent(0, 100);
        CHECK(b);

        b = icurr->getRefCurrent(0, &val);
        CHECK(b);

        b = icurr->getCurrentRange(0, &min, &max);
        CHECK(b);
    }
}

#endif
