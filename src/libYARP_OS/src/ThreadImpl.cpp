// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/ThreadImpl.h>
#include <yarp/SemaphoreImpl.h>
#include <yarp/Logger.h>
#include <yarp/IOException.h>

using namespace yarp;

int ThreadImpl::threadCount = 0;
SemaphoreImpl ThreadImpl::threadMutex(1);


#ifdef __WIN32__
static unsigned __stdcall theExecutiveBranch (void *args)
#else
    unsigned theExecutiveBranch (void *args)
#endif
{
    // just for now -- rather deal with broken pipes through normal procedures
    ACE_OS::signal(SIGPIPE, SIG_IGN);

    ThreadImpl *thread = (ThreadImpl *)args;
    try {
        thread->run();
    } catch (IOException e) {
        YARP_ERROR(Logger::get(),String("uncaught exception in thread: ") +
                   e.toString());
    }
    ThreadImpl::changeCount(-1);
    YARP_DEBUG(Logger::get(),"Thread shutting down");
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
    YARP_DEBUG(Logger::get(),"Thread being deleted");
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


