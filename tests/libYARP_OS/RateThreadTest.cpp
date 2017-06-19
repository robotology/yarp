/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

// added test for threadInit/threadRelease functions, synchronization and
// thread init success/failure notification -nat

#include <yarp/os/RateThread.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/Clock.h>
#include <yarp/os/SystemClock.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;

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

class RateThreadTest : public UnitTest {
private:
    class RateThread1: public RateThread
    {
    public:
        double t1;
        double t2;
        double average;
        double period;
        int n;

        RateThread1(int r): RateThread(r){}

        virtual bool threadInit() override
        {
            printf("-->Starting rate thread: %.2lf[ms]...", getRate());
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

            // printf(".");
        }

        virtual void threadRelease() override
        {
            if (n>0)
                period=1000*average/(n-1);
            else
                period=0;

            printf("thread quit\n");
        }

    };

    class RateThread2: public RateThread
    {
    public:
        bool fail;
        int state;

        RateThread2(int r): RateThread(r),fail(false),state(-1){}

        void threadWillFail(bool f)
        {fail=f;}

        virtual bool threadInit() override
        {
            state=-1;
            return !fail;
        }

        virtual void afterStart(bool s) override
        {
            if (s)
                state=0;
        }

        virtual void run() override
        {}

        virtual void threadRelease() override
        {
            state++;
        }
    };

    class RateThread3: public RateThread
    {
    public:
        bool fail;
        int state;

        RateThread3(int r): RateThread(r),fail(false),state(-1){}

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

    class RateThread4: public RateThread
    {
    public:
        int count;

        RateThread4(int r): RateThread(r),count(10){}

        virtual void run() override
        {
                count--;

               //terminate when count is zero
               if (count==0)
                   RateThread::askToStop();
        }

    };

    class RateThread5: public RateThread
    {
    public:
        int count;

        RateThread5(int r): RateThread(r),count(0){}

        virtual void run() override {
            count++;
        }
    };

    class AskForStopThread : public RateThread {
    public:
        bool done;

        AskForStopThread() : RateThread(100) {
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

public:
    virtual ConstString getName() override { return "RateThreadTest"; }

    double test(int rate, double delay)
    {
        double estPeriod=0;
        RateThread1 *thread1=new RateThread1(rate);

        thread1->start();
        Time::delay(delay);
        thread1->stop();

        estPeriod=thread1->period;

        delete thread1;
        return estPeriod;
    }

    void testInitSuccessFailure()
    {
        report(0,"checking init failure/success notification");
        RateThread2 t(200);
        t.threadWillFail(false);
        t.start();
        checkTrue(t.isRunning(), "thread is running");
        t.stop();
        checkTrue(!t.isRunning(), "thread was stopped");
        checkEqual(1, t.state, "init success was properly notified");

        t.threadWillFail(true);
        t.start();
        checkTrue(!t.isRunning(), "thread stopped");
        checkEqual(-1,t.state, "init failure was properly notified");

        report(0,"done");
    }

    void testInitReleaseSynchro()
    {
        report(0,"Checking init/release synchronization");
        RateThread3 t(200);
        t.threadWillFail(false);
        // if start does not wait for threadRelease/threadInit, a race condition
        // will be detected
        t.start();
        checkEqual(1, t.state, "synchronization on init");
        // if start does not wait for threadRelease/threadInit, a race condition
        // will be detected
        t.stop();
        checkEqual(2, t.state, "synchronization on release");

        t.threadWillFail(true);
        t.start();
        checkEqual(-2, t.state, "synchronization on a failed init");
        report(0,"done");
    }

    void testRateThread() {
        report(0,"testing rate thread precision");
        report(0,"setting high res scheduler (this affects only windows)");

        Time::turboBoost();
        char message[255];

        //try plausible rates
        double p;
        sprintf(message, "Thread1 requested period: %d[ms]", 15);
        report(0, message);
        p=test(15, 1);
        sprintf(message, "Thread1 estimated: %.2lf[ms]", p);
        report(0, message);

        sprintf(message, "Thread2 requested period: %d[ms]", 10);
        report(0, message);
        p=test(10, 1);
        sprintf(message, "Thread2 estimated period: %.2lf[ms]", p);
        report(0, message);

        sprintf(message, "Thread3 requested period: %d[ms]", 1);
        report(0, message);
        p=test(1, 1);
        sprintf(message, "Thread3 estimated period: %.2lf[ms]", p);
        report(0, message);

        report(0, "successful");

        report(0, "Testing askToStop() function");
        RateThread4 thread(10);
        thread.start();

        bool running=true;
        int count=10;
        while((running)&&(count>0))
        {
            running=thread.isRunning();
            Time::delay(0.1);
            count--;
        }

        checkTrue(!thread.isRunning(), "thread terminated correctly");
        //join thread
        thread.stop();
    }

    void testRunnable()
    {
        report(0, "Testing runnable");

        Runnable1 foo;
        RateThreadWrapper t;
        t.setRate(100);
        t.attach(foo);
        t.start();
        checkTrue(t.isRunning(), "thread is running");
        t.close();
        checkTrue(!t.isRunning(), "thread was stopped");
        checkTrue(foo.initCalled, "init was called");
        checkTrue(foo.initNotified, "afterStart() was called");
        checkTrue(foo.releaseCalled, "release was called");

        report(0, "successful");
    }


    void testSimTime() {
        report(0, "Testing simulated time");
        MyClock clock;
        Time::useCustomClock(&clock);
        RateThread5 thread(100*1000); // 100 secs
        thread.start();
        SystemClock clk;
        for (int i=0; i<20; i++) {
            clk.delay(0.1);
            if (thread.count == 1) break;
        }
        checkEqual(thread.count,1,"starting ok");
        clock.t += 100*1000;
        for (int i=0; i<20; i++) {
            clk.delay(0.1);
            if (thread.count == 2) break;
        }
        checkEqual(thread.count,2,"stepping ok");
        clock.done = true;
        thread.stop();
        Time::useSystemClock();
        checkTrue(Time::isSystemClock(), "test is using system clock");
    }

    void testStartAskForStopStart() {
        report(0,"testing start() askForStop() start() sequence...");
        AskForStopThread test;
        int ct = 0;
        while (ct<10) {
            if (!test.isRunning()) {
                test.start();
                checkTrue(test.isRunning(),"isRunning is correct");
                test.done = true;
                ct++;
            }
        }
    }

    virtual void runTests() override {
        testInitSuccessFailure();
        testInitReleaseSynchro();
        testRunnable();
        testRateThread();
        testSimTime();
        testStartAskForStopStart();
    }
};

static RateThreadTest theRateThreadTest;

UnitTest& getRateThreadTest() {
    return theRateThreadTest;
}

