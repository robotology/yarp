// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/ThreadImpl.h>
#include <yarp/os/RateThread.h>
#include <yarp/Logger.h>
//#include <ace/Thread_Manager.h>
#include <ace/Synch.h>
#include <ace/Time_Value.h>
#include <ace/High_Res_Timer.h>
#include <yarp/os/Semaphore.h>

#include <yarp/os/Time.h>

using namespace yarp;
using namespace yarp::os;

const ACE_Time_Value _timeout_value(20,0);	// (20 sec) timeout value for the release (20 sec)

class RateThreadCallbackAdapter: public ThreadImpl 
{
private:
    ACE_Time_Value period;
    RateThread& owner; 
	ACE_Auto_Event synchro;	// event for init synchro
    Semaphore mutex;

    bool suspended;

public:

    RateThreadCallbackAdapter(RateThread& owner, int p) : owner(owner) 
    {
        period.set(0, p*1000);	//period here is usec
        suspended=false;
    }

    virtual bool start()
    {
        bool ret=ThreadImpl::start();

        synchro.wait();

        return ret;
    }

    virtual void close()
    {
        ThreadImpl::join(-1);
    }

    virtual void run() 
    {
		ACE_High_Res_Timer	thread_timer;	// timer to estimate thread time
		ACE_Time_Value		est_time;		// thread time
		ACE_Time_Value		sleep_period;	// thread sleep

        owner.doInit();

        synchro.signal();

        while(!isClosing())
            {
                thread_timer.start();

                if (!suspended)
                    owner.doLoop();
                
                thread_timer.stop();

                thread_timer.elapsed_time(est_time);
                //             fprintf(stderr, "Elasped: %d %d\n", est_time.usec(), est_time.sec());
                
                //compute the sleep time
                sleep_period = period-est_time;

                // round to 1ms. I noticed that ACE_OS::sleep() has a different 
                // 'rounding' behavior depending on the os. This could be an ACE 
                // problem because win32 and cygwin behaves differently. We should 
                // check the implementation -- nat
                int us=sleep_period.usec()%1000;
                if (us>=500)
                    sleep_period = sleep_period+ACE_Time_Value(0, 1000-us);
                else
                    sleep_period = sleep_period-ACE_Time_Value(0, us);
			
                if (sleep_period.usec() < 0 || sleep_period.sec() < 0)
                    sleep_period.set(0,0);

                //              fprintf(stderr, "Sleep: %d %d\n", sleep_period.usec(), sleep_period.sec());
                ACE_OS::sleep(sleep_period);
                //Time::delay(sleep_period.sec()+sleep_period.usec()/1000000.0);
            }


        if (!suspended)
            owner.doRelease();
		// synchro.signal(); not required, stop is synchronizing on join
    }

    bool setRate(int p)
    {
        period.set(0, p*1000);	//period here is usec
        return true;
    }

    double getRate()
    {
        return period.usec()/1000.0;
    }

    bool isSuspended()
    { return suspended; }

    void suspend()
    { suspended=true; }

    void resume()
    { suspended=false; }

    void lock() {mutex.wait();}
    void unlock() {mutex.post();}
};


RateThread::RateThread(int p) 
{
    // use p
    implementation = new RateThreadCallbackAdapter(*this, p);
    YARP_ASSERT(implementation!=0);
}

RateThread::~RateThread() {
    if (implementation!=0) {
        delete ((RateThreadCallbackAdapter*)implementation);
        implementation = 0;
    }
}

bool RateThread::setRate(int p)
{
    return ((RateThreadCallbackAdapter*)implementation)->setRate(p);
}

double RateThread::getRate()
{
    return ((RateThreadCallbackAdapter*)implementation)->getRate();
}

bool RateThread::isSuspended()
{
    return ((RateThreadCallbackAdapter*)implementation)->isSuspended();
}

bool RateThread::join(double seconds) {
    return ((ThreadImpl*)implementation)->join(seconds);
}

bool RateThread::stop() {
    ((ThreadImpl*)implementation)->close();
    return true;
}

bool RateThread::start() {
    return ((ThreadImpl*)implementation)->start();
}

bool RateThread::isRunning() {
    return ((ThreadImpl*)implementation)->isRunning();
}

void RateThread::suspend()
{ 
    ((RateThreadCallbackAdapter*)implementation)->suspend();
}

void RateThread::resume()
{
    ((RateThreadCallbackAdapter*)implementation)->resume();
}

void RateThread::doInit()
{}

void RateThread::doRelease()
{}




bool RateThreadWrapper::open(double framerate) {
    int period = 0;
    if (framerate>0) {
        period=(int) (0.5+1000.0/framerate);
        ACE_OS::printf("Setting framerate to: %.0lf[Hz] (thread period %d[ms])\n", 
                       framerate, period);
    } else {
        ACE_OS::printf("No framerate specified, polling the device\n");
        period=0; //continuous
    }    
    RateThread::setRate(period);
    start();
    return true;
}
