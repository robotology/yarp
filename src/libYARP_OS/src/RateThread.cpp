// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Lorenzo Natale.
* Copyright (C) 2006 The Robotcub consortium
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*/


#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/impl/Logger.h>
#include <ace/Synch.h>
#include <ace/Time_Value.h>
#include <ace/High_Res_Timer.h>
#include <yarp/os/Semaphore.h>

#include <yarp/os/Time.h>

#include <math.h> //sqrt

//added threadRelease/threadInit methods and synchronization -nat 

using namespace yarp::os::impl;
using namespace yarp::os;

const ACE_Time_Value _timeout_value(20,0);	// (20 sec) timeout value for the release (20 sec)

class RateThreadCallbackAdapter: public ThreadImpl 
{
private:
    unsigned int period;
    double adaptedPeriod;
    RateThread& owner; 
    Semaphore mutex;
    ACE_Time_Value now;
    ACE_Time_Value currentRunTV;
    ACE_Time_Value previousRunTV;
    ACE_Time_Value sleep;
    ACE_Time_Value sleepPeriodTV;
    ACE_High_Res_Timer	thread_timer;	// timer to estimate thread time
    double    	        sleep_period;	// thread sleep

    bool suspended;
    double totalUsed;      //total time taken iterations
    unsigned int count;    //number of iterations from last reset
    unsigned int estPIt;   //number of useful iterations for period estimation
    double totalT;         //time bw run, accumulated
    double sumTSq;         //cumulative sum sq of estimated period dT
    double sumUsedSq;      //cumulative sum sq of estimated thread tun
    double previousRun;    //time when last iteration started
    double currentRun;     //time when this iteration started
    bool scheduleReset; 

    void _resetStat()
    {
        totalUsed=0;
        count=0;
        estPIt=0;
        totalT=0;
        sumUsedSq=0;
        sumTSq=0;
        scheduleReset=false;
    }

    inline ACE_Time_Value getTime()
    {
#ifdef ACE_WIN32
        now = ACE_High_Res_Timer::gettimeofday_hr();
#else
        now = ACE_OS::gettimeofday ();
#endif
        return now;
    }
    
    inline void sleepThread(ACE_Time_Value sleep_period)
    {
        if (sleep_period.usec() < 0 || sleep_period.sec() < 0)
            sleep_period.set(0,0);
        ACE_OS::sleep(sleep_period);
    }
    
    inline double toDouble(const ACE_Time_Value &v)
    {
        return double(v.sec()) + v.usec() * 1e-6; 
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
        double ret;
        lock();
        if (estPIt==0)
            ret=0;
        else
            ret=totalT/estPIt;
        unlock();
        return ret;
    }

    void getEstPeriod(double &av, double &std)
    {
        lock();
        if (estPIt==0)
            {
                av=0;
                std=0;
            }
        else
            {
                av=totalT/estPIt;
                if (estPIt>1)
                    {
                        std=sqrt(((1.0/(estPIt-1))*(sumTSq-estPIt*av*av)));
                    }
                else
                    std=0;
            }

        unlock();
    }
    
    unsigned int getIterations()
    { 
        lock();
        unsigned int ret=count;
        unlock();
        return ret;
    }

    double getEstUsed()
    { 
        double ret;
        lock();
        if (count<1)
            ret=0.0;
        else
            ret=totalUsed/count;
        unlock();
        return ret;
    }

    void getEstUsed(double &av, double &std)
    {
        lock();
        if (count<1)
            {
                av=0;
                std=0;
            }
        else
            {
                av=totalUsed/count;
                if (count>1)
                    {
                        std=sqrt((1.0/(count-1))*(sumUsedSq-count*av*av));
                    }
                else
                    std=0;
            }

        unlock();
    }


    void singleStep()
    {
        lock();
        currentRunTV=getTime();
        currentRun=toDouble(currentRunTV);
        
        if (scheduleReset)
            _resetStat();

        if (count>0)
            {
                //double saved=adaptedPeriod;
                double dT=(currentRun-previousRun)*1000;
                sumTSq+=dT*dT;
                totalT+=dT;
                //double error=(static_cast<double>(period)-dT);
                //adaptedPeriod+=0.0*error; //not available
                if (adaptedPeriod<0)
                    adaptedPeriod=0;

                //fprintf(stderr, "dT:%lf, error %lf, adaptedPeriod: %lf, new:%lf\n", dT, error, saved, adaptedPeriod);
                estPIt++;
            }

        previousRun=currentRun;
        unlock();

        if (!suspended)
            {   
                owner.run();
            }
        
        count++;
        lock();

        ACE_Time_Value elapsedTV=getTime();
        double elapsed=toDouble(elapsedTV)-currentRun;

        //save last
        totalUsed+=elapsed*1000;
        sumUsedSq+=elapsed*1000*elapsed*1000;
        unlock();

        //compute sleep time
        sleepPeriodTV.msec(static_cast<int>(adaptedPeriod+0.5));
        sleepPeriodTV+=currentRunTV;
        sleepPeriodTV-=elapsedTV;
        // Time::delay(sleep_period/1000.0);
        sleepThread(sleepPeriodTV);
    }

    virtual void run() 
    {
        adaptedPeriod=period;
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
        adaptedPeriod=period;
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

void RateThread::getEstPeriod(double &av, double &std)
{
    ((RateThreadCallbackAdapter*)implementation)->getEstPeriod(av, std);
}

void RateThread::getEstUsed(double &av, double &std)
{
    ((RateThreadCallbackAdapter*)implementation)->getEstUsed(av,std);
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
