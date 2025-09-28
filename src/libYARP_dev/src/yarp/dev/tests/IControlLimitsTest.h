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
        b = ilims->getPosLimits (0, &min, &max);
        CHECK(b);
        CHECK(min!=100);
        CHECK(max!=200);

        b = ilims->getVelLimits(0, &min, &max);
        CHECK(b);
        CHECK(min!=1000);
        CHECK(max!=2000);

        b = ilims->setPosLimits(0, 100, 200);
        CHECK(b);

        b = ilims->setVelLimits(0, 1000, 2000);
        CHECK(b);

        b = ilims->getPosLimits (0, &min, &max);
        CHECK(b);
        CHECK(min==100);
        CHECK(max==200);

        b = ilims->getVelLimits(0, &min, &max);
        CHECK(b);
        CHECK(min==1000);
        CHECK(max==2000);
    }
}

#endif
