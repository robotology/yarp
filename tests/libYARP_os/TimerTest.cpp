/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Timer.h>

#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>

#include <cmath>
#include <string>
#include <vector>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;

static bool monoMultiThreadCallback(const YarpTimerEvent& timings)
{
    return true;
}

#if defined(ENABLE_BROKEN_TESTS)
static bool         timingTestResult{false};
static double       startTime{0};
static double       timingTolerance{0.1};
static double       spinDuration{0.5};
static unsigned int runCount{ 1 };

static bool timingCallback(const YarpTimerEvent& timings)
{
    timingTestResult = true; //the callback has been called
    double now = yarp::os::Time::now();
    std::vector<pair<std::function<bool()>, std::string>> checks //checks vector.. add new lines here to add new checks
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
    INFO(std::string((multiThread ? "MultiThread " : "MonoThread ")) + std::string("timing test").c_str());
}
#endif // ENABLE_BROKEN_TESTS

static void monoMultiThreadTest(bool multiThread)
{
    int threadCount = yarp::os::Thread::getCount();
    std::vector<Timer*> timers
    {
        new Timer({0.5}, &monoMultiThreadCallback, multiThread),
        new Timer({0.4}, &monoMultiThreadCallback, multiThread),
        new Timer({0.3}, &monoMultiThreadCallback, multiThread)
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


TEST_CASE("os::TimerTest", "[yarp::os]")
{
#if defined(DISABLE_FAILING_TESTS)
    YARP_SKIP_TEST("Skipping failing tests")
#endif

    //please note that the return values of the callbacks does not rappresent
    // the result of the tests but simply keeps the timer alive..

#if defined(ENABLE_BROKEN_TESTS)
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
#endif // ENABLE_BROKEN_TESTS

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
