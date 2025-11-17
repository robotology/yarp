/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IAXISINFOTEST_H
#define IAXISINFOTEST_H

#include <yarp/dev/IAxisInfo.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iAxisInfo_test_1(IAxisInfo* iinfo, std::string jname="joint0")
    {
        REQUIRE(iinfo !=nullptr);

        bool b;

        int ax;
        b = iinfo->getAxes(&ax);
        CHECK(b);
        CHECK(ax != 0);

        std::string name;
        b = iinfo->getAxisName(0,name);
        CHECK(b);
        CHECK(name == jname);

        yarp::dev::JointTypeEnum type;
        b = iinfo->getJointType(0, type);
        CHECK(b);
        CHECK(type == yarp::dev::JointTypeEnum::VOCAB_JOINTTYPE_REVOLUTE);
    }
}

#endif
