// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_RATETHREAD_
#define _YARP2_RATETHREAD_


namespace yarp {
    namespace os {
        class RateThread;
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

    virtual void doInit();
    virtual void doRelease();
    virtual void doLoop()=0;

    bool start();
    bool stop();

    bool isRunning();

    bool setRate(int p);

    void suspend();
    void resume();

private:
    bool join(double seconds = -1);

    void *implementation;
};

#endif

