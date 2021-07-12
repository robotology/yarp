/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_PERIODICTHREAD_H
#define YARP_OS_PERIODICTHREAD_H

#include <yarp/os/api.h>
#include <yarp/os/Time.h>

namespace yarp {
namespace os {

enum class PeriodicThreadClock
{
    Relative,
    Absolute
};

/**
 * \ingroup key_class
 *
 * An abstraction for a periodic thread.
 */
class YARP_os_API PeriodicThread
{
public:
    /**
     * Constructor.  Thread begins in a dormant state.  Call PeriodicThread::start
     * to get things going.
     * @param period The period in seconds [sec] between successive calls to the
     * PeriodicThread::run method (remember you need to call PeriodicThread::start
     * first before anything happens)
     * @param useSystemClock whether the thread should always use the system clock,
     * or depend on the current configuration of the network
     * @param clockAccuracy whether the thread should ensure all steps wake up
     * at precise intervals using an absolute reference, otherwise compute them
     * in a relative fashion assuming error accumulation over time due to drifts.
     * @warning PeriodicThreadClock::Absolute may cause starvation if two or more
     * busy threads lock on a common resource
     */
    explicit PeriodicThread(double period,
                            ShouldUseSystemClock useSystemClock = ShouldUseSystemClock::No,
                            PeriodicThreadClock clockAccuracy = PeriodicThreadClock::Relative);

    /**
     * Constructor.  Thread begins in a dormant state.  Call PeriodicThread::start
     * to get things going.
     * @param period The period in seconds [sec] between successive calls to the
     * PeriodicThread::run method (remember you need to call PeriodicThread::start
     * first before anything happens)
     * @param clockAccuracy whether the thread should ensure all steps wake up
     * at precise intervals using an absolute reference, otherwise compute them
     * in a relative fashion assuming error accumulation over time due to drifts
     * @warning PeriodicThreadClock::Absolute may cause starvation if two or more
     * busy threads lock on a common resource
     */
    explicit PeriodicThread(double period, PeriodicThreadClock clockAccuracy);

    virtual ~PeriodicThread();

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
    void step();

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
    bool isRunning() const;

    /**
     * Returns true when the thread is suspended, false otherwise.
     */
    bool isSuspended() const;

    /**
     * Set the (new) period of the thread.
     * @param period the period [sec]
     * @return true.
     */
    bool setPeriod(double period);

    /**
     * @brief Return the current period of the thread.
     * @return thread current period [sec].
     */
    double getPeriod() const;

    /**
     * @brief Suspend the thread, the thread keeps running by doLoop is
     * never executed.
     */
    void suspend();

    /**
     * @brief Resume the thread if previously suspended.
     */
    void resume();

    /**
     * @brief Reset thread statistics.
     */
    void resetStat();

    /**
     * @brief Return estimated period since last reset.
     */
    double getEstimatedPeriod() const;

    /**
     * @brief Return estimated period since last reset.
     * @param[out] av average value
     * @param[out] std standard deviation
     */
    void getEstimatedPeriod(double& av, double& std) const;

    /**
     * @brief Return the number of iterations performed since last reset.
     */
    unsigned int getIterations() const;

    /**
     * @brief Return the estimated duration of the run() function since
     * last reset.
     */
    double getEstimatedUsed() const;

    /**
     * @brief Return estimated duration of the run() function since last reset.
     * @param[out] av average value
     * @param[out] std standard deviation
     */
    void getEstimatedUsed(double& av, double& std) const;

    /**
     * @brief Set the priority and scheduling policy of the thread, if the OS supports that.
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
     * @brief Query the current priority of the thread, if the OS supports that.
     * @return the priority of the thread.
     */
    int getPriority() const;

    /**
     * @brief Query the current scheduling policy of the thread, if the OS supports that.
     * @return the scheduling policy of the thread.
     */
    int getPolicy() const;

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
     * The thread calls the run() function every <period> ms.
     * At the end of each run, the thread will sleep the amounth of time
     * required, taking into account the time spent inside the loop function.
     * Example:  requested period is 10ms, the run() function take 3ms to
     * be executed, the thread will sleep for 7ms.
     *
     * Note: after each run is completed, the thread will call a yield()
     * in order to facilitate other threads to run.
     */
    virtual void run() = 0;

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PERIODICTHREAD_H
