/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "ThreadImpl.h"
#include "SemaphoreImpl.h"


int ThreadImpl::threadCount = 0;
SemaphoreImpl ThreadImpl::threadMutex(1);


#ifdef __WIN32__
static unsigned __stdcall theExecutiveBranch (void *args)
#else
    unsigned theExecutiveBranch (void *args)
#endif
{
    ACE_OS::signal(SIGPIPE, SIG_IGN);

    ThreadImpl *thread = (ThreadImpl *)args;
    thread->run();
    ThreadImpl::changeCount(-1);
    //ACE_Thread::exit();
    return 0;
}


ThreadImpl::ThreadImpl() {
    delegate = NULL;
    active = false;
    closing = false;
    setOptions();
}


ThreadImpl::ThreadImpl(Runnable *target) {
    delegate = target;
    active = false;
    closing = false;
    setOptions();
}


ThreadImpl::~ThreadImpl() {
    if (active) {
        join();
    }
}


long int ThreadImpl::getKey() {
    // if id doesn't fit in long int, should do local translation
    return (long int)id;
}


void ThreadImpl::setOptions(int stackSize) {
    this->stackSize = stackSize;
}

int ThreadImpl::join(double seconds) {
    closing = true;
    if (active) {
        int result = ACE_Thread::join(hid);
        active = false;
        return result;
    }
    return 0;
}

void ThreadImpl::run() {
    if (delegate!=NULL) {
        delegate->run();
    }
}

void ThreadImpl::close() {
    closing = true;
    if (delegate!=NULL) {
        delegate->close();
    }
}

void ThreadImpl::beforeStart() {
    if (delegate!=NULL) {
        delegate->beforeStart();
    }
}

void ThreadImpl::afterStart(bool success) {
    if (delegate!=NULL) {
        delegate->afterStart(success);
    }
}

bool ThreadImpl::start() {
    closing = false;
    beforeStart();
    int result = ACE_Thread::spawn((ACE_THR_FUNC)theExecutiveBranch,
                                   (void *)this,
                                   THR_JOINABLE | THR_NEW_LWP,
                                   &id,
                                   &hid,
                                   ACE_DEFAULT_THREAD_PRIORITY,
                                   0,
                                   (size_t)stackSize);
    if (result==0) {
        ThreadImpl::changeCount(1);
        active = true;
        afterStart(true);
    } else {
        afterStart(false);
    }
    return result==0;
}


bool ThreadImpl::isClosing() {
    return closing;
}

int ThreadImpl::getCount() {
    threadMutex.wait();
    int ct = threadCount;
    threadMutex.post();
    return ct;
}


void ThreadImpl::changeCount(int delta) {
    threadMutex.wait();
    threadCount+=delta;
    threadMutex.post();
}


