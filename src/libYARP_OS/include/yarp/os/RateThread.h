/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_RATETHREAD_H
#define YARP_OS_RATETHREAD_H

#include <yarp/os/Runnable.h>

namespace yarp {
    namespace os {
        class RateThread;
        class RateThreadWrapper;
        class SystemRateThread;
    }
}

/**
 * \ingroup key_class
 *
 * An abstraction for a periodic thread.
 */
class YARP_OS_API yarp::os::RateThread {
public:

    /**
     * Constructor.  Thread begins in a dormant state.  Call RateThread::start
     * to get things going.
     * @param period The period in milliseconds [ms] between
     * successive calls to the RateThread::run method
     * (remember you need to call RateThread::start first
     * before anything happens)
     */
    RateThread(int period);

    virtual ~RateThread();

    /**
     * Initialization method. The thread executes this function
     * when it starts and before "run". This is a good place to
     * perform initialization tasks that need to be done by the
     * thread itself (device drivers initialization, memory
     * allocation etc). If the function returns false the thread
     * quits and never calls "run". The return value of threadInit()
     * is notified to the class and passed as a parameter
     * to afterStart(). Note that afterStart() is called by the
     * same thread that is executing the "start" method.
     */
    virtual bool threadInit();

    /**
     * Release method. The thread executes this function once when
     * it exits, after the last "run". This is a good place to release
     * resources that were initialized in threadInit() (release memory,
     * and device driver resources).
     */
     virtual void threadRelease();

    /**
     * Loop function. This is the thread itself.
     */
    virtual void run() = 0;

    /**
     * Call this to start the thread. Blocks until initThread()
     * is executed.
     */
    bool start();

    /**
     * Call this to "step" the thread rather than
     * starting it.  This will execute at most one call
     * to doLoop before returning.
     */
    bool step();

    /**
     * Call this to stop the thread, this call blocks until the
     * thread is terminated (and releaseThread() called). Actually
     * calls join. This will deadlock if called from run(), use
     * askToStop() instead.
     */
    void stop();

     /**
     * Stop the thread. Like stop but it does not call join, safe
     * to be called from run().
     */
    void askToStop();

    /**
     * Returns true when the thread is started, false otherwise.
     */
    bool isRunning();

    /**
     * Returns true when the thread is suspended, false otherwise.
     */
    bool isSuspended();

    /**
     * Set the (new) rate of the thread.
     * @param period the rate [ms]
     * @return true.
     */
    bool setRate(int period);

    /**
     * Return the current rate of the thread.
     * @return thread current rate [ms].
     */
    double getRate();

    /**
     * Suspend the thread, the thread keeps running by doLoop is
     * never executed.
     */
    void suspend();

    /**
     * Resume the thread if previously suspended.
     */
    void resume();

    /**
     * Reset thread statistics.
     */
    void resetStat();

    /**
     * Return estimated period since last reset.
     */
    double getEstPeriod();

    /**
     * Return estimated period since last reset.
     * @param av average value
     * @param std standard deviation
     */
    void getEstPeriod(double &av, double &std);

    /**
     * Return the number of iterations performed since last reset.
     */
    unsigned int getIterations();

    /**
     * Return the estimated duration of the run() function since
     * last reset.
     */
    double getEstUsed();

    /**
     * Return estimated duration of the run() function since last reset.
     * @param av average value
     * @param std standard deviation
     */
    void getEstUsed(double &av, double &std);

    /**
     * Called just before a new thread starts. This method is executed
     * by the same thread that calls start().
     */
    virtual void beforeStart();

    /**
     * Called just after a new thread starts (or fails to start), this
     * is executed by the same thread that calls start().
     * @param success true iff the new thread started successfully.
     */
    virtual void afterStart(bool success);

    /**
     * Set the priority and scheduling policy of the thread, if the OS supports that.
     * @param priority the new priority of the thread.
     * @param policy the scheduling policy of the thread
     * @return -1 if the priority cannot be set.
     * @note The thread plolicy is highly OS dependant and a right combination of
     * priority and policy should be used. In some platform changing thread priorities
     * is subject to having the right permission. For example, the following combinations
     * are supported on most Linux platforms:
     * SCHED_OTHER : policy=0, priority=[0 ..  0]
     * SCHED_FIFO  : policy=1, priority=[1 .. 99]
     * SCHED_RR    : policy=2, priority=[1 .. 99]
     */
    int setPriority(int priority, int policy=-1);

    /**
     * Query the current priority of the thread, if the OS supports that.
     * @return the priority of the thread.
     */
    int getPriority();

    /**
     * @brief Query the current scheduling policy of the thread, if the OS supports that.
     * @return the scheduling policy of the theread.
     */
    int getPolicy();

private:
    bool join(double seconds = -1);

    void *implementation;

    void initWithSystemClock();

friend class SystemRateThread;
};


class YARP_OS_API yarp::os::SystemRateThread : public yarp::os::RateThread
{
private:
    using RateThread::step;

public:
    SystemRateThread(int period);

    virtual ~SystemRateThread();

    bool stepSystem();
};


/**
 * This class takes a Runnable instance and wraps a thread around it.
 * This class is under development - API may change a lot.
 */
class YARP_OS_API yarp::os::RateThreadWrapper : public RateThread {
private:
    Runnable *helper;
    int owned;
public:
    /**
     * Default constructor.
     */
    RateThreadWrapper();
    RateThreadWrapper(Runnable *helper);
    RateThreadWrapper(Runnable& helper);

    virtual ~RateThreadWrapper();

    void detach();
    virtual bool attach(Runnable& helper);
    virtual bool attach(Runnable *helper);

    bool open(double framerate = -1, bool polling = false);
    void close();
    void stop();

    virtual void run() YARP_OVERRIDE;
    virtual bool threadInit() YARP_OVERRIDE;
    virtual void threadRelease() YARP_OVERRIDE;
    virtual void afterStart(bool success) YARP_OVERRIDE;
    virtual void beforeStart() YARP_OVERRIDE;

    Runnable *getAttachment() const;
};

#endif // YARP_OS_RATETHREAD_H
