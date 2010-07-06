// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2008 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

// initThread/releaseThread methods, synchronization on init failure/success -nat

#ifndef _YARP2_THREAD_
#define _YARP2_THREAD_


namespace yarp {
    namespace os {
        class Thread;
    }
}

/**
 * \ingroup key_class
 *
 * An abstraction for a thread of execution.
 */
class yarp::os::Thread {
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
    virtual void run()=0;

    /**
     * Call-back, called while halting the thread (before
     * join).
     * Should not be called directly.
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
	{ return true;}

	/**
     * Release method. The thread executes this function once when
     * it exits, after the last "run". This is a good place to release
	 * resources that were initialized in threadInit() (release memory, 
	 * and device driver resources).
     */
	virtual void threadRelease()
	{}


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
     * Set the stack size for the new thread.  Must be called before
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

    /**
     * Set the priority of the thread, if the OS supports that.
     * @param priority the new priority of the thread.
     * @return -1 if the priority cannot be set.
     */
    int setPriority(int priority);

    /**
     * Query the current priority of the thread, if the OS supports that.
     * @return the priority of the thread.
     */
    int getPriority();


    /**
     *
     * Set the default stack size for all threads created after this
     * point.  A value of 0 will use a reasonable default.
     *
     * @param stackSize the desired stack size in bytes.
     *
     */
    static void setDefaultStackSize(int stackSize);

private:
    bool join(double seconds = -1);


    void *implementation;
    bool stopping;
};

#endif

