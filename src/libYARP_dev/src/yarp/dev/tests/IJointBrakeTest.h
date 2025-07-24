/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IJOINTBRAKETEST_H
#define IJOINTBRAKETEST_H

#include <yarp/dev/IJointBrake.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iJointBrake_test1 (IJointBrake* ibrake)
    {
        REQUIRE(ibrake != nullptr);

        bool b =false;
        bool braked = false;
        std::string message;
        b = ibrake->isJointBraked(0,braked);
        CHECK(b);
        CHECK(braked == false);

        b = ibrake->setManualBrakeActive(0, true);
        CHECK(b);
        b = ibrake->isJointBraked(0, braked);
        CHECK(b);
        CHECK(braked == true);
        b = ibrake->setManualBrakeActive(0, false);
        CHECK(b);
        b = ibrake->isJointBraked(0, braked);
        CHECK(b);
        CHECK(braked == false);

        bool enab = false;
        b = ibrake->getAutoBrakeEnabled(0, enab);
        CHECK(b);
        CHECK(enab == false);

        b = ibrake->setAutoBrakeEnabled(0, true);
        CHECK(b);
        b = ibrake->getAutoBrakeEnabled(0, enab);
        CHECK(b);
        CHECK(enab == true);
        b = ibrake->setAutoBrakeEnabled(0, false);
        CHECK(b);
        b = ibrake->getAutoBrakeEnabled(0, enab);
        CHECK(b);
        CHECK(enab == false);
    }
}

#endif
