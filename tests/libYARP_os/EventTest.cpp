/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Event.h>

#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>

#include <catch.hpp>
#include <harness.h>

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

    virtual void run() override {
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

    virtual void run() override {
        x.wait();
        done.post();
    }
};



TEST_CASE("os::EventTest", "[yarp::os]")
{

    SECTION("basic event sanity check")
    {
        Event x;
        x.signal();
        x.wait();
        x.reset();
    }

    SECTION("check blocking behavior")
    {
        EventTestHelper helper;
        helper.start();
        helper.done.wait();
        CHECK(helper.state == 1); // helper blocked
        helper.x.signal();
        helper.done.wait();
        CHECK(helper.state == 2); // helper unblocked
    }

    SECTION("check single wakeup")
    {
        Event x(true);
        EventTestHelper2 h1(x), h2(x);
        h1.start();
        h2.start();
        Time::delay(0.2);
        CHECK_FALSE(h1.done.check()); // first not woken too early
        CHECK_FALSE(h2.done.check()); // second not woken too early

        x.signal();
        Time::delay(0.2);

        int ct = 0;
        if (h1.done.check()) { h1.done.post(); ct++; }
        if (h2.done.check()) { h2.done.post(); ct++; }
        CHECK(ct == 1); // just one awoke

        x.signal();
        Time::delay(0.2);

        ct = 0;
        if (h1.done.check()) { h1.done.post(); ct++; }
        if (h2.done.check()) { h1.done.post(); ct++; }
        CHECK(ct == 2); // both awoke
    }

    SECTION("check multiple wakeup")
    {
        Event x(false);
        EventTestHelper2 h1(x), h2(x);
        h1.start();
        h2.start();
        Time::delay(0.2);
        CHECK_FALSE(h1.done.check()); // first not woken too early
        CHECK_FALSE(h2.done.check()); // second not woken too early

        x.signal();
        Time::delay(0.2);

        int ct = 0;
        h1.done.wait();
        ct++;
        h2.done.wait();
        ct++;
        CHECK(ct == 2); // both awoke
    }
}
