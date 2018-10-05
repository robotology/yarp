/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <string>
#include <yarp/os/Network.h>
#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/Timer.h>
#include <yarp/os/Time.h>
#include <string>
#include <yarp/os/LogStream.h>
#include <cmath>

#if defined(USE_SYSTEM_CATCH)
#include <catch.hpp>
#else
#include "catch.hpp"
#endif

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace std;



static bool monoMultiThreadCallback(const YarpTimerEvent& timings)
{
    return true;
}

#ifdef BROKEN_TEST
static bool         timingTestResult{false};
static double       startTime{0};
static double       timingTolerance{0.1};
static double       spinDuration{0.5};
static unsigned int runCount{ 1 };

static bool timingCallback(const YarpTimerEvent& timings)
{
    timingTestResult = true; //the callback has been called
    double now = yarp::os::Time::now();
    vector<pair<std::function<bool()>, string>> checks //checks vector.. add new lines here to add new checks
    {
        {[&timings]       { return timings.currentExpected - startTime + spinDuration * runCount; }, "currentExpected error"},
        {[&timings, &now] { return fabs(now - timings.currentReal) < 0.0001; }, "currentReal error"},
        {[&timings]       { return runCount == 1 || (timings.lastDuration - spinDuration) < timingTolerance; }, "lastDuration error"},
        {[&timings, &now] { return fabs((now - timings.lastExpected) - spinDuration) < timingTolerance; }, "lastExpexted error"},
        {[&timings]       { return runCount == 1 || fabs(timings.lastExpected - timings.lastReal) < timingTolerance; }, "lastReal error"},
        {[&timings]       { return timings.runCount == runCount; }, "runCount error"}
    };

    for (auto& c : checks)
    {
        timingTestResult = c.first();
        if (!timingTestResult)
        {
            yError() << c.second;
            return false;
        }
    }
    runCount++;
    return true;
}
static void timingTest(bool multiThread)
{
    Timer timing(TimerSettings(spinDuration), &timingCallback, multiThread);
    startTime = yarp::os::Time::now();
    timing.start();
    yarp::os::Time::delay(5);
    timing.stop();
    CHECK(timingTestResult);
    INFO(string((multiThread ? "MultiThread " : "MonoThread ")) + string("timing test").c_str());
}
#endif // BROKEN_TEST

static void monoMultiThreadTest(bool multiThread)
{
    int threadCount = yarp::os::Thread::getCount();
    vector<Timer*> timers
    {
        new Timer({0.5},&monoMultiThreadCallback, multiThread),
        new Timer({0.4},&monoMultiThreadCallback, multiThread),
        new Timer({0.3},&monoMultiThreadCallback, multiThread)
    };

    for (auto& t : timers)
    {
        t->start();
    }

    if (multiThread)
        CHECK(yarp::os::Thread::getCount() - threadCount == 3); // multiThread test
    else
        CHECK(yarp::os::Thread::getCount() - threadCount == 1); // singleThread test

    for (auto& t : timers)
    {
        t->stop();
        delete t;
    }
}

static void apiTest(bool multiThread)
{
    int i{ 0 }, j{ 0 };
    Timer t(TimerSettings(0.1, 3, 10), [&i](const YarpTimerEvent& timings) {i++; return true; }, multiThread);
    Timer t2(TimerSettings(0.1, 10, 0.4), [&j](const YarpTimerEvent& timings) {j++; return true; }, multiThread);

    CHECK(t.getSettings() == TimerSettings(0.1, 3, 10)); // API getSettings check
    CHECK_FALSE(t.isRunning()); // API isRunning check
    t.step();
    CHECK(i == 1); // API step check
    t.start();
    t2.start();
    CHECK((t.isRunning() && t2.isRunning())); // API start check
    yarp::os::Time::delay(1);
    CHECK((2 < j && j < 5)); // time expiration
    CHECK(i == 4); // run count expiration
    CHECK_FALSE((t.isRunning() || t2.isRunning())); // timers automatically stopped
}


TEST_CASE("OS::TimerTest", "[yarp::os]") {
    //please note that the return values of the callbacks does not rappresent the result of the tests but simply keeps the timer alive..

#ifdef BROKEN_TEST
    //fails with valgrind for valgrind slowing down the system
    SECTION("timing multithread")
    {
        timingTest(true);
    }

    SECTION("timing monothread")
    {
        runCount = 1;
        timingTest(false);
    }
#endif // BROKEN_TEST

    SECTION("api test multithread")
    {
        apiTest(true);
    }

    SECTION("api test monothread")
    {
        apiTest(false);
    }

    SECTION("mono multithread test multithread")
    {
        monoMultiThreadTest(true);
    }

    SECTION("mono multithread test monothread")
    {
        monoMultiThreadTest(false);
    }

}
