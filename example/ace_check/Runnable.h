/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef _ACECHECK_RUNNABLE_
#define _ACECHECK_RUNNABLE_

/**
 * A class that can be managed by a Thread object.
 */
class Runnable {
public:
    virtual ~Runnable() {}

    /**
     * Body to run
     */
    virtual void run() {}


    /**
     * User-defined procedure for stopping execution.  There is no
     * general-purpose way to achieve that.
     */
    virtual void close() {}
  
    /**
     * Called from the creator before the new thread exists
     * and before the associcated Thread::start() call returns
     */
    virtual void beforeStart() {}

    virtual void afterStart(bool success) {}


};

#endif

