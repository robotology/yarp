// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_OS_RUNNABLE_
#define _YARP2_OS_RUNNABLE_

namespace yarp {
    namespace os {
        class Runnable;
    }
}

/**
 * A class that can be managed by another thread.
 */
class yarp::os::Runnable {
public:
    virtual ~Runnable() {}

    /**
     * Body to run - could be periodic or continuous
     */
    virtual void run() {}


    /**
     * User-defined procedure for stopping execution.  There is no
     * general-purpose way to achieve that.
     */
    virtual void close() {}
  
    /**
     * Should be called from the creator *before* the thread exists
     * and before a call that requested the thread returns
     */
    virtual void beforeStart() {}

    /**
     * Should be called from the creator *after* the thread exists
     * and before a call that requested the thread returns
     */
    virtual void afterStart(bool success) {}


};

#endif

