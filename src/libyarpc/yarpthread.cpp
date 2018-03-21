/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>

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
    callbacks->beforeStart = NULL;
    callbacks->afterStart = NULL;
    callbacks->onStop = NULL;
    callbacks->threadInit = NULL;
    callbacks->threadRelease = NULL;
    callbacks->unused4 = NULL;
    callbacks->unused5 = NULL;
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

    virtual void beforeStart() {
        if (callbacks->beforeStart) {
            callbacks->beforeStart(client);
        }
    }

    virtual void afterStart(bool success) {
        if (callbacks->afterStart) {
            callbacks->afterStart(success,client);
        }
    }

    virtual void onStop() {
        if (callbacks->onStop) callbacks->onStop(client);
    }

    virtual bool threadInit() {
        if (callbacks->threadInit) return !callbacks->threadInit(client);
        return true;
    }

    virtual void threadRelease() {
        if (callbacks->threadRelease) callbacks->threadRelease(client);
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
    YarpcxxThread *t = new YarpcxxThread(callbacks,thread->client);
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


YARP_DEFINE(int) yarpThreadIsStopping(yarpThreadPtr thread) {
    YARP_OK(thread);
    return YARP_THREAD(thread).isStopping();
}

