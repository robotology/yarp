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
        REQUIRE (ax > 0);

        double* test_encs = new double [ax];
        double* test_times = new double[ax];
        b = ienc->getEncoders(test_encs);
        //CHECK(b);   //this is streaming, it will return true if data is arrived in time
        b = ienc->getEncoderSpeeds(test_encs);
        //CHECK(b);   //this is streaming, it will return true if data is arrived in time
        b = ienc->getEncoderAccelerations(test_encs);
        //CHECK(b);   //this is streaming, it will return true if data is arrived in time
        b = ienc->getEncodersTimed(test_encs, test_times);
        //CHECK(b);   //this is streaming, it will return true if data is arrived in time

        for (size_t i=0; i< ax; i++)
        {
            double val=0;
            double time=0;
            b = ienc->getEncoder(i,&val);
            //CHECK(b); //this is streaming, it will return true if data is arrived in time
            b = ienc->getEncoderSpeed(i, &val);
            //CHECK(b); //this is streaming, it will return true if data is arrived in time
            b = ienc->getEncoderAcceleration(i, &val);
            //CHECK(b); //this is streaming, it will return true if data is arrived in time
            b = ienc->getEncoderTimed(i, &val, &time);
            //CHECK(b); //this is streaming, it will return true if data is arrived in time
        }

        double val=0;
        b = ienc->resetEncoder(val);
        CHECK(b);
        b = ienc->resetEncoders();
        CHECK(b);
        b = ienc->setEncoder(0,val);
        CHECK(b);
        b = ienc->setEncoders(test_encs);
        CHECK(b);

        delete[] test_encs;
        delete[] test_times;
    }
}

#endif
