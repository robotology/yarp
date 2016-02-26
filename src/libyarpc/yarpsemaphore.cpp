/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "yarp.h"
#include "yarpimpl.h"

#include <yarp/os/Semaphore.h>

YARP_DEFINE(yarpSemaphorePtr) yarpSemaphoreCreate(int initial) {
    yarpSemaphorePtr sema = new yarpSemaphore;
    if (sema!=NULL) {
        sema->implementation = new Semaphore(initial);
        if (sema->implementation==NULL) {
            delete sema;
            sema = NULL;
        }
    }
    return sema;
}


YARP_DEFINE(void) yarpSemaphoreFree(yarpSemaphorePtr sema) {
    if (sema!=NULL) {
        if (sema->implementation!=NULL) {
            delete (Semaphore*)(sema->implementation);
            sema->implementation = NULL;
        }
        delete sema;
    }
}


YARP_DEFINE(int) yarpSemaphoreWait(yarpSemaphorePtr sema) {
    if (sema!=NULL) {
        if (sema->implementation!=NULL) {
            ((Semaphore*)(sema->implementation))->wait();
            return 0;
        }
    }
    return -1;
}


YARP_DECLARE(int) yarpSemaphorePost(yarpSemaphorePtr sema) {
    if (sema!=NULL) {
        if (sema->implementation!=NULL) {
            ((Semaphore*)(sema->implementation))->post();
            return 0;
        }
    }
    return -1;
}


YARP_DECLARE(int) yarpSemaphoreCheck(yarpSemaphorePtr sema) {
    if (sema!=NULL) {
        if (sema->implementation!=NULL) {
            bool result = ((Semaphore*)(sema->implementation))->check();
            return result?0:1;
        }
    }
    return -1;
}


