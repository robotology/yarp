/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>
#include <mutex>
#include <yarp/os/Time.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os::impl;
using namespace yarp::os;


static Semaphore sema{0};
static Semaphore state{1};
static int expectCount{0};
static int gotCount{0};


namespace {

class ThreadImmediateReturn : public Thread
{
    public:
        void run() override
        {
        }
};

class ThreadDelay: public Thread
{
public:
    double delay;
    bool hold;
    bool active{true};
    std::mutex mutex;

    ThreadDelay(double delay = 0.5, bool hold = false) :
            delay(delay),
            hold(hold)
    {
    }

    void run() override
    {
        bool h;
        do {
            Time::delay(delay);
            mutex.lock();
            h = hold;
            mutex.unlock();
        } while (h);
        active = false;
    }
};

class Thread0: public Thread
{
public:
    void run() override
    {
        Time::delay(0.01);
    }
};

class Thread1 : public Thread
{
public:
    void run() override
    {
        for (int i=0; i<5; i++) {
            //printf("tick %d\n", i);
            state.wait();
            expectCount++;
            state.post();
            sema.post();
            Time::delay(0.1);
        }
    }
};

class Thread2 : public Thread
{
public:
    void run() override
    {
        bool done = false;
        while (!done) {
            sema.wait();
            mutex.lock();
            done = finished;
            mutex.unlock();
            state.wait();
            //printf("burp\n");
            gotCount++;
            state.post();
        }
        while (sema.check()) {
            state.wait();
            gotCount++;
            state.post();
        }
        //printf("done\n");
    }

    void onStop() override
    {
        mutex.lock();
        finished = true;
        state.wait();
        expectCount++;
        state.post();
        sema.post();
        mutex.unlock();
    }

private:
    std::mutex mutex;
    bool finished{false};
};

class Thread3 : public Thread
{
public:
    bool onStopCalled{false};
    int state{-1};

    bool threadInit() override
    {
        onStopCalled=false;
        state=0;
        return true;
    }

    void run() override
    {
        Time::delay(0.5);
        state++;
    }

    void threadRelease() override
    {
        state++;
    }

    void onStop() override
    {
        onStopCalled=true;
    }
};

class Thread4 : public Thread
{
public:
    int state{-1};
    bool fail{false};

    bool threadInit() override
    {
        state=0;
        return !fail;
    }

    void threadWillFail(bool f)
    {
        fail=f;
    }

    void afterStart(bool s) override
    {
        if (!s) {
            state=-1;
        }
    }

    void run() override
    {
        Time::delay(1);
        state++;
    }

    void threadRelease() override
    {
        state++;
    }
};

class Thread5: public Thread
{
public:
    int state{0};
    bool fail{false};
    std::mutex mutex;

    void threadWillFail(bool f)
    {
        mutex.lock();
        state=0;
        fail=f;
        mutex.unlock();
    }

    bool threadInit() override
    {
        Time::delay(0.5);
        mutex.lock();
        state=1;
        mutex.unlock();
        return !fail;
    }

    void afterStart(bool s) override
    {
        mutex.lock();
        if (!s) {
            state++;
        }
        mutex.unlock();
    }

    void run() override
    {
    }

    void threadRelease() override
    {
        Time::delay(0.5);
        mutex.lock();
        state++;
        mutex.unlock();
    }
};

class ThreadIdentity : public Thread
{
public:
    long int dynamicId{-1};
    long int staticId{-1};

    void run() override
    {
        dynamicId = getKey();
        staticId = Thread::getKeyOfCaller();
    }
};
} // namespace

TEST_CASE("os::ThreadTest", "[yarp::os]")
{
    SECTION("testing isRunning function")
    {
        ThreadDelay foo;
        foo.start();
        CHECK(foo.isRunning()); // thread is running
        foo.stop(); //calls join
        CHECK(!foo.isRunning()); // thread quit
        INFO("done");
    }

    SECTION("testing cross-thread synchronization...")
    {
        int tct = Thread::getCount();
        Thread1    t1;
        Thread1    t2;
        Thread2    burper;
        INFO("starting threads ...");
        burper.start();
        t1.start();
        Time::delay(0.05);
        t2.start();
        CHECK(Thread::getCount() == tct+3); // thread count
        CHECK(t1.join()); // thread t1 joined succesfully
        CHECK(t2.join()); // thread t2 joined succesfully
        burper.stop();
        CHECK(burper.join()); // thread burper joined succesfully
        INFO("... done threads");
        CHECK(expectCount == gotCount); // thread event counts
        CHECK(expectCount==11); // thread event counts

        INFO("done");
    }

    SECTION("testing start/stop")
    {
        Thread3 t;
        CHECK(!t.isRunning()); // not active
        CHECK(t.start()); // t started succefully
        CHECK(t.isRunning()); // t is active
        CHECK(t.stop()); // t stopped succefully
        CHECK(!t.isRunning()); // not active
        CHECK(t.onStopCalled); // onStop was called

        INFO("done");
    }

    SECTION("Checking init/release functions are actually called")
    {
        Thread3 t;
        t.start();
        t.stop();
        CHECK(t.state==2); // init/release were called
        INFO("done");
    }

    SECTION("Checking init failure/success notification")
    {
        Thread4 foo;
        foo.threadWillFail(false);
        foo.start();
        CHECK(foo.isRunning()); // Thread is running
        foo.stop();
        CHECK(2 == foo.state); // Init success was properly notified

        foo.threadWillFail(true);
        foo.start();
        CHECK(!foo.isRunning()); // Thread is not running
        CHECK(-1 == foo.state); // Init failure was properly notified

        INFO("done");
    }

    SECTION("Checking init/release synchronization")
    {
        Thread5 t;
        INFO("Starting thread... thread will wait 0.5 second");
        t.start();
        t.mutex.lock();
        CHECK(1 == t.state); // Start synchronized on init
        t.mutex.unlock();

        INFO("Stopping thread... thread will wait 0.5 second");
        t.stop();
        t.mutex.lock();
        CHECK(2 == t.state); // Stop synchronized on release
        t.mutex.unlock();

        t.threadWillFail(true);
        t.start();
        t.mutex.lock();
        CHECK(2 == t.state); // Start synchronized on failed init
        t.mutex.unlock();

        INFO("done");
    }

    SECTION("testing minimal thread functions to check for mem leakage...")
    {
        for (int i=0; i<20; i++) {
            Thread0 t0, t1;
            t0.start();
            t1.start();
            t0.stop();
            t1.stop();
        }
        INFO("...done");
    }


    SECTION("testing running flag (bug #1695724)...")
    {
        ThreadDelay t1;
        CHECK_FALSE(t1.isRunning()); // not running before start
        t1.start();
        CHECK(t1.isRunning()); // running after start
        t1.stop();
        CHECK_FALSE(t1.isRunning()); // not running after stop
        t1.start();
        CHECK(t1.isRunning()); // running after start
        t1.stop();
        CHECK_FALSE(t1.isRunning()); // not running after stop

        ThreadImmediateReturn t0;
        t0.start();
        Time::delay(0.5);
        CHECK_FALSE(t0.isRunning()); // not running after thread exits
    }

    SECTION("testing IDs...")
    {
        ThreadIdentity t[3];
        for (int i = 0; i < 3; i++) {
            t[i].start();
        }
        for (int i = 0; i < 3; i++) {
            t[i].stop();
        }
        for (int i=0; i<3; i++) {
            CHECK(t[i].staticId == t[i].dynamicId); // IDs match appropriately
        }
        for (int i=0; i<3; i++) {
            for (int j=0; j<3; j++) {
                if (i == j) {
                    continue;
                }
                CHECK_FALSE(t[i].staticId==t[j].staticId); // IDs differ appropriately
            }
        }
    }

    SECTION("testing join timeout...")
    {
        ThreadDelay t(0.1, true);
        CHECK(t.active); // flag starts out ok
        t.start();
        CHECK(t.join(1) == 0); // thread t joined succesfully before 1 second timeout
        CHECK(t.active); // timeout join returns before thread stops
        t.mutex.lock();
        t.hold = false;
        t.mutex.unlock();
        t.stop();
        CHECK_FALSE(t.active); // flag behaves correctly
        t.hold = false;
    }
}
