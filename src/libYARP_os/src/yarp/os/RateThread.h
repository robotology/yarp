/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_RATETHREAD_H
#define YARP_OS_RATETHREAD_H

#include <yarp/conf/system.h>

#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/os/RateThread.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0 (RateThreadWrapper since YARP 3.3)
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING

#include <yarp/os/api.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Runnable.h>

namespace yarp {
namespace os {

/**
 * \ingroup key_class
 *
 * An abstraction for a periodic thread.
 *
 * @deprecated since YARP 3.0.0
 */
class YARP_os_DEPRECATED_API_MSG("Use PeriodicThread instead") RateThread : private PeriodicThread
{
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
    void getEstPeriod(double& av, double& std);

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
    void getEstUsed(double& av, double& std);

    /**
     * Set the priority and scheduling policy of the thread, if the OS supports that.
     * @param priority the new priority of the thread.
     * @param policy the scheduling policy of the thread
     * @return -1 if the priority cannot be set.
     * @note The thread policy is highly OS dependent and a right combination of
     * priority and policy should be used. In some platform changing thread priorities
     * is subject to having the right permission. For example, the following combinations
     * are supported on most Linux platforms:
     * SCHED_OTHER : policy=0, priority=[0 ..  0]
     * SCHED_FIFO  : policy=1, priority=[1 .. 99]
     * SCHED_RR    : policy=2, priority=[1 .. 99]
     */
    int setPriority(int priority, int policy = -1);

    /**
     * Query the current priority of the thread, if the OS supports that.
     * @return the priority of the thread.
     */
    int getPriority();

    /**
     * @brief Query the current scheduling policy of the thread, if the OS supports that.
     * @return the scheduling policy of the thread.
     */
    int getPolicy();

protected:
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
    bool threadInit() override;

    /**
     * Release method. The thread executes this function once when
     * it exits, after the last "run". This is a good place to release
     * resources that were initialized in threadInit() (release memory,
     * and device driver resources).
     */
    void threadRelease() override;

    /**
     * Loop function. This is the thread itself.
     * The thread calls the run() function every <period> ms.
     * At the end of each run, the thread will sleep the amounth of time
     * required, taking into account the time spent inside the loop function.
     * Example:  requested period is 10ms, the run() function take 3ms to
     * be executed, the thread will sleep for 7ms.
     *
     * Note: after each run is completed, the thread will call a yield()
     * in order to facilitate other threads to run.
     */
    void run() override = 0;

    /**
     * Called just before a new thread starts. This method is executed
     * by the same thread that calls start().
     */
    void beforeStart() override;

    /**
     * Called just after a new thread starts (or fails to start), this
     * is executed by the same thread that calls start().
     * @param success true iff the new thread started successfully.
     */
    void afterStart(bool success) override;
};

/**
 * @deprecated since YARP 3.0
 */
class YARP_os_DEPRECATED_API_MSG("Use PeriodicThread(..., == ShouldUseSystemClock::Yes) instead") SystemRateThread : public PeriodicThread
{
public:
    SystemRateThread(int period);

    virtual ~SystemRateThread();

    bool stepSystem();
};


/**
 * This class takes a Runnable instance and wraps a thread around it.
 *
 * @deprecated since YARP 3.3
 */
class YARP_os_DEPRECATED_API RateThreadWrapper : public PeriodicThread
{
private:
    yarp::os::Runnable* helper;
    bool owned;

public:
    /**
     * Default constructor.
     */
    RateThreadWrapper();
    RateThreadWrapper(Runnable* helper);
    RateThreadWrapper(Runnable& helper);

    virtual ~RateThreadWrapper();

    void detach();
    virtual bool attach(Runnable& helper);
    virtual bool attach(Runnable* helper);

    bool open(double framerate = -1, bool polling = false);
    void close();
    void stop();

    void run() override;
    bool threadInit() override;
    void threadRelease() override;
    void afterStart(bool success) override;
    void beforeStart() override;

    Runnable* getAttachment() const;
};

} // namespace os
} // namespace yarp

YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED

#endif // YARP_OS_RATETHREAD_H
