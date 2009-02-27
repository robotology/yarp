// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <math.h>

#include <yarp/os/impl/String.h>

#include <yarp/os/all.h>

#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os::impl;
using namespace yarp::os;

class SemaphoreTestHelper : public Thread {
public:
    Semaphore x;
    int state;

    SemaphoreTestHelper() : x(0) {
        state = 1;
    }

    virtual void run() {
        x.wait();
        state = 2;
    }
};

class SemaphoreTest : public UnitTest {
public:
    virtual String getName() { return "SemaphoreTest"; }

    void checkBasic() {
        report(0, "basic semaphore sanity check...");
        Semaphore x(0);
        x.post();
        x.post();
        checkTrue(x.check(),"pop one");
        checkTrue(x.check(),"pop two");
        checkFalse(x.check(),"pop one too many");
    }

    void checkBlock() {
        report(0, "check blocking behavior...");
        SemaphoreTestHelper helper;
        helper.start();
        Time::delay(0.5); 
        checkEqual(helper.state,1,"helper blocked");
        helper.x.post();
        for (int i=0; i<20&&helper.state==1; i++) {
            Time::delay(0.1); 
        }
        checkEqual(helper.state,2,"helper unblocked");
    }

    virtual void runTests() {
        checkBasic();
        checkBlock();
    }
};

static SemaphoreTest theSemaphoreTest;

UnitTest& getSemaphoreTest() {
    return theSemaphoreTest;
}
