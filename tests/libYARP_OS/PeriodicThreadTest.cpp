/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/Clock.h>
#include <yarp/os/SystemClock.h>


#include <catch.hpp>


using namespace yarp::os;
using namespace yarp::os::impl;

class MyClock : public Clock {
public:
    double t;
    bool done;

    MyClock() {
        t = 0;
        done = false;
    }

    virtual double now() override {
        return t;
    }

    virtual void delay(double seconds) override {
        double target = t+seconds;
        SystemClock c;
        while (t<target && !done) {
            c.delay(0.1);
        }
    }

    virtual bool isValid() const override {
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

    virtual bool threadInit() override
    {
        n=0;
        t1=0;
        t2=0;
        average=0;

        return true;
    }

    virtual void run() override
    {
        t2=Time::now();

        if (n>0)
        {
            average+=(t2-t1);
        }
        n++;
        t1=t2;
    }

    virtual void threadRelease() override
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

    virtual bool threadInit() override
    {
        state = -1;
        return !fail;
    }

    virtual void afterStart(bool s) override
    {
        if (s) {
            state=0;
        }
    }

    virtual void run() override
    {
    }

    virtual void threadRelease() override
    {
        state++;
    }
};

class PeriodicThread3: public PeriodicThread
{
public:
    bool fail;
    int state;

    PeriodicThread3(double r): PeriodicThread(r),fail(false),state(-1){}

    void threadWillFail(bool f)
    {
        fail=f;
        //reset internal state
        state=-1;
    }

    virtual bool threadInit() override
    {
        Time::delay(0.5);
        state++;
        return !fail;
    }

    virtual void afterStart(bool s) override
    {
        if (s)
            state++;
        else
            state=-2;
    }

    virtual void run() override
    {}

    virtual void threadRelease() override
    {
        Time::delay(0.5);
        state++;
    }
};

class PeriodicThread4: public PeriodicThread
{
public:
    int count;

    PeriodicThread4(double r): PeriodicThread(r),count(10){}

    virtual void run() override
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

    PeriodicThread5(double r): PeriodicThread(r),count(0){}

    virtual void run() override {
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

    BusyThread(double r): PeriodicThread(r),count(0){}

    virtual void run() override {
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

class Runnable1:public Runnable
{
public:
    bool initCalled;
    bool releaseCalled;
    bool runExecuted;
    bool initNotified;

    Runnable1(): initCalled(false),
                    releaseCalled(false),
                    runExecuted(false),
                    initNotified(false){}

    virtual bool threadInit() override
    {
        initCalled=true;
        return true;
    }

    virtual void threadRelease() override
    {
        releaseCalled=true;
    }

    virtual void run() override
    {}

    virtual void afterStart(bool s) override
    {
        initNotified=true;
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

TEST_CASE("OS::PeriodicThreadTest", "[yarp::os]") {
        

    SECTION("checking init failure/success notification") {
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

    SECTION("Checking init/release synchronization") {
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

    SECTION("testing rate thread precision") {
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
        if(!success)  YARP_WARN(Logger::get(), message);

        desiredPeriod = 0.010;
        sprintf(message, "Thread2 requested period: %f[s]", desiredPeriod);
        INFO(message);
        actualPeriod = test(desiredPeriod, 1);
        if( (actualPeriod > (desiredPeriod*(1-acceptedThreshold))) && (actualPeriod < (desiredPeriod * (1+acceptedThreshold))) )
            success = true;
        sprintf(message, "Thread2 estimated period: %f[s]", actualPeriod);
        INFO(message);
        sprintf(message, "Period NOT within range of %d%%", (int)(acceptedThreshold*100));
        if(!success)  YARP_WARN(Logger::get(), message);

        desiredPeriod = 0.001;
        sprintf(message, "Thread3 requested period: %f[s]", desiredPeriod);
        INFO(message);
        actualPeriod = test(desiredPeriod, 1);
        if( (actualPeriod > (desiredPeriod*(1-acceptedThreshold))) && (actualPeriod < (desiredPeriod * (1+acceptedThreshold))) )
            success = true;
        sprintf(message, "Thread3 estimated period: %f[s]", actualPeriod);
        INFO(message);
        sprintf(message, "Period NOT within range of %d%%", (int)(acceptedThreshold*100));
        if(!success)  YARP_WARN(Logger::get(), message);

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

    SECTION("Testing runnable") {
        Runnable1 foo;
        RateThreadWrapper t;
        t.setPeriod(0.1);
        t.attach(foo);
        t.start();
        CHECK(t.isRunning()); // thread is running
        t.close();
        CHECK(!t.isRunning()); // thread was stopped
        CHECK(foo.initCalled); // init was called
        CHECK(foo.initNotified); // afterStart() was called
        CHECK(foo.releaseCalled); // release was called
    }


    SECTION("Testing simulated time") {
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

    SECTION("testing start() askForStop() start() sequence...") {
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
