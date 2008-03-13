// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Lorenzo Natale
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

//added threadRelease/threadInit methods and synchronization -nat 

using namespace yarp;
using namespace yarp::os;

const ACE_Time_Value _timeout_value(20,0);	// (20 sec) timeout value for the release (20 sec)

class RateThreadCallbackAdapter: public ThreadImpl 
{
private:
    unsigned int period;
    RateThread& owner; 
    Semaphore mutex;
    ACE_Time_Value now;
    ACE_Time_Value sleep;
    ACE_High_Res_Timer	thread_timer;	// timer to estimate thread time
    double    	        sleep_period;	// thread sleep

    bool suspended;
    double totalUsed;      //total time taken iterations
    unsigned int count;    //number of iterations from last reset
    double totalT;         //time bw run, accumulated
    double previousRun;    //time when last iteration started
    double currentRun;     //time when this iteration started
    bool scheduleReset; 

    void _resetStat()
    {
        totalUsed=0;
        count=0;
        totalT=0;
        scheduleReset=false;
    }

public:

    RateThreadCallbackAdapter(RateThread& owner, int p) : owner(owner) 
    {
        period=p;
        suspended = false;
        _resetStat();
    }

    void resetStat()
    {
        scheduleReset=true;
    }

    double getEstPeriod()
    { 
        if (count<1)
            return 0.0;
        return totalT/(count);
    }
    
    unsigned int getIterations()
    { return count; }

    double getEstUsed()
    { 
        if (count<1)
            return 0.0;
        return totalUsed/count;
    }

    inline double getTime()
    {        
#ifdef ACE_WIN32
        now = ACE_High_Res_Timer::gettimeofday_hr();
#else
        now = ACE_OS::gettimeofday ();
#endif
        return double(now.sec()) + now.usec() * 1e-6; 
    }

    inline void sleepThread(double seconds) 
    {
        sleep.sec (long(seconds));
        sleep.usec (long((seconds-long(seconds)) * 1.0e6));
        ACE_OS::sleep(sleep);
    }


    void singleStep()
    {
        currentRun=getTime();

        if (scheduleReset)
            _resetStat();

        if (count>0)
            totalT+=(currentRun-previousRun)*1000;
        previousRun=currentRun;

        if (!suspended)
            {   
                owner.run();
                count++;
            }

        double elapsed=getTime()-currentRun;
        //save last
        totalUsed+=elapsed*1000;

        //compute the sleep time
        if (elapsed*1000<period)
            sleep_period = period-(elapsed*1000);
        else
            sleep_period=0;

        Time::delay(sleep_period/1000.0);
#if 0
        int us=sleep_period.usec()%1000;
        if (us>=500)
            sleep_period = sleep_period+ACE_Time_Value(0, 1000-us);
        else
            sleep_period = sleep_period-ACE_Time_Value(0, us);
        
        if (sleep_period.usec() < 0 || sleep_period.sec() < 0)
            sleep_period.set(0,0);
#endif
    }

    virtual void run() 
    {
        while(!isClosing())
            {
                singleStep();
            }
    }

	virtual bool threadInit()
	{
		return owner.threadInit();
	}

	virtual void threadRelease()
	{
		owner.threadRelease();
	}

    bool setRate(int p)
    {
        period=p;
        return true;
    }

    double getRate()
    {
        return period;
    }

    bool isSuspended()
    { return suspended; }

    void suspend()
    { suspended=true; }

    void resume()
    { suspended=false; }

	virtual void afterStart(bool s)
	{ owner.afterStart(s); }

	virtual void beforeStart()
	{ owner.beforeStart(); }

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

void RateThread::stop() {
    ((ThreadImpl*)implementation)->close();
    //return true;
}

bool RateThread::step() {
    ((RateThreadCallbackAdapter*)implementation)->singleStep();
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

unsigned int RateThread::getIterations()
{
    return ((RateThreadCallbackAdapter*)implementation)->getIterations();
}

double RateThread::getEstPeriod()
{
    return ((RateThreadCallbackAdapter*)implementation)->getEstPeriod();
}

double RateThread::getEstUsed()
{
    return ((RateThreadCallbackAdapter*)implementation)->getEstUsed();
}

void RateThread::resetStat()
{
    ((RateThreadCallbackAdapter*)implementation)->resetStat();
}

bool RateThread::threadInit()
{
	return true;
}

void RateThread::threadRelease()
{}

void RateThread::beforeStart()
{}

void RateThread::afterStart(bool s)
{}

bool RateThreadWrapper::open(double framerate, bool polling) {
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
    if (!polling) {
        start();
    }
    return true;
}
