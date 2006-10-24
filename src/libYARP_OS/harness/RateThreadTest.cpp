// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/RateThread.h>
#include <yarp/NameServer.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;

class MyRateThread: public RateThread
{
public:
    double t1;
    double t2;
    double average;
    double period;
    int n;

    MyRateThread(int r): RateThread(r){}

    virtual void doInit()
    {
        ACE_OS::printf("-->Starting rate thread: %.2lf[ms]...", getRate());
        n=0;
        t1=0;
        t2=0;
        average=0;
    }

    virtual void doLoop()
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

    virtual void doRelease()
    {
        if (n>0)
            period=1000*average/(n-1);
        else 
            period=0;

        ACE_OS::printf("thread quit\n");
    }

};

class RateThreadTest : public UnitTest {
public:
    virtual String getName() { return "RateThreadTest"; }

    double test(int rate, double delay)
    {
        double estPeriod=0;
        MyRateThread *thread1=new MyRateThread(rate);

        thread1->start();
        Time::delay(delay);
        thread1->stop();

        estPeriod=thread1->period;
        
        delete thread1;
        return estPeriod;
    }

    virtual void testRateThread() {
        char message[255];
        
        report(0,"testing rate thread");

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

    virtual void runTests() {
        testRateThread();
    }
};

static RateThreadTest theRateThreadTest;

UnitTest& getRateThreadTest() {
    return theRateThreadTest;
}

