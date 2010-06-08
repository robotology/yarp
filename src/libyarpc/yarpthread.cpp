// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>

#include "yarp.h"
#include "yarpimpl.h"


    /**
     *
     * Initialize thread callbacks.
     *
     */
YARP_DEFINE(int) yarpThreadCallbacksInit(yarpThreadCallbacksPtr callbacks) {
    YARP_OK0(callbacks);
    callbacks->run = NULL;
    callbacks->afterStart = NULL;
    callbacks->onStop = NULL;
    return 0;
}

class YarpcxxThread : public Thread {
public:
    YarpcxxThread(yarpThreadCallbacksPtr callbacks,
                  void *client) : callbacks(callbacks), client(client) {
    }
    
    virtual void run() {
        if (callbacks->run) callbacks->run(client);
    }

    virtual void afterStart(bool success) {
        if (callbacks->afterStart) {
            callbacks->afterStart(success,client);
        }
    }
    
    virtual void onStop() {
        if (callbacks->onStop) callbacks->onStop(client);
    }
    
private:
    yarpThreadCallbacksPtr callbacks;
    void *client;
};

YARP_DEFINE(int) yarpThreadInit(yarpThreadPtr thread,
                                yarpThreadCallbacksPtr callbacks) {
    YARP_OK0(thread);
    YARP_OK0(callbacks);
    thread->implementation = NULL;
    thread->callbacks = callbacks;
    YarpcxxThread *t = new YarpcxxThread(callbacks,thread);
    if (t==NULL) return -1;
    thread->implementation = t;
    return 0;
}

YARP_DEFINE(int) yarpThreadFini(yarpThreadPtr thread) {
    YARP_OK(thread);
    delete &YARP_THREAD(thread);
    thread->implementation = NULL;
    return 0;
}

YARP_DEFINE(int) yarpThreadStart(yarpThreadPtr thread) {
    YARP_OK(thread);
    YARP_THREAD(thread).start();
    return 0;
}


YARP_DEFINE(int) yarpThreadStop(yarpThreadPtr thread) {
    YARP_OK(thread);
    YARP_THREAD(thread).stop();
    return 0;
}


