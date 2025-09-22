/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ICURRENTCONTROLTEST_H
#define ICURRENTCONTROLTEST_H

#include <yarp/os/Time.h>

#include <yarp/dev/ICurrentControl.h>
#include <yarp/dev/IControlMode.h>
#include <catch2/catch_amalgamated.hpp>

#include <memory>
#include <numeric>
#include <vector>

#include "Utils.h"

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
        CHECK(val != 100);

        b = icurr->setRefCurrent(0, 100);
        CHECK(b);
        //Streaming message sent on separated thread, delay required before get()
        yarp::os::Time::delay(0.020);

        b = icurr->getRefCurrent(0, &val);
        CHECK(b);
        CHECK(val == 100);

        b = icurr->getCurrentRange(0, &min, &max);
        CHECK(b);

        auto mins = std::vector< double >(axis);
        auto maxs = std::vector< double >(axis);
        auto currs = std::vector< double >(axis);

        auto refs = std::vector< double >(axis);
        auto refs_check = std::vector< double >(axis);

        b = icurr->getCurrentRanges(mins.data(), maxs.data());
        CHECK(b);

        b = icurr->getCurrents(currs.data());
        CHECK(b);

        set_vector_crescent(axis, refs.data(), 10.0);
        set_vector_crescent(axis, refs_check.data(), 10.0);
        b = icurr->setRefCurrents(refs.data());
        CHECK(b);
        //Streaming message sent on separated thread, delay required before get()
        yarp::os::Time::delay(0.050);

        b = icurr->getRefCurrents(refs.data());
        CHECK(b);
        CHECK(vectors_equal(axis, refs.data(), refs_check.data()));
    }
}

#endif
