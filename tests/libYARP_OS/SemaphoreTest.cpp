/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cmath>

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

    virtual void run() override {
        x.wait();
        state = 2;
        x.post();
    }
};

class SemaphoreTest : public UnitTest {
public:
    virtual std::string getName() const override { return "SemaphoreTest"; }

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
        Time::delay(0.5);
        helper.x.wait();
        checkEqual(helper.state,2,"helper unblocked");
        helper.x.post();
        helper.stop();
    }

    void checkTimed() {
        report(0, "check timed blocking behavior...");
        Semaphore x(0);
        bool result = x.waitWithTimeout(0.5);
        checkFalse(result, "wait timed out ok");
        x.post();
        result = x.waitWithTimeout(1000);
        checkTrue(result, "wait succeeded");
    }

    virtual void runTests() override {
        checkBasic();
        checkBlock();
        checkTimed();
    }
};

static SemaphoreTest theSemaphoreTest;

UnitTest& getSemaphoreTest() {
    return theSemaphoreTest;
}
