/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef _ACECHECK_THREAD_
#define _ACECHECK_THREAD_


/**
 * An abstraction for a thread of execution.
 */
class Thread {
public:

    /**
     * Constructor.  Thread begins in a dormat state.  Call Thread::start
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
    virtual void run();

    /**
     * Call-back, called while halting the thread.
     * Should not be called directly.
     * Override this method to do the right thing for
     * your particular Thread::run.
     */
    virtual void onStop();

    /**
     * Start the new thread running.
     * The new thread will call the user-defined Thread::run method.
     * @return true iff the new thread starts successfully
     */
    bool start();

    /**
     * Stop the new thread.
     * Thread::isStopping will start returning true.
     * The user-defined Thread::onStop method will be called.
     * Then, this simply sits back and waits.
     * @return true iff the new thread stops successfully
     */
    bool stop();

    /**
     * Called just before a new thread starts.
     */
    virtual void beforeStart();

    /**
     * Called just after a new thread starts (or fails to start).
     * @param success true iff the new thread started successfully
     */
    virtual void afterStart(bool success);

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
     * Set the stack size the the new thread.  Must be called before
     * Thread::start
     * @param stackSize the desired stack size in bytes (if 0, uses
     *                  a reasonable default)
     */
    void setOptions(int stackSize = 0);

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

private:
    bool join(double seconds = -1);


    void *implementation;
};

#endif

