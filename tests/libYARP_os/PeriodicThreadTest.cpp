/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/PeriodicThread.h>
#ifndef YARP_NO_DEPRECATED
#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#include <yarp/os/RateThread.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#endif // YARP_NO_DEPRECATED

#include <yarp/os/impl/NameServer.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/Clock.h>
#include <yarp/os/SystemClock.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;

namespace harness_os {
namespace periodicThread {

class MyClock : public Clock {
public:
    double t;
    bool done;

    MyClock() {
        t = 0;
        done = false;
    }

    double now() override {
        return t;
    }

    void delay(double seconds) override {
        double target = t+seconds;
        SystemClock c;
        while (t<target && !done) {
            c.delay(0.1);
        }
    }

    bool isValid() const override {
        return true;
    }
};


class PeriodicThread1: public PeriodicThread
{
public:
    double t1;
    double t2;
    double average;
    double period;
    int n;

    PeriodicThread1(double r): PeriodicThread(r){}

    bool threadInit() override
    {
        n=0;
        t1=0;
        t2=0;
        average=0;

        return true;
    }

    void run() override
    {
        t2=Time::now();

        if (n>0)
        {
            average+=(t2-t1);
        }
        n++;
        t1=t2;
    }

    void threadRelease() override
    {
        if (n>0)
            period=average/(n-1);
        else
            period=0;
    }

};

class PeriodicThread2: public PeriodicThread
{
public:
    bool fail;
    int state;

    PeriodicThread2(double r) :
            PeriodicThread(r),
            fail(false),
            state(-1)
    {
    }

    void threadWillFail(bool f)
    {
        fail = f;
    }

    bool threadInit() override
    {
        state = -1;
        return !fail;
    }

    void afterStart(bool s) override
    {
        if (s) {
            state=0;
        }
    }

    void run() override
    {
    }

    void threadRelease() override
    {
        state++;
    }
};

class PeriodicThread3: public PeriodicThread
{
public:
    bool fail;
    int state;

    PeriodicThread3(double r): PeriodicThread(r), fail(false), state(-1){}

    void threadWillFail(bool f)
    {
        fail=f;
        //reset internal state
        state=-1;
    }

    bool threadInit() override
    {
        Time::delay(0.5);
        state++;
        return !fail;
    }

    void afterStart(bool s) override
    {
        if (s)
            state++;
        else
            state=-2;
    }

    void run() override
    {}

    void threadRelease() override
    {
        Time::delay(0.5);
        state++;
    }
};

class PeriodicThread4: public PeriodicThread
{
public:
    int count;

    PeriodicThread4(double r): PeriodicThread(r), count(10){}

    void run() override
    {
            count--;

            //terminate when count is zero
            if (count==0)
                PeriodicThread::askToStop();
    }

};

class PeriodicThread5: public PeriodicThread
{
public:
    int count;

    PeriodicThread5(double r): PeriodicThread(r), count(0){}

    void run() override {
        count++;
    }
};

/*
    * This thread is intended to have the run() function which
    * takes more time to execute then the thread period.
    * The delay will be a negative number.
    * Check that thread does not hangs forever.
    */
class BusyThread: public PeriodicThread
{
public:
    int count;

    BusyThread(double r): PeriodicThread(r), count(0){}

    void run() override {
        printf("BusyThread running ...\n");
        SystemClock::delaySystem(1);
    }
};

class AskForStopThread : public PeriodicThread {
public:
    bool done;

    AskForStopThread() : PeriodicThread(0.1) {
        done = false;
    }

    void run() override {
        if (done) askToStop();
    }

    void threadRelease() override {
        done =false;
    }
};

double test(double period, double delay)
{
    double estPeriod=0;
    PeriodicThread1 *thread1=new PeriodicThread1(period);

    thread1->start();
    Time::delay(delay);
    thread1->stop();

    estPeriod=thread1->period;

    delete thread1;
    return estPeriod;
}

} // namespace harness_os
} // namespace periodicThread

using namespace harness_os::periodicThread;

TEST_CASE("os::PeriodicThreadTest", "[yarp::os]")
{
#if defined(DISABLE_FAILING_TESTS)
    YARP_SKIP_TEST("Skipping failing tests")
#endif

    SECTION("checking init failure/success notification")
    {
        PeriodicThread2 t(0.200);
        t.threadWillFail(false);
        t.start();
        CHECK(t.isRunning()); // thread is running
        t.stop();
        CHECK(!t.isRunning()); // thread was stopped
        CHECK(1); // t.state

        t.threadWillFail(true);
        t.start();
        CHECK(!t.isRunning()); // thread stopped
        CHECK(-1); // t.state
    }

    SECTION("Checking init/release synchronization")
    {
        PeriodicThread3 t(0.200);
        t.threadWillFail(false);
        // if start does not wait for threadRelease/threadInit, a race condition
        // will be detected
        t.start();
        CHECK(1); // t.state
        // if start does not wait for threadRelease/threadInit, a race condition
        // will be detected
        t.stop();
        CHECK(2); // t.state

        t.threadWillFail(true);
        t.start();
        CHECK(-2); // t.state
    }

    SECTION("testing rate thread precision")
    {
        bool success = false;
        double acceptedThreshold = 0.10;

        char message[255];

        //try plausible rates
        double desiredPeriod, actualPeriod;
        desiredPeriod = 0.015;
        sprintf(message, "Thread1 requested period: %f[s]", desiredPeriod);
        INFO(message);
        actualPeriod = test(desiredPeriod, 1);
        if( (actualPeriod > (desiredPeriod*(1-acceptedThreshold))) && (actualPeriod < (desiredPeriod * (1+acceptedThreshold))) )
            success = true;
        sprintf(message, "Thread1 estimated period: %f[s]", actualPeriod);
        INFO(message);
        sprintf(message, "Period NOT within range of %d%%", (int)(acceptedThreshold*100));
        if(!success)
            WARN(message);

        desiredPeriod = 0.010;
        sprintf(message, "Thread2 requested period: %f[s]", desiredPeriod);
        INFO(message);
        actualPeriod = test(desiredPeriod, 1);
        if( (actualPeriod > (desiredPeriod*(1-acceptedThreshold))) && (actualPeriod < (desiredPeriod * (1+acceptedThreshold))) )
            success = true;
        sprintf(message, "Thread2 estimated period: %f[s]", actualPeriod);
        INFO(message);
        sprintf(message, "Period NOT within range of %d%%", (int)(acceptedThreshold*100));
        if(!success)
            WARN(message);

        desiredPeriod = 0.001;
        sprintf(message, "Thread3 requested period: %f[s]", desiredPeriod);
        INFO(message);
        actualPeriod = test(desiredPeriod, 1);
        if( (actualPeriod > (desiredPeriod*(1-acceptedThreshold))) && (actualPeriod < (desiredPeriod * (1+acceptedThreshold))) )
            success = true;
        sprintf(message, "Thread3 estimated period: %f[s]", actualPeriod);
        INFO(message);
        sprintf(message, "Period NOT within range of %d%%", (int)(acceptedThreshold*100));
        if(!success)
            WARN(message);

        INFO("Testing askToStop() function");
        PeriodicThread4 thread(0.010);
        thread.start();

        bool running=true;
        int count=10;
        while((running)&&(count>0))
        {
            running=thread.isRunning();
            Time::delay(0.1);
            count--;
        }

        CHECK(!thread.isRunning()); // thread terminated correctly
        //join thread
        thread.stop();

        SystemClock::delaySystem(-2);
        Time::delay(-2);
        CHECK(true); // Negative Time::delay() and delaySystem() is safe.

        BusyThread  busy(0.010);
        busy.start();
        SystemClock::delaySystem(2);
        busy.stop();
        CHECK(true); // Negative delay on reteThread is safe.
    }

    SECTION("Testing simulated time")
    {
        MyClock clock;
        Time::useCustomClock(&clock);
        CHECK(Time::isCustomClock()); // isCustomClock is true
        CHECK(Time::getClockType() == YARP_CLOCK_CUSTOM); // getClockType is YARP_CLOCK_CUSTOM
        PeriodicThread5 thread(100); // 100 secs
        thread.start();
        SystemClock clk;
        for (int i=0; i<20; i++) {
            clk.delay(0.1);
            if (thread.count == 1) break;
        }
        CHECK(thread.count); // 1
        clock.t += 100*1000;
        for (int i=0; i<20; i++) {
            clk.delay(0.1);
            if (thread.count == 2) break;
        }
        CHECK(thread.count); // 2
        clock.done = true;
        thread.stop();
        Time::useSystemClock();
        CHECK(Time::isSystemClock()); // test is using system clock
        CHECK(Time::getClockType() == YARP_CLOCK_SYSTEM); // getClockType is YARP_CLOCK_SYSTEM
    }

    SECTION("testing start() askForStop() start() sequence...")
    {
        AskForStopThread test;
        int ct = 0;
        while (ct<10) {
            if (!test.isRunning()) {
                test.start();
                CHECK(test.isRunning()); // isRunning is correct
                test.done = true;
                ct++;
            }
        }
    }
};
