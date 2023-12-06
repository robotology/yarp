/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ICONTROLLIMITSTEST_H
#define ICONTROLLIMITSTEST_H

#include <memory>
#include <numeric>
#include <vector>

#include <yarp/dev/IAxisInfo.h>
#include <yarp/dev/IControlLimits.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iControlLimits_test1(IControlLimits* ilims, IAxisInfo* iinfo)
    {
        REQUIRE(ilims != nullptr);
        REQUIRE(iinfo != nullptr);

        bool b;
        int ax;

        b = iinfo->getAxes(&ax);
        CHECK(b);
        REQUIRE(ax > 0);

        double min=0;
        double max=0;
        b = ilims->getLimits (0, &min, &max);
        CHECK(b);

        b = ilims->getVelLimits(0, &min, &max);
        CHECK(b);

        b = ilims->setLimits(0, min, max);
        CHECK(b);

        b = ilims->setVelLimits(0, min, max);
        CHECK(b);
    }
}

#endif
