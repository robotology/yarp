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

    virtual void runTests() {
        checkBasic();
    }
};

static SemaphoreTest theSemaphoreTest;

UnitTest& getSemaphoreTest() {
    return theSemaphoreTest;
}
