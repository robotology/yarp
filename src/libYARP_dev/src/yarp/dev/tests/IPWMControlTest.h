/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IPWMCONTROLTEST_H
#define IPWMCONTROLTEST_H

#include <yarp/dev/IPWMControl.h>
#include <yarp/dev/IControlMode.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iPwmControl_test_1(IPWMControl* ipwm, IControlMode* icmd)
    {
        REQUIRE(ipwm != nullptr);
        REQUIRE(icmd != nullptr);

        bool b = false;
        int axis = 0;
        double val  = 0;

        b = ipwm->getNumberOfMotors(&axis);
        CHECK(b);

        b = ipwm->getDutyCycle(0, &val);
        CHECK(b);

        b = ipwm->getRefDutyCycle(0, &val);
        CHECK(b);

        b = ipwm->setRefDutyCycle(0,100);
        CHECK(b);
    }
}

#endif
