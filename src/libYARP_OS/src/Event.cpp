// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Event.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os::impl;
using namespace yarp::os;

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE

#include <ace/Auto_Event.h>
#include <ace/Manual_Event.h>

#define EVENT_IMPL(x) (static_cast<ACE_Event*>(x))

yarp::os::Event::Event(bool autoResetAfterWait) {
    if (autoResetAfterWait) {
        implementation = new ACE_Auto_Event;
    } else {
        implementation = new ACE_Manual_Event;
    }
    yAssert(implementation!=NULL);
}


#else

#include <yarp/os/Semaphore.h>
class YarpEventImpl {
private:
    bool autoReset;
    bool signalled;
    int waiters;
    Semaphore stateMutex;
    Semaphore action;
public:
    YarpEventImpl(bool autoReset) : autoReset(autoReset), action(0) {
        signalled = false;
        waiters = 0;
    }

    void wait() {
        stateMutex.wait();
        if (signalled) {
            stateMutex.post();
            return;
        }
        waiters++;
        stateMutex.post();
        action.wait();
        if (autoReset) {
            reset();
        }
    }

    void signal(bool after = true) {
        stateMutex.wait();
        int w = waiters;
        if (w>0) {
            if (autoReset) { w = 1; }
            for (int i=0; i<w; i++) {
                action.post();
                waiters--;
            }
        }
        signalled = after;
        stateMutex.post();
    }

    void reset() {
        stateMutex.wait();
        signalled = false;
        stateMutex.post();
    }
};

#define EVENT_IMPL(x) (static_cast<YarpEventImpl*>(x))

yarp::os::Event::Event(bool autoResetAfterWait) {
    implementation = new YarpEventImpl(autoResetAfterWait);
    yAssert(implementation!=NULL);
}


#endif

yarp::os::Event::~Event() {
    if (implementation!=NULL) {
        delete EVENT_IMPL(implementation);
        implementation = NULL;
    }
}

void yarp::os::Event::wait() {
    EVENT_IMPL(implementation)->wait();
}

void yarp::os::Event::signal() {
    EVENT_IMPL(implementation)->signal();
}

void yarp::os::Event::reset() {
    EVENT_IMPL(implementation)->reset();
}

