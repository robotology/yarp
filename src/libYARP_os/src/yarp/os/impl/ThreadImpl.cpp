/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/ThreadImpl.h>

#include <yarp/os/NetType.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/PlatformSignal.h>

#include <cstdlib>
#include <sstream>
#include <thread>

#if defined(YARP_HAS_ACE)
#    include <ace/Thread.h> // For using ACE_hthread_t as native_handle
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#endif

#if defined(__linux__) // Use the POSIX syscalls for the gettid()
#    include <sys/syscall.h>
#    include <unistd.h>
#endif


using namespace yarp::os::impl;

namespace {
YARP_OS_LOG_COMPONENT(THREADIMPL, "yarp.os.impl.ThreadImpl")
} // namespace

static std::atomic<int> threadCount{0};

void theExecutiveBranch(void* args)
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

    auto* thread = (ThreadImpl*)args;

    yCDebug(THREADIMPL, "Thread starting up");

    bool success = thread->threadInit();
    thread->notify(success);
    thread->notifyOpened(success);
    thread->synchroPost();

    if (success) {
        // the thread id must be set before calling run() to avoid a race
        // condition in case the run() method checks it.
        thread->id = std::this_thread::get_id();
#if defined(__linux__)
        // Use the POSIX syscalls to get
        // the real thread ID (gettid) on Linux machine
        thread->tid = static_cast<long int>(syscall(SYS_gettid));
#else
        thread->tid = static_cast<long int>(std::hash<std::thread::id>()(thread->id));
#endif

        thread->setPriority();
        thread->run();
        thread->threadRelease();
    }

    --threadCount;
    yCDebug(THREADIMPL, "Thread shutting down");

    thread->notify(false);
    thread->synchroPost();

    return;
}


ThreadImpl::~ThreadImpl()
{
    yCDebug(THREADIMPL, "Thread being deleted");
    join();
}


long int ThreadImpl::getKey()
{
    return tid;
}


long int ThreadImpl::getKeyOfCaller()
{
#if defined(__linux__)
    // Use the POSIX syscalls to get
    // the real thread ID (gettid) on Linux machine
    return static_cast<long int>(syscall(SYS_gettid));
#else
    return static_cast<long int>(std::hash<std::thread::id>()(std::this_thread::get_id()));
#endif
}


int ThreadImpl::join(double seconds)
{
    closing = true;
    if (needJoin) {
        if (seconds > 0) {
            if (!initWasSuccessful) {
                // join called before start completed
                yCError(THREADIMPL, "Tried to join a thread before starting it");
                return -1;
            }
            synchro.waitWithTimeout(seconds);
            if (active) {
                return -1;
            }
        }

        int result = -1;
        if (thread.joinable()) {
            thread.join();
            result = 0;
        }

        needJoin = false;
        active = false;
        while (synchro.check()) {
        }
        return result;
    }
    return 0;
}

void ThreadImpl::run()
{
}

void ThreadImpl::close()
{
    closing = true;
    join(-1);
}

// similar to close(), but does not join (does not block)
void ThreadImpl::askToClose()
{
    closing = true;
}

void ThreadImpl::beforeStart()
{
}

void ThreadImpl::afterStart(bool success)
{
}

bool ThreadImpl::threadInit()
{
    return true;
}

void ThreadImpl::threadRelease()
{
}

bool ThreadImpl::start()
{
    join();
    closing = false;
    initWasSuccessful = false;
    beforeStart();
    thread = std::thread(theExecutiveBranch, (void*)this);
    int result = thread.joinable() ? 0 : 1;
    if (result == 0) {
        // we must, at some point in the future, join the thread
        needJoin = true;

        // the thread started correctly, wait for the initialization
        yCDebug(THREADIMPL, "Child thread initializing");
        synchroWait();
        initWasSuccessful = true;
        if (opened) {
            ++threadCount;
            yCDebug(THREADIMPL, "Child thread initialized ok");
            afterStart(true);
            return true;
        }
        yCDebug(THREADIMPL, "Child thread did not initialize ok");
        //wait for the thread to really exit
        ThreadImpl::join(-1);
    }
    //the thread did not start, call afterStart() to warn the user
    yCError(THREADIMPL, "A thread failed to start with error code: %d", result);
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
    active = s;
}

bool ThreadImpl::isClosing()
{
    return closing;
}

bool ThreadImpl::isRunning()
{
    return active;
}

int ThreadImpl::getCount()
{
    return threadCount;
}

int ThreadImpl::setPriority(int priority, int policy)
{
    if (priority == -1) {
        priority = defaultPriority;
        policy = defaultPolicy;
    } else {
        defaultPriority = priority;
        defaultPolicy = policy;
    }
    if (active && priority != -1) {
#if defined(YARP_HAS_ACE)
        if (std::is_same<std::thread::native_handle_type, ACE_hthread_t>::value) {
            return ACE_Thread::setprio(thread.native_handle(), priority, policy);
        }
        yCError(THREADIMPL, "Cannot set priority without ACE");
#elif defined(__unix__)
        if (std::is_same<std::thread::native_handle_type, pthread_t>::value) {
            struct sched_param thread_param;
            thread_param.sched_priority = priority;
            int ret = pthread_setschedparam(thread.native_handle(), policy, &thread_param);
            return (ret != 0) ? -1 : 0;
        } else {
            yCError(THREADIMPL, "Cannot set priority without ACE");
        }
#else
        yCError(THREADIMPL, "Cannot set priority without ACE");
#endif
    }
    return 0;
}

int ThreadImpl::getPriority()
{
    int prio = defaultPriority;
    if (active) {
#if defined(YARP_HAS_ACE)
        if (std::is_same<std::thread::native_handle_type, ACE_hthread_t>::value) {
            ACE_Thread::getprio(thread.native_handle(), prio);
        } else {
            yCError(THREADIMPL, "Cannot get priority without ACE");
        }
#elif defined(__unix__)
        if (std::is_same<std::thread::native_handle_type, pthread_t>::value) {
            struct sched_param thread_param;
            int policy;
            if (pthread_getschedparam(thread.native_handle(), &policy, &thread_param) == 0) {
                prio = thread_param.sched_priority;
            } else {
                yCError(THREADIMPL, "Cannot get priority without ACE");
            }
        }
#else
        yCError(THREADIMPL, "Cannot get priority without ACE");
#endif
    }
    return prio;
}

int ThreadImpl::getPolicy()
{
    int policy = defaultPolicy;
    if (active) {
#if defined(YARP_HAS_ACE)
        if (std::is_same<std::thread::native_handle_type, ACE_hthread_t>::value) {
            int prio;
            ACE_Thread::getprio(thread.native_handle(), prio, policy);
        } else {
            yCError(THREADIMPL, "Cannot get scheduling policy without ACE");
        }
#elif defined(__unix__)
        if (std::is_same<std::thread::native_handle_type, pthread_t>::value) {
            struct sched_param thread_param;
            if (pthread_getschedparam(thread.native_handle(), &policy, &thread_param) != 0) {
                policy = defaultPolicy;
            }
        } else {
            yCError(THREADIMPL, "Cannot get scheduling policy without ACE");
        }
#else
        yCError(THREADIMPL, "Cannot get scheduling policy without ACE");
#endif
    }
    return policy;
}

long ThreadImpl::getTid()
{
    return tid;
}

void ThreadImpl::yield()
{
    std::this_thread::yield();
}
