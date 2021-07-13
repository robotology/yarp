/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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

    void run() override {
        if (callbacks->run) callbacks->run(client);
    }

    void beforeStart() override {
        if (callbacks->beforeStart) {
            callbacks->beforeStart(client);
        }
    }

    void afterStart(bool success) override {
        if (callbacks->afterStart) {
            callbacks->afterStart(success,client);
        }
    }

    void onStop() override {
        if (callbacks->onStop) callbacks->onStop(client);
    }

    bool threadInit() override {
        if (callbacks->threadInit) return !callbacks->threadInit(client);
        return true;
    }

    void threadRelease() override {
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
