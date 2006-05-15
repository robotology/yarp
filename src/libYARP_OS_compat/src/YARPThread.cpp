// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// based on Id: YARPThread.cpp,v 2.0 2005/11/06 22:21:26 gmetta Exp
//
/// $Id: YARPThread.cpp,v 1.2 2006-05-15 15:57:59 eshuy Exp $
///
///

#include <yarp/ThreadImpl.h>
#include <yarp/Logger.h>

#define DEBUG_H_INC
#include <yarp/YARPThread.h>

using namespace yarp;

const size_t DEFAULT_THREAD_STACK_SIZE = 8000;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

class ThreadAdaptor : public ThreadImpl {
private:
    YARPBareThread& owner;
public:
    ThreadAdaptor(YARPBareThread& owner) : owner(owner) {
    }

    virtual void run() {
        owner.Body();
    }
};

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


YARPBareThread::YARPBareThread(void) : sema(0) {
    system_resource = NULL;
    identifier = -1;
    size = -1;
    running = false;
    shutdown_state = YT_None;
    sema.Post();
}

YARPBareThread::YARPBareThread (const YARPBareThread& yt) : sema(0) {
    /// a bit ambiguous?
    system_resource = NULL;
    identifier = yt.identifier;
    running = yt.running;
    shutdown_state = YT_None;
    sema.Post();
}

YARPBareThread::~YARPBareThread (void) {
    if (!IsTerminated()) End();
    if (system_resource!=NULL) {
        delete ((ThreadAdaptor*)system_resource);
        system_resource = NULL;
    }
}


void YARPBareThread::Begin (int stack_size) {
    sema.Wait();
    ACE_ASSERT (running == 0);
    ACE_ASSERT (shutdown_state == YT_None);
  
    if (stack_size <= 0) {
        stack_size = DEFAULT_THREAD_STACK_SIZE;
    }

    ACE_ASSERT (system_resource == NULL);

    if (system_resource == NULL) {
        ThreadAdaptor * thread = new ThreadAdaptor(*this);
        system_resource = thread;
        ACE_ASSERT (system_resource != NULL);

        if (!thread->start()) {
            YARP_ERROR(Logger::get(),"Error in spawning thread");
        }

        static int ct = 0;
        identifier = ct;
        ct++;
        running = true;
        sema.Post();
    }
}

int YARPBareThread::Join (int timeout) {
    sema.Wait();
    if (shutdown_state == YT_AskedEnd)
        shutdown_state = YT_Joining;
    int ok = (system_resource!=NULL);
    sema.Post();
  
    ACE_UNUSED_ARG (timeout);
  
    int r = 0;
  
    if (ok) {
        ((ThreadAdaptor*)system_resource)->close();
        r = ((ThreadAdaptor*)system_resource)->join();
    }
  
    /// if joined (otherwise hung!)
    sema.Wait();
    if (system_resource!=NULL) {
        delete ((ThreadAdaptor*)system_resource);
        system_resource = NULL;
    }
    identifier = -1;
    shutdown_state = YT_None;
    sema.Post();
  
    return r;
}


int YARPBareThread::GetPriority (void) {
    YARP_ERROR(Logger::get(),"GetPriority not yet implemented");
    return 0;
}

int YARPBareThread::SetPriority (int prio) {
    YARP_ERROR(Logger::get(),"SetPriority not yet implemented");
    return 0;
}

void YARPBareThread::End(int dontkill) {
    YARP_DEBUG(Logger::get(),"YARPThread::End doesn't do what it used to");
    Join();
}


int YARPBareThread::IsTerminated (void) {
    sema.Wait();
    int r = !running;
    sema.Post();
    return r;
}


void YARPBareThread::AskForEnd (void) {
    sema.Wait();
    if (shutdown_state != YT_None && shutdown_state != YT_AskedEnd)
        {
            sema.Post();
            return;
        }
    running = false;
    shutdown_state = YT_AskedEnd;
    sema.Post();
}

void YARPBareThread::CleanState (void) {
    sema.Wait();
    shutdown_state = YT_None;
    if (system_resource!=NULL) {
        delete ((ThreadAdaptor*)system_resource);
        system_resource = NULL;
    }
    identifier = -1;
    running = false;
    sema.Post();
}
