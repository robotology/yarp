/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPCXX_Thread_INC
#define YARPCXX_Thread_INC

#include <yarp/yarpcxx.h>

namespace yarpcxx {
    namespace os {
        class Thread;
    }
}

class yarpcxx::os::Thread {
public:
    Thread() {
        implementation.client = this;
        yarpThreadCallbacksInit(&callbacks);
        yarpThreadInit(&implementation,&callbacks);
        callbacks.run = __impl_run;
        callbacks.beforeStart = __impl_before_start;
        callbacks.afterStart = __impl_after_start;
        callbacks.onStop = __impl_on_stop;
        callbacks.threadInit = __impl_thread_init;
        callbacks.threadRelease = __impl_thread_release;
    }

    virtual ~Thread() {
        yarpThreadFini(&implementation);
    }

    virtual void run() = 0;

    bool start() {
        return !yarpThreadStart(&implementation);
    }

    bool stop() {
        return !yarpThreadStop(&implementation);
    }

    bool isStopping() {
        return yarpThreadIsStopping(&implementation);
    }

    virtual bool threadInit() {
        return true;
    }

    virtual void threadRelease() {
    }

    virtual void beforeStart() {
    }

    virtual void afterStart(bool success) {
    }

    virtual void onStop() {
    }

private:
    yarpThread implementation;
    yarpThreadCallbacks callbacks;

    static int __impl_run(void *client) {
        ((Thread *)client)->run();
        return 0;
    }

    static void __impl_before_start(void *client) {
        ((Thread *)client)->beforeStart();
    }

    static void __impl_after_start(int success, void *client) {
        ((Thread *)client)->afterStart((bool)success);
    }

    static void __impl_on_stop(void *client) {
        ((Thread *)client)->onStop();
    }

    static int __impl_thread_init(void *client) {
        return !((Thread *)client)->threadInit();
    }

    static void __impl_thread_release(void *client) {
        ((Thread *)client)->threadRelease();
    }
};

namespace yarp {
    using namespace yarpcxx;
}

#endif
