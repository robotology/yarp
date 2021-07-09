/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_THREAD_H
#define YARP_OS_THREAD_H

#include <yarp/os/api.h>

namespace yarp {
namespace os {


/**
 * \ingroup key_class
 *
 * An abstraction for a thread of execution.
 */
class YARP_os_API Thread
{
public:
    /**
     * Constructor.  Thread begins in a dormant state.  Call Thread::start
     * to get things going.
     */
    Thread();

    /**
     * Destructor.
     */
    virtual ~Thread();

    /**
     * Main body of the new thread.
     * Override this method to do what you want.
     * After Thread::start is called, this
     * method will start running in a separate thread.
     * It is important that this method either keeps checking
     * Thread::isStopping to see if it should stop, or
     * you override the Thread::onStop method to interact
     * with it in some way to shut the new thread down.
     * There is no really reliable, portable way to stop
     * a thread cleanly unless that thread cooperates.
     */
    virtual void run() = 0;

    /**
     * Call-back, called while halting the thread (before
     * join). This callback is executed by the same thread
     * that calls stop(). It should not be called directly.
     * Override this method to do the right thing for
     * your particular Thread::run.
     */
    virtual void onStop();

    /**
     * Start the new thread running.
     * The new thread will call the user-defined Thread::run method.
     * The function starts the thread and waits until the thread executes
     * threadInit(). If the initialization was not successful the thread
     * exits, otherwise run is executed. The return value of threadInit()
     * is passed to afterStart().
     * @return true iff the new thread starts successfully
     */
    bool start();

    /**
     * Stop the thread.
     * Thread::isStopping will start returning true.
     * The user-defined Thread::onStop method will be called.
     * Then, this simply sits back and waits. Wait for thread
     * termination so cannot be called from within run().
     * @return true iff the thread stops successfully
     */
    bool stop();


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
    virtual bool threadInit()
    {
        return true;
    }

    /**
     * Release method. The thread executes this function once when
     * it exits, after the last "run". This is a good place to release
     * resources that were initialized in threadInit() (release memory,
     * and device driver resources).
     */
    virtual void threadRelease()
    {
    }


    /**
     * Returns true if the thread is stopping (Thread::stop has
     * been called).
     * @return true iff the thread is stopping
     */
    bool isStopping();

    /**
     * Returns true if the thread is running (Thread::start has
     * been called successfully and the thread has not stopped).
     * @return true iff the thread is running
     */
    bool isRunning();

    /**
     * Check how many threads are running.
     * @return the number of threads currently running
     */
    static int getCount();

    /**
     * Get a unique identifier for the thread.
     * @return an identifier that is different for each thread within a process
     */
    long int getKey();

    /**
     * Get a unique identifier for the calling thread.
     * @return an identifier for the calling thread
     */
    static long int getKeyOfCaller();

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

    /**
     * The function returns when the thread execution has completed.
     * Stops the execution of the thread that calls this function until either
     * the thread to join has finished execution (when it returns from run())
     * or after \a seconds seconds.
     * @param seconds the maximum number of seconds to block the thread.
     * @return true if the thread execution is finished, false on time out.
     */
    bool join(double seconds = -1);

    /**
     * Reschedule the execution of current thread, allowing other threads to run.
     */
    static void yield();


#ifndef YARP_NO_DEPRECATED // since YARP 3.0.0
    /**
     * Set the stack size for the new thread.  Must be called before
     * Thread::start
     * @param stackSize the desired stack size in bytes (if 0, uses
     *                  a reasonable default)
     * @deprecated since YARP 3.0.0
     */
    YARP_DEPRECATED
    void setOptions(int stackSize = 0)
    {
    }

    /**
     * Set the default stack size for all threads created after this
     * point.  A value of 0 will use a reasonable default.
     * @param stackSize the desired stack size in bytes.
     */
    YARP_DEPRECATED
    static void setDefaultStackSize(int stackSize)
    {
    }
#endif // YARP_NO_DEPRECATED


#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* const mPriv;
#endif
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_THREAD_H
