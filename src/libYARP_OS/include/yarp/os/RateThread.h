// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_RATETHREAD_
#define _YARP2_RATETHREAD_

#include <yarp/os/Runnable.h>

namespace yarp {
    namespace os {
        class RateThread;
        class RateThreadWrapper;
    }
}

/**
 * \ingroup key_class
 *
 * An abstraction for a periodic thread.
 */
class yarp::os::RateThread {
public:

    /**
     * Constructor.  Thread begins in a dormat state.  Call Thread::start
     * to get things going.
     */
    RateThread(int period);

    virtual ~RateThread();

    /**
     * Initialization method. This function is executed once when
     * the thread starts and before the first doLoop. The function
     * is executed by the thread itself.
     */
    virtual void doInit();
    
    /**
     * Release method. This function is executed once when
     * the thread exits and after the last doLoop. The function
     * is executed by the thread itself.
     */
    virtual void doRelease();

    /**
     * Loop function. This is the period thread itself.
     */
    virtual void doLoop()=0;

    /** 
     * Call this to start the thread. Blocks until doInit()
     * is executed.
     */
    bool start();
 
    /** 
     * Call this to stop the thread, this call blocks until the 
     * thread is terminated (and doRelease() called). Actually 
     * calls join.
     */
    bool stop();

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
     * @param p the rate [ms]
     * @return true.
     */
    bool setRate(int p);

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

private:
    bool join(double seconds = -1);

    void *implementation;
};



/**
 * This class takes a Runnable instance and wraps a thread around it.
 * This class is under development - API may change a lot.
 */
class yarp::os::RateThreadWrapper : public RateThread {
private:
    Runnable *helper;
    int owned;
public:
  /**
   * Default constructor.
   */
    RateThreadWrapper(): RateThread(0) {
        helper = 0/*NULL*/;
        owned = false;
    }

    RateThreadWrapper(Runnable *helper): RateThread(0) {
        this->helper = helper;
        owned = true;
    }

    RateThreadWrapper(Runnable& helper): RateThread(0) {
        this->helper = &helper;
        owned = false;
    }
    
    virtual ~RateThreadWrapper() {
        detach();
    }

    void detach() {
        if (owned) {
            if (helper!=0/*NULL*/) {
                delete helper;
            }
        }
        helper = 0/*NULL*/;
        owned = false;
    }
  
    virtual bool attach(Runnable& helper) {
        detach();
        this->helper = &helper;
        owned = false;
        return true;
    }
    
    virtual bool attach(Runnable *helper) {
        detach();
        this->helper = helper;
        owned = true;
        return true;
    }

    bool open(double framerate = -1);

    bool close() {
        stop();
        return true;
    }

    virtual void doLoop() {
        if (helper!=0/*NULL*/) {
            helper->run();
        }
    }
};

#endif

