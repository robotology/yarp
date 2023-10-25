/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IENCODERSTIMEDTEST_H
#define IENCODERSTIMEDTEST_H

#include <yarp/dev/IEncodersTimed.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iEncodersTimed_test_1(IEncodersTimed* ienc)
    {
        REQUIRE(ienc !=nullptr);

        bool b;

        int ax=0;
        b = ienc->getAxes(&ax);
        CHECK(b);
        REQUIRE (ax>0);

        double* test_encs = new double [ax];
        b = ienc->getEncoders(test_encs);
        CHECK(b);
        delete [] test_encs;
    }
}

#endif
