// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

// added test for threadInit/threadRelease functions, synchronization and 
// thread init success/failure notification -nat

#include <yarp/os/RateThread.h>
#include <yarp/NameServer.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#include <yarp/UnitTest.h>
//#include "TestList.h"

using namespace yarp;
using namespace yarp::os;

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
		
		virtual bool threadInit()
		{
			ACE_OS::printf("-->Starting rate thread: %.2lf[ms]...", getRate());
			n=0;
			t1=0;
			t2=0;
			average=0;
			
			return true;
		}
		
		virtual void run()
		{
			t2=Time::now();
			
			if (n>0)
            {
                average+=(t2-t1);        
            }
			n++;
			t1=t2;
			
			// ACE_OS::printf(".");
		}
		
		virtual void threadRelease()
		{
			if (n>0)
				period=1000*average/(n-1);
			else 
				period=0;
			
			ACE_OS::printf("thread quit\n");
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
		
		virtual bool threadInit()
		{
			state=-1;
			return !fail;
		}
		
		virtual void afterStart(bool s)
		{
			if (s)
				state=0;
		}
		
		virtual void run()
		{}
		
		virtual void threadRelease()
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
		
		virtual bool threadInit()
		{
			Time::delay(0.5);
			state++;
			return !fail;
		}
		
		virtual void afterStart(bool s)
		{
			if (s)
				state++;
			else
				state=-2;
		}
		
		virtual void run()
		{}
		
		virtual void threadRelease()
		{ 
			Time::delay(0.5);
			state++;
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

		virtual bool threadInit()
		{
			initCalled=true;
			return true;
		}

		virtual void threadRelease()
		{
			releaseCalled=true;
		}

		virtual void run()
		{}

		virtual void afterStart(bool s)
		{
			initNotified=true;
		}

	};

public:
    virtual String getName() { return "RateThreadTest"; }

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
        ACE_OS::sprintf(message, "Thread1 requested period: %d[ms]", 15);
        report(0, message);
        p=test(15, 1);
        ACE_OS::sprintf(message, "Thread1 estimated: %.2lf[ms]", p);
        report(0, message);
        
        ACE_OS::sprintf(message, "Thread2 requested period: %d[ms]", 10);
        report(0, message);
        p=test(10, 1);
        ACE_OS::sprintf(message, "Thread2 estimated period: %.2lf[ms]", p);
        report(0, message);

        ACE_OS::sprintf(message, "Thread3 requested period: %d[ms]", 1);
        report(0, message);
        p=test(1, 1);
        ACE_OS::sprintf(message, "Thread3 estimated period: %.2lf[ms]", p);
        report(0, message);

        report(0, "successful");
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

    virtual void runTests() {
   		testInitSuccessFailure();
		testInitReleaseSynchro();
		testRunnable();
		testRateThread();
    }
};

static RateThreadTest theRateThreadTest;

UnitTest& getRateThreadTest() {
    return theRateThreadTest;
}

