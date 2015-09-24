// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <math.h>

#include <yarp/conf/system.h>
#include <yarp/os/all.h>

#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os::impl;
using namespace yarp::os;

class EventTestHelper : public Thread {
public:
    Semaphore done;
    Event x;
    int state;

    EventTestHelper() : done(0), x() {
        state = 1;
    }

    ~EventTestHelper() {
        stop();
    }

    virtual void run() {
        done.post();
        x.wait();
        state = 2;
        done.post();
    }
};

class EventTestHelper2 : public Thread {
public:
    Event& x;
    Semaphore done;

    EventTestHelper2(Event& x) : x(x), done(0) {
    }

    ~EventTestHelper2() {
        stop();
    }

    virtual void run() {
        x.wait();
        done.post();
    }
};

class EventTest : public UnitTest {
public:
    virtual String getName() { return "EventTest"; }

    void checkBasic() {
        report(0, "basic event sanity check...");
        Event x;
        x.signal();
        x.wait();
    }

    void checkBlock() {
        report(0, "check blocking behavior...");
        EventTestHelper helper;
        helper.start();
        helper.done.wait();
        checkEqual(helper.state,1,"helper blocked");
        helper.x.signal();
        helper.done.wait();
        checkEqual(helper.state,2,"helper unblocked");
    }

    void checkSingleWakeup() {
        report(0, "check single wakeup...");
        Event x;
        EventTestHelper2 h1(x), h2(x);
        h1.start();
        h2.start();
        x.signal();
        Time::delay(0.5);
        int ct = 0;
        if (h1.done.check()) ct++;
        if (h2.done.check()) ct++;
        checkEqual(ct,1,"just one awoke");
        x.signal();
    }

    void checkMultipleWakeup() {
        report(0, "check multiple wakeup...");
        Event x(false);
        EventTestHelper2 h1(x), h2(x);
        h1.start();
        h2.start();
        Time::delay(0.5);

        // removing these checks since they could fail on loaded machine
        checkFalse(h1.done.check(),"first not woken too early");
        checkFalse(h2.done.check(),"second not woken too early");
        x.signal();
        int ct = 0;
        h1.done.wait();
        ct++;
        h2.done.wait();
        ct++;
        checkEqual(ct,2,"both awoke");
    }

    virtual void runTests() {
        checkBasic();
        checkBlock();
        // Removing this test since it is difficult to implement without
        // delays/timeouts and this would cause test to depend on current
        // load (bad on virtual machines)
        // checkSingleWakeup() also seems to be redundant w.r.t checkMultipleWakeup()
        checkMultipleWakeup();
    }
};

static EventTest theEventTest;

UnitTest& getEventTest() {
    return theEventTest;
}
