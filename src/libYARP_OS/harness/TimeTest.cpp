// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/Time.h>
#include <yarp/NetType.h>
#include <yarp/String.h>

#include <yarp/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os;

class TimeTest : public yarp::UnitTest {
public:
    virtual yarp::String getName() { return "TimeTest"; }

    void testDelay() {
        report(0,"testing delay (there will be a short pause)...");
        double target = 0.75;
        double t1 = Time::now();
        Time::delay(target);
        double t2 = Time::now();
        double dt = t2-t1-target;
        double limit = 0.3; // don't be too picky, there is a lot of undefined slop
        bool inLimits = (-limit<dt)&&(dt<limit);
        report(0,yarp::String("delay was late(+) or early(-) by ") +
               yarp::NetType::toString((int)(dt*1000)) +
               " ms");
        checkEqual(true,inLimits,"delay for 0.75 seconds");
    }

    virtual void runTests() {
        testDelay();
    }
};

static TimeTest theTimeTest;

yarp::UnitTest& getTimeTest() {
    return theTimeTest;
}

