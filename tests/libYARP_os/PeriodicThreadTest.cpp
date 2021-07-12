/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
#include <cmath> // std::floor, std::ceil

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

    PeriodicThread1(double r, PeriodicThreadClock acc): PeriodicThread(r, acc){}

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

    SECTION("checking init/release synchronization")
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

    SECTION("testing periodic thread accuracy (relative)")
    {
        double delay = 1.0;
        PeriodicThreadClock clockAccuracy = PeriodicThreadClock::Relative;

        double desiredPeriod1 = 0.100;
        PeriodicThread1 thread1(desiredPeriod1, clockAccuracy);
        thread1.start();
        Time::delay(delay);
        thread1.stop();
        CHECK(thread1.period > desiredPeriod1);
        CHECK(thread1.getIterations() <= std::ceil(delay / desiredPeriod1));

        double desiredPeriod2 = 0.010;
        PeriodicThread1 thread2(desiredPeriod2, clockAccuracy);
        thread2.start();
        Time::delay(delay);
        thread2.stop();
        CHECK(thread2.period > desiredPeriod2);
        CHECK(thread2.getIterations() <= std::ceil(delay / desiredPeriod2));

        double desiredPeriod3 = 0.001;
        PeriodicThread1 thread3(desiredPeriod3, clockAccuracy);
        thread3.start();
        Time::delay(delay);
        thread3.stop();
        CHECK(thread3.period > desiredPeriod3);
        CHECK(thread3.getIterations() <= std::ceil(delay / desiredPeriod3));

        thread3.resetStat(); // get the iteration counter back to zero

        thread3.start();
        Time::delay(delay / 2.0);
        thread3.setPeriod(desiredPeriod2);
        Time::delay(delay / 2.0);
        thread3.stop();
        CHECK(thread3.getPeriod() == desiredPeriod2);
        unsigned int countEstimation = 0.5 * (delay / desiredPeriod3 + delay / desiredPeriod2);
        CHECK(thread3.getIterations() <= std::ceil(countEstimation));
    }

    SECTION("testing periodic thread accuracy (absolute)")
    {
        double periodThreshold = 0.001;
        int countThreshold = 2;
        double delay = 1.0;
        PeriodicThreadClock clockAccuracy = PeriodicThreadClock::Absolute;

        double desiredPeriod1 = 0.100;
        PeriodicThread1 thread1(desiredPeriod1, clockAccuracy);
        thread1.start();
        Time::delay(delay);
        thread1.stop();
        CHECK(thread1.period > desiredPeriod1 * (1.0 - periodThreshold));
        CHECK(thread1.period < desiredPeriod1 * (1.0 + periodThreshold));
        CHECK(thread1.getIterations() >= std::floor(delay / desiredPeriod1) - countThreshold);
        CHECK(thread1.getIterations() <= std::ceil(delay / desiredPeriod1) + countThreshold);

        double desiredPeriod2 = 0.010;
        PeriodicThread1 thread2(desiredPeriod2, clockAccuracy);
        thread2.start();
        Time::delay(delay);
        thread2.stop();
        CHECK(thread2.period > desiredPeriod2 * (1.0 - periodThreshold));
        CHECK(thread2.period < desiredPeriod2 * (1.0 + periodThreshold));
        CHECK(thread2.getIterations() >= std::floor(delay / desiredPeriod2) - countThreshold);
        CHECK(thread2.getIterations() <= std::ceil(delay / desiredPeriod2) + countThreshold);

        double desiredPeriod3 = 0.001;
        PeriodicThread1 thread3(desiredPeriod3, clockAccuracy);
        thread3.start();
        Time::delay(delay);
        thread3.stop();
        CHECK(thread3.period > desiredPeriod3 * (1.0 - periodThreshold));
        CHECK(thread3.period < desiredPeriod3 * (1.0 + periodThreshold));
        CHECK(thread3.getIterations() >= std::floor(delay / desiredPeriod3) - countThreshold);
        CHECK(thread3.getIterations() <= std::ceil(delay / desiredPeriod3) + countThreshold);

        thread3.resetStat(); // get the iteration counter back to zero

        thread3.start();
        Time::delay(delay / 2.0);
        thread3.setPeriod(desiredPeriod2);
        Time::delay(delay / 2.0);
        thread3.stop();
        CHECK(thread3.getPeriod() == desiredPeriod2);
        double countEstimation = 0.5 * (delay / desiredPeriod3 + delay / desiredPeriod2);
        CHECK(thread3.getIterations() >= std::floor(countEstimation) - countThreshold);
        CHECK(thread3.getIterations() <= std::ceil(countEstimation) + countThreshold);
    }

    SECTION("testing askToStop() function")
    {
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

    SECTION("testing simulated time")
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
