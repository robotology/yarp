/*
* Author: Lorenzo Natale.
* Copyright (C) 2006 The Robotcub consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/


#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PlatformTime.h>
#include <yarp/os/Semaphore.h>

#include <yarp/os/Time.h>
#include <yarp/os/SystemClock.h>

#include <cmath> //sqrt

//added threadRelease/threadInit methods and synchronization -nat

using namespace yarp::os::impl;
using namespace yarp::os;

//const YARP_timeval _timeout_value(20, 0);    // (20 sec) timeout value for the release (20 sec)

class RateThreadCallbackAdapter: public ThreadImpl
{
private:
    float period_ms;
    double adaptedPeriod;
    RateThread& owner;
    Semaphore mutex;

    bool useSystemClock;   // force to use the system clock. Used by SystemRateThread only

    double  elapsed;
    double  sleepPeriod;

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

    void _resetStat() {
        totalUsed=0;
        count=0;
        estPIt=0;
        totalT=0;
        sumUsedSq=0;
        sumTSq=0;
        elapsed=0;
        scheduleReset=false;
    }

public:

    RateThreadCallbackAdapter(RateThread& owner, int p) : owner(owner), useSystemClock(false) {
        period_ms=p;
        elapsed=0;
        suspended = false;
        _resetStat();
    }

    void initWithSystemClock()
    {
        useSystemClock = true;
    }

    void resetStat() {
        scheduleReset=true;
    }

    double getEstPeriod() {
        double ret;
        lock();
        if (estPIt==0)
            ret=0;
        else
            ret=(totalT/estPIt) *1000;
        unlock();
        return ret;
    }

    void getEstPeriod(double &av, double &std) {
        lock();
        if (estPIt==0) {
            av=0;
            std=0;
        } else {
            av=totalT/estPIt;
            if (estPIt>1) {
                std=sqrt(((1.0/(estPIt-1))*(sumTSq-estPIt*av*av))) *1000;  // av is computed in [secs], while user expects data in [ms]
            } else {
                std=0;
            }
            av*=1000;    // av is computed in [secs], while user expects data in [ms]
        }
        unlock();
    }

    unsigned int getIterations() {
        lock();
        unsigned int ret=count;
        unlock();
        return ret;
    }

    double getEstUsed() {
        double ret;
        lock();
        if (count<1)
            ret=0.0;
        else
            ret=totalUsed/count*1000;
        unlock();
        return ret;
    }

    void getEstUsed(double &av, double &std) {
        lock();
        if (count<1) {
            av=0;
            std=0;
        } else {
            av=totalUsed/count;
            if (count>1) {
                std=sqrt((1.0/(count-1))*(sumUsedSq-count*av*av)) *1000;
            } else {
                std=0;
            }
            av*=1000;
        }
        unlock();
    }


    void singleStep() {
        lock();
        currentRun = Time::now();

        if (scheduleReset)
            _resetStat();

        if (count>0) {
            double dT=(currentRun-previousRun);
            sumTSq+=dT*dT;
            totalT+=dT;
            if (adaptedPeriod<0)
                adaptedPeriod=0;

            //fprintf(stderr, "dT:%lf, error %lf, adaptedPeriod: %lf, new:%lf\n", dT, error, saved, adaptedPeriod);
            estPIt++;
        }

        previousRun=currentRun;
        unlock();

        if (!suspended) {
            owner.run();
        }

        count++;
        lock();

        double elapsed = yarp::os::Time::now() - currentRun;

        //save last
        totalUsed+=elapsed;
        sumUsedSq+=elapsed*elapsed;
        unlock();

        sleepPeriod= adaptedPeriod - elapsed; // everything is in [seconds] except period, for it is used in the interface as [ms]
        // Check if sleepPeriod is negative here or inside the delay (or both?)
        yarp::os::Time::delay(sleepPeriod);
    }

    void run() override {
        adaptedPeriod = period_ms/1000.0;   //  divide by 1000 because user's period is [ms] while all the rest is [secs]
        while(!isClosing())
        {
            if(useSystemClock)
                singleStepSystem();
            else
                singleStep();
        }
    }

    void singleStepSystem()
    {
        lock();
        currentRun = SystemClock::nowSystem();

        if (scheduleReset)
            _resetStat();

        if (count>0)
        {
            double dT=(currentRun-previousRun); // *1000;

            sumTSq+=dT*dT;
            totalT+=dT;

            if (adaptedPeriod<0)
                adaptedPeriod=0;

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

        double elapsed = SystemClock::nowSystem() - currentRun;

        //save last
        totalUsed+=elapsed;
        sumUsedSq+=elapsed*elapsed;
        unlock();

        sleepPeriod= adaptedPeriod - elapsed;  //  all time computatio are done in [sec]
        // Check if sleepPeriod is negative here or inside the delay (or both?)
        SystemClock::delaySystem(sleepPeriod);
    }

    bool threadInit() override {
        return owner.threadInit();
    }

    void threadRelease() override {
        owner.threadRelease();
    }

    bool setRate(int p) {
        period_ms=p;
        adaptedPeriod = period_ms/1000.0;   //  divide by 1000 because user's period is [ms] while all the rest is [secs]
        return true;
    }

    double getRate() {
        return period_ms;
    }

    bool isSuspended() {
        return suspended;
    }

    void suspend() {
        suspended=true;
    }

    void resume() {
        suspended=false;
    }

    void afterStart(bool s) override {
        owner.afterStart(s);
    }

    void beforeStart() override {
        owner.beforeStart();
    }

    void lock() {
        mutex.wait();
    }

    void unlock() {
        mutex.post();
    }
}; // class RateThreadCallbackAdapter



RateThread::RateThread(int period)
{
    // use period
    implementation = new RateThreadCallbackAdapter(*this, period);
    yAssert(implementation!=YARP_NULLPTR);
}

RateThread::~RateThread()
{
    if (implementation!=YARP_NULLPTR) {
        delete ((RateThreadCallbackAdapter*)implementation);
        implementation = YARP_NULLPTR;
    }
}

void RateThread::initWithSystemClock()
{
    ((RateThreadCallbackAdapter*)implementation)->initWithSystemClock();
}

bool RateThread::setRate(int period)
{
    return ((RateThreadCallbackAdapter*)implementation)->setRate(period);
}

double RateThread::getRate()
{
    return ((RateThreadCallbackAdapter*)implementation)->getRate();
}

bool RateThread::isSuspended()
{
    return ((RateThreadCallbackAdapter*)implementation)->isSuspended();
}

bool RateThread::join(double seconds)
{
    return ((ThreadImpl*)implementation)->join(seconds);
}

void RateThread::stop()
{
    ((ThreadImpl*)implementation)->close();
}

void RateThread::askToStop()
{
    ((ThreadImpl*)implementation)->askToClose();
}

bool RateThread::step()
{
    ((RateThreadCallbackAdapter*)implementation)->singleStep();
    return true;
}

bool RateThread::start()
{
    return ((ThreadImpl*)implementation)->start();
}

bool RateThread::isRunning()
{
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
    ((RateThreadCallbackAdapter*)implementation)->getEstUsed(av, std);
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

void RateThread::afterStart(bool success)
{}

int RateThread::setPriority(int priority, int policy)
{
    return ((ThreadImpl*)implementation)->setPriority(priority, policy);
}

int RateThread::getPriority()
{
    return ((ThreadImpl*)implementation)->getPriority();
}

int RateThread::getPolicy()
{
    return ((ThreadImpl*)implementation)->getPolicy();
}

//
//  System Rate Thread
//

SystemRateThread::SystemRateThread(int period) : RateThread(period)
{
    RateThread::initWithSystemClock();
}

SystemRateThread::~SystemRateThread()
{ }

bool SystemRateThread::stepSystem()
{
    ((RateThreadCallbackAdapter*)implementation)->singleStepSystem();
    return true;
}


RateThreadWrapper::RateThreadWrapper(): RateThread(0) {
    helper = YARP_NULLPTR;
    owned = false;
}


RateThreadWrapper::RateThreadWrapper(Runnable *helper): RateThread(0) {
    this->helper = helper;
    owned = true;
}

RateThreadWrapper::RateThreadWrapper(Runnable& helper): RateThread(0) {
    this->helper = &helper;
    owned = false;
}

RateThreadWrapper::~RateThreadWrapper() {
    detach();
}

void RateThreadWrapper::detach() {
    if (owned) {
        if (helper!=YARP_NULLPTR) {
            delete helper;
        }
    }
    helper = YARP_NULLPTR;
    owned = false;
}

bool RateThreadWrapper::attach(Runnable& helper) {
    detach();
    this->helper = &helper;
    owned = false;
    return true;
}

bool RateThreadWrapper::attach(Runnable *helper) {
    detach();
    this->helper = helper;
    owned = true;
    return true;
}

bool RateThreadWrapper::open(double framerate, bool polling) {
    int period = 0;
    if (framerate>0) {
        period=(int) (0.5+1000.0/framerate);
        YARP_SPRINTF2(Logger::get(), info,
                      "Setting framerate to: %.0lf[Hz] (thread period %d[ms])\n",
                      framerate, period);
    } else {
        YARP_SPRINTF0(Logger::get(), info,
                      "No framerate specified, polling the device");
        period=0; //continuous
    }
    RateThread::setRate(period);
    if (!polling) {
        start();
    }
    return true;
}

void RateThreadWrapper::close() {
    RateThread::stop();
}

void RateThreadWrapper::stop() {
    RateThread::stop();
}

void RateThreadWrapper::run() {
    if (helper!=YARP_NULLPTR) {
        helper->run();
    }
}

bool RateThreadWrapper::threadInit() {
    if (helper!=YARP_NULLPTR) {
        return helper->threadInit();
    }
    else
        return true;
}

void RateThreadWrapper::threadRelease() {
    if (helper!=YARP_NULLPTR) {
        helper->threadRelease();
    }
}

void RateThreadWrapper::afterStart(bool success) {
    if (helper!=YARP_NULLPTR) {
        helper->afterStart(success);
    }
}

void RateThreadWrapper::beforeStart() {
    if (helper!=YARP_NULLPTR) {
        helper->beforeStart();
    }
}

Runnable *RateThreadWrapper::getAttachment() const {
    return helper;
}
