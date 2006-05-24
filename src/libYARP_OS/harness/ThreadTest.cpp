// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/ThreadImpl.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;


class ThreadTest : public UnitTest {
public:

    Semaphore sema;
    Semaphore state;
    int expectCount;
    int gotCount;


private:

    class Thread0: public Thread {
    public:
        virtual void run() {
            Time::delay(0.01);
        }
    };

    class Thread1 : public Runnable {
    public:
        ThreadTest& owner;

        Thread1(ThreadTest& owner) : owner(owner) {}

        virtual void run() {
            for (int i=0; i<5; i++) {
                //ACE_OS::printf("tick %d\n", i);
                owner.state.wait();
                owner.expectCount++;
                owner.state.post();
                owner.sema.post();
                Time::delay(0.1);
            }
        }
    };
  
  
    class Thread2 : public ThreadImpl {
    public:
        ThreadTest& owner;

        Thread2(ThreadTest& owner) : mutex(1), finished(false), owner(owner) {}
    
        virtual void run() {
            bool done = false;
            while (!done) {
                owner.sema.wait();
                mutex.wait();
                done = finished;
                mutex.post();
                owner.state.wait();
                //ACE_OS::printf("burp\n");
                owner.gotCount++;
                owner.state.post();
            }
            while (owner.sema.check()) {
                owner.state.wait();
                owner.gotCount++;
                owner.state.post();
            }
            //ACE_OS::printf("done\n");
        }
    
        virtual void close() {
            mutex.wait();
            finished = true;
            owner.state.wait();
            owner.expectCount++;
            owner.state.post();
            owner.sema.post();
            mutex.post();
        }

    private:
        Semaphore mutex;
        bool finished;
    };


    class Thread3: public Thread {
    public:
        virtual void run() {
            Time::delay(1);
        }
    };



public:
    ThreadTest() : sema(0), state(1) {
        expectCount = 0;
        gotCount = 0;
    }

    virtual yarp::String getName() { return "ThreadTest"; }

    void testSync() {
        report(0,"testing cross-thread synchronization...");
        int tct = ThreadImpl::getCount();
        Thread1 bozo(*this);
        Thread1 bozo2(*this);
        Thread2 burper(*this);
        ThreadImpl t1(&bozo);
        ThreadImpl t2(&bozo2);
        report(0,"starting threads ...");
        burper.start();
        t1.start();
        Time::delay(0.05);
        t2.start();
        checkEqual(ThreadImpl::getCount(),tct+3,"thread count");
        t1.join();
        t2.join();
        burper.close();
        burper.join();
        report(0,"... done threads");
        checkEqual(expectCount,gotCount,"thread event counts");
        checkEqual(true,expectCount==11,"thread event counts");
    }

    void testCloseVersusStop() {
        report(0,"testing that onStop called directly doesn't cause crash");
        Thread3 t;
        t.start();
        t.stop();
        report(0,"done");
    }

    virtual void testMin() {
        report(0,"testing minimal thread functions to check for mem leakage...");
        for (int i=0; i<20; i++) {
            Thread0 t0, t1;
            t0.start();
            t1.start();
            t0.stop();
            t1.stop();
        }
        report(0,"...done");
    }

    virtual void runTests() {
        testMin();
        testSync();
        testCloseVersusStop();
    }
};

static ThreadTest theThreadTest;

UnitTest& getThreadTest() {
    return theThreadTest;
}

