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

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace std;

class TimerTest : public UnitTest {

    bool         timingTestResult{false};
    double       startTime{0};
    double       timingTolerance{0.1};
    double       spinDuration{0.5};
    unsigned int runCount{ 1 };

public:
    virtual std::string getName() override { return "TimerTest"; }

    //please note that the return values of the callbacks does not rappresent the result of the tests but simply keeps the timer alive..
    bool timingCallback(const YarpTimerEvent& timings)
    {
        timingTestResult = true; //the callback has been called
        double now = yarp::os::Time::now();
        vector<pair<std::function<bool()>, string>> checks //checks vector.. add new lines here to add new checks
        {
            {[&timings, this]       { return timings.currentExpected - startTime + spinDuration * runCount; }, "currentExpected error"},
            {[&timings, this, &now] { return fabs(now - timings.currentReal) < 0.0001; }, "currentReal error"},
            {[&timings, this]       { return runCount == 1 || (timings.lastDuration - spinDuration) < timingTolerance; }, "lastDuration error"},
            {[&timings, this, &now] { return fabs((now - timings.lastExpected) - spinDuration) < timingTolerance; }, "lastExpexted error"},
            {[&timings, this]       { return runCount == 1 || fabs(timings.lastExpected - timings.lastReal) < timingTolerance; }, "lastReal error"},
            {[&timings, this]       { return timings.runCount == runCount; }, "runCount error"}
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

    bool monoMultiThreadCallback(const YarpTimerEvent& timings)
    {
        return true;
    }

    void timingTest(bool multiThread)
    {
        Timer timing(TimerSettings(spinDuration), &TimerTest::timingCallback, this, multiThread);
        startTime = yarp::os::Time::now();
        timing.start();
        yarp::os::Time::delay(5);
        timing.stop();
        checkTrue(timingTestResult, (string((multiThread ? "MultiThread " : "MonoThread ")) + string("timing test")).c_str());
    }

    void monoMultiThreadTest(bool multiThread)
    {
        int threadCount = yarp::os::Thread::getCount();
        vector<Timer*> timers
        {
            new Timer({0.5},&TimerTest::monoMultiThreadCallback, this, multiThread),
            new Timer({0.4},&TimerTest::monoMultiThreadCallback, this, multiThread),
            new Timer({0.3},&TimerTest::monoMultiThreadCallback, this, multiThread)
        };

        for (auto& t : timers)
        {
            t->start();
        }

        if (multiThread)
            checkTrue(yarp::os::Thread::getCount() - threadCount == 3, "multiThread test");
        else
            checkTrue(yarp::os::Thread::getCount() - threadCount == 1, "singleThread test");

        for (auto& t : timers)
        {
            t->stop();
            delete t;
        }
    }

    void apiTest(bool multiThread)
    {
        int i{ 0 }, j{ 0 };
        Timer t(TimerSettings(0.1, 3, 10), [&i](const YarpTimerEvent& timings) {i++; return true; }, multiThread);
        Timer t2(TimerSettings(0.1, 10, 0.4), [&j](const YarpTimerEvent& timings) {j++; return true; }, multiThread);

        checkTrue(t.getSettings() == TimerSettings(0.1, 3, 10), "API getSettings check");
        checkFalse(t.isRunning(), "API isRunning check");
        t.step();
        checkTrue(i == 1, "API step check");
        t.start();
        t2.start();
        checkTrue(t.isRunning() && t2.isRunning(), "API start check");
        yarp::os::Time::delay(1);
        checkTrue(2 < j && j < 5, "time expiration");
        checkTrue(i == 4, "run count expiration");
        checkFalse(t.isRunning() || t2.isRunning(), "timers automatically stopped");
    }

    virtual void runTests() override
    {
        //fails with valgrind for valgrind slowing down the system
        //timingTest(true);
        //runCount = 1;
        //timingTest(false);

        apiTest(true);
        apiTest(false);
        monoMultiThreadTest(true);
        monoMultiThreadTest(false);
    }
};


static TimerTest theTimerTest;

UnitTest& getTimerTest() {
    return theTimerTest;
}
