// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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
        ACE_OS::printf("Starting rate thread: %.2lf[ms]...", getRate());
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
        
        ACE_OS::printf("Thread looped %d times\n", thread1->n);
        ACE_OS::printf("->Requested period was %d[ms], estimated period is %.2lf[ms]\n", 
            rate, estPeriod); 
 
        delete thread1;
        return estPeriod;
    }

    virtual void testRateThread() {
        report(0,"testing rate thread");

        double estimated;
        //try plausible rates
        double p1=test(15, 1);
        double p2=test(10, 1);
        double p3=test(1,  1);

        if (fabs(p3-1)<0.5)
            estimated=1;
        else if (fabs(p2-10)<1)
            estimated=10;
        else if (fabs(p3-15)<1)
            estimated=15;
        else
            estimated=-1;

        report(0, "RateThread test was successful");
        
        if (estimated!=-1)
        {
            ACE_OS::printf("I was able to run threads with period up to %.0lf[ms]\n", estimated);
        }
        else
        {
            ACE_OS::printf("Timing of periodic threads was not reliable. ");
            ACE_OS::printf("Note: this might be due to many factors (e.g. cpu load) and should");
            ACE_OS::printf(" not be considered a problem\n");
        }
        
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

