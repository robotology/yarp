/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/NetType.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PlatformSignal.h>
#include <yarp/os/impl/PlatformThread.h>
#include <yarp/os/impl/SemaphoreImpl.h>

#include <cstdlib>
#include <thread>

#if defined(YARP_HAS_CXX11)
#  if defined(YARP_HAS_ACE)
#    include <ace/Thread.h> // For using ACE_hthread_t as native_handle
#  endif
#endif

#if defined(__linux__) // Use the POSIX syscalls for the gettid()
    #include <sys/syscall.h>
    #include <unistd.h>
#endif

using namespace yarp::os::impl;

int ThreadImpl::threadCount = 0;
int ThreadImpl::defaultStackSize = 0;
SemaphoreImpl *ThreadImpl::threadMutex = nullptr;
SemaphoreImpl *ThreadImpl::timeMutex = nullptr;

void ThreadImpl::init()
{
    if (!threadMutex) {
        threadMutex = new SemaphoreImpl(1);
    }
    if (!timeMutex) {
        timeMutex = new SemaphoreImpl(1);
    }
}

void ThreadImpl::fini()
{
    if (threadMutex) {
        delete threadMutex;
        threadMutex = nullptr;
    }
    if (timeMutex) {
        delete timeMutex;
        timeMutex = nullptr;
    }
}


PLATFORM_THREAD_RETURN theExecutiveBranch (void *args)
{
    // just for now -- rather deal with broken pipes through normal procedures
    yarp::os::impl::signal(SIGPIPE, SIG_IGN);


    /*
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGHUP);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGCHLD);
    ACE_OS::thr_sigsetmask(SIG_BLOCK, &set, nullptr);
    fprintf(stderr, "Blocking signals\n");
    */

    ThreadImpl *thread = (ThreadImpl *)args;

    YARP_DEBUG(Logger::get(), "Thread starting up");

    bool success=thread->threadInit();
    thread->notify(success);
    thread->notifyOpened(success);
    thread->synchroPost();

    if (success) {
        // c++11 std::thread, pthread and ace threads on some platforms do not
        // return the thread id, therefore it must be set before calling run(),
        // to avoid a race condition in case the run() method checks it.
        thread->id = PLATFORM_THREAD_SELF();

#if defined(__linux__)
        // Use the POSIX syscalls to get
        // the real thread ID (gettid) on Linux machine
        thread->tid = (long) syscall(SYS_gettid);
#else
        thread->tid = (long int)thread->id;
#endif

        thread->setPriority();
        thread->run();
        thread->threadRelease();
    }


    //YARP_ERROR(Logger::get(), ConstString("uncaught exception in thread: ") +
    //             e.toString());

    ThreadImpl::changeCount(-1);
    YARP_DEBUG(Logger::get(), "Thread shutting down");
    //ACE_Thread::exit();

    thread->notify(false);
    thread->synchroPost();

    return 0;
}


ThreadImpl::ThreadImpl() :
        tid(-1),
        id(0),
        defaultPriority(-1),
        defaultPolicy(-1),
#if defined(YARP_HAS_CXX11)
        hid(std::thread()),
#else
        hid(0),
#endif
        active(false),
        opened(false),
        closing(false),
        needJoin(false),
        delegate(nullptr),
        synchro(0),
        initWasSuccessful(false)
{
    setOptions();
}


ThreadImpl::ThreadImpl(Runnable *target) :
        tid(-1),
        id(0),
        defaultPriority(-1),
        defaultPolicy(-1),
#if defined(YARP_HAS_CXX11)
        hid(std::thread()),
#else
        hid(0),
#endif
        active(false),
        opened(false),
        closing(false),
        needJoin(false),
        delegate(target),
        synchro(0),
        initWasSuccessful(false)
{
    setOptions();
}


ThreadImpl::~ThreadImpl()
{
    YARP_DEBUG(Logger::get(), "Thread being deleted");
    join();
}


long int ThreadImpl::getKey()
{
    // if id doesn't fit in long int, should do local translation
    return (long int)id;
}


long int ThreadImpl::getKeyOfCaller()
{
    return (long int)PLATFORM_THREAD_SELF();
}


void ThreadImpl::setOptions(int stackSize)
{
    this->stackSize = stackSize;
}


int ThreadImpl::join(double seconds)
{
    closing = true;
    if (needJoin) {
        if (seconds>0) {
            if (!initWasSuccessful) {
                // join called before start completed
                YARP_ERROR(Logger::get(), ConstString("Tried to join a thread before starting it"));
                return -1;
            }
            synchro.waitWithTimeout(seconds);
            if (active) {
                return -1;
            }
        }
        int result = PLATFORM_THREAD_JOIN(hid);
        needJoin = false;
        active = false;
        while (synchro.check()) {}
        return result;
    }
    return 0;
}

void ThreadImpl::run()
{
    if (delegate!=nullptr) {
        delegate->run();
    }
}

void ThreadImpl::close()
{
    closing = true;
    if (delegate!=nullptr) {
        delegate->close();
    }
    join(-1);
}

// similar to close(), but does not join (does not block)
void ThreadImpl::askToClose()
{
    closing = true;
    if (delegate!=nullptr) {
        delegate->close();
    }
}

void ThreadImpl::beforeStart()
{
    if (delegate!=nullptr) {
        delegate->beforeStart();
    }
}

void ThreadImpl::afterStart(bool success)
{
    if (delegate!=nullptr) {
        delegate->afterStart(success);
    }
}

bool ThreadImpl::threadInit()
{
    if (delegate!=nullptr) {
        return delegate->threadInit();
    }
    else
        return true;
}

void ThreadImpl::threadRelease()
{
    if (delegate!=nullptr){
        delegate->threadRelease();
    }
}

bool ThreadImpl::start()
{
    join();
    closing = false;
    initWasSuccessful = false;
    beforeStart();
#if defined(YARP_HAS_CXX11)
    hid = std::thread(theExecutiveBranch, (void*)this);
    int result = hid.joinable() ? 0 : 1;
#elif defined(YARP_HAS_ACE)
    size_t s = stackSize;
    if (s==0) {
        s = (size_t)defaultStackSize;
    }
    // c++11 std::thread, pthread and ace threads on some platforms do not
    // return the thread id, therefore we set it in theExecutiveBranch() for all
    // platforms. We don't set id here and use a dummy instead, since setting it
    // in both places could cause a race condition.
    ACE_thread_t dummy_id;
    int result = ACE_Thread::spawn((ACE_THR_FUNC)theExecutiveBranch,
                                   (void *)this,
                                   THR_JOINABLE | THR_NEW_LWP,
                                   &dummy_id,
                                   &hid,
                                   ACE_DEFAULT_THREAD_PRIORITY,
                                   nullptr,
                                   s);
#else
    pthread_attr_t attr;
    int s = pthread_attr_init(&attr);
    if (s != 0) {
        perror("pthread_attr_init");
    }

    if (stackSize > 0) {
        s = pthread_attr_setstacksize(&attr, stackSize);
        if (s != 0)
            perror("pthread_attr_setstacksize");
    }

    int result = pthread_create(&hid, &attr, theExecutiveBranch, (void*)this);
#endif
    if (result==0) {
        // we must, at some point in the future, join the thread
        needJoin = true;

        // the thread started correctly, wait for the initialization
        YARP_DEBUG(Logger::get(), ConstString("Child thread initializing"));
        synchroWait();
        initWasSuccessful = true;
        if (opened) {
            ThreadImpl::changeCount(1);
            YARP_DEBUG(Logger::get(), "Child thread initialized ok");
            afterStart(true);
            return true;
        } else {
            YARP_DEBUG(Logger::get(), "Child thread did not initialize ok");
            //wait for the thread to really exit
            ThreadImpl::join(-1);
        }
    }
    //the thread did not start, call afterStart() to warn the user
    char tmp[80];
    sprintf(tmp, "%d", result);
    YARP_ERROR(Logger::get(), ConstString("A thread failed to start with error code: ")+ConstString(tmp));
    afterStart(false);
    return false;
}

void ThreadImpl::synchroWait()
{
    synchro.wait();
}

void ThreadImpl::synchroPost()
{
    synchro.post();
}

void ThreadImpl::notify(bool s)
{
    threadMutex->wait();
    active=s;
    threadMutex->post();
}

bool ThreadImpl::isClosing()
{
    return closing;
}

bool ThreadImpl::isRunning()
{
    threadMutex->wait();
    bool b = active;
    threadMutex->post();
    return b;
}

int ThreadImpl::getCount()
{
    init();
    threadMutex->wait();
    int ct = threadCount;
    threadMutex->post();
    return ct;
}


void ThreadImpl::changeCount(int delta)
{
    init();
    threadMutex->wait();
    threadCount+=delta;
    threadMutex->post();
}

int ThreadImpl::setPriority(int priority, int policy)
{
    if (priority==-1) {
        priority = defaultPriority;
        policy = defaultPolicy;
    } else {
        defaultPriority = priority;
        defaultPolicy = policy;
    }
    if (active && priority!=-1) {

#if defined(YARP_HAS_CXX11)
#  if defined(YARP_HAS_ACE)
        if (std::is_same<std::thread::native_handle_type, ACE_hthread_t>::value) {
            return ACE_Thread::setprio(hid.native_handle(), priority, policy);
        } else {
            YARP_ERROR(Logger::get(), "Cannot set priority without ACE");
        }
#  elif defined(__unix__)
        if (std::is_same<std::thread::native_handle_type, pthread_t>::value) {
            struct sched_param thread_param;
            thread_param.sched_priority = priority;
            int ret = pthread_setschedparam(hid.native_handle(), policy, &thread_param);
            return (ret != 0) ? -1 : 0;
        } else {
            YARP_ERROR(Logger::get(), "Cannot set priority without ACE");
        }
#  else
        YARP_ERROR(Logger::get(), "Cannot set priority without ACE");
#  endif
#elif defined(YARP_HAS_ACE) // Use ACE API
        return ACE_Thread::setprio(hid, priority, policy);
#elif defined(__unix__) // Use the POSIX syscalls
        struct sched_param thread_param;
        thread_param.sched_priority = priority;
        int ret = pthread_setschedparam(hid, policy, &thread_param);
        return (ret != 0) ? -1 : 0;
#else
        YARP_ERROR(Logger::get(), "Cannot set priority without ACE");
#endif
    }
    return 0;
}

int ThreadImpl::getPriority()
{
    int prio = defaultPriority;
    if (active) {
#if defined(YARP_HAS_CXX11)
#  if defined(YARP_HAS_ACE)
        if (std::is_same<std::thread::native_handle_type, ACE_hthread_t>::value) {
            ACE_Thread::getprio(hid.native_handle(), prio);
        } else {
            YARP_ERROR(Logger::get(), "Cannot get priority without ACE");
        }
#  elif defined(__unix__)
        if (std::is_same<std::thread::native_handle_type, pthread_t>::value) {
            struct sched_param thread_param;
            int policy;
            if (pthread_getschedparam(hid.native_handle(), &policy, &thread_param) == 0) {
                prio = thread_param.sched_priority;
            }
        } else {
            YARP_ERROR(Logger::get(), "Cannot get priority without ACE");
        }
#  else
        YARP_ERROR(Logger::get(), "Cannot get priority without ACE");
#  endif
#elif defined(YARP_HAS_ACE) // Use ACE API
        ACE_Thread::getprio(hid, prio);
#elif defined(__unix__) // Use the POSIX syscalls
        struct sched_param thread_param;
        int policy;
        if (pthread_getschedparam(hid, &policy, &thread_param) == 0) {
            prio = thread_param.sched_priority;
        }
#else
        YARP_ERROR(Logger::get(), "Cannot read priority without ACE");
#endif
    }
    return prio;
}

int ThreadImpl::getPolicy()
{
    int policy = defaultPolicy;
    if (active) {
#if defined(YARP_HAS_CXX11)
#  if defined(YARP_HAS_ACE)
        if (std::is_same<std::thread::native_handle_type, ACE_hthread_t>::value) {
            int prio;
            ACE_Thread::getprio(hid.native_handle(), prio, policy);
        } else {
            YARP_ERROR(Logger::get(), "Cannot get scheduling policy without ACE");
        }
#  elif defined(__unix__)
        if (std::is_same<std::thread::native_handle_type, pthread_t>::value) {
            struct sched_param thread_param;
            if (pthread_getschedparam(hid.native_handle(), &policy, &thread_param) != 0) {
                policy = defaultPolicy;
            }
        } else {
            YARP_ERROR(Logger::get(), "Cannot get scheduling policy without ACE");
        }
#  else
        YARP_ERROR(Logger::get(), "Cannot get scheduling policy without ACE");
#  endif
#elif defined(YARP_HAS_ACE) // Use ACE API
        int prio;
        ACE_Thread::getprio(hid, prio, policy);
#elif defined(__unix__) // Use the POSIX syscalls
        struct sched_param thread_param;
        if (pthread_getschedparam(hid, &policy, &thread_param) != 0) {
            policy = defaultPolicy;
        }
#else
        YARP_ERROR(Logger::get(), "Cannot read scheduling policy without ACE");
#endif
    }
    return policy;
}

long ThreadImpl::getTid()
{
    return tid;
}


void ThreadImpl::setDefaultStackSize(int stackSize)
{
    defaultStackSize = stackSize;
}

void ThreadImpl::yield()
{
    std::this_thread::yield();
}
