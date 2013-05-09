/*
 * Copyright (C) 2006, 2009 RobotCub Consortium
 * Authors: Miguel Sarabia del Castillo, Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/impl/SemaphoreImpl.h>


#if defined(__linux__)
//BEGIN LINUX IMPLEMENTATION
#  include <time.h>

yarp::os::impl::SemaphoreImpl::SemaphoreImpl(unsigned int initialCount) {
    sem_init(&sema,0,initialCount);
}

yarp::os::impl::SemaphoreImpl::~SemaphoreImpl() {
    sem_destroy(&sema);
}

void yarp::os::impl::SemaphoreImpl::wait() {
    sem_wait(&sema);
}

bool yarp::os::impl::SemaphoreImpl::waitWithTimeout(double timeout) {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) return false;
    ts.tv_sec = ts.tv_sec + (int)timeout;
    ts.tv_nsec = ts.tv_nsec +
                 (long)((timeout-(int)timeout)*1000000000L+0.5);
    if (ts.tv_nsec >= 1000000000L) {
        ts.tv_sec++;
        ts.tv_nsec = ts.tv_nsec - 1000000000L;
    }
    int result = sem_timedwait(&sema, &ts);
    return result == 0;
}

bool yarp::os::impl::SemaphoreImpl::check() {
    return sem_trywait(&sema)==0;
}

void yarp::os::impl::SemaphoreImpl::post() {
    sem_post(&sema);
}
//END LINUX IMPLEMENTATION


#elif defined(__APPLE__)
//BEGIN APPLE IMPLEMENTATION
#  include <mach/mach_init.h>
#  include <mach/task.h>
#  include <yarp/os/impl/Logger.h>

yarp::os::impl::SemaphoreImpl::SemaphoreImpl(unsigned int initialCount = 1) :
        sema(initialCount)
{
    int result = semaphore_create(mach_task_self(), &sema, SYNC_POLICY_FIFO, initialCount);
    YARP_ASSERT(result==KERN_SUCCESS);
}

yarp::os::impl::SemaphoreImpl::~SemaphoreImpl() {
    semaphore_destroy(mach_task_self(),sema);
}

void yarp::os::impl::SemaphoreImpl::wait() {
    semaphore_wait(sema);
}

bool yarp::os::impl::SemaphoreImpl::waitWithTimeout(double timeout) {
    mach_timespec_t ts = { 0, 0 };
    ts.tv_sec = ts.tv_sec + (int)timeout;
    ts.tv_nsec = (long)((timeout-(int)timeout)*1000000000L+0.5);
    return semaphore_timedwait(sema,ts) == KERN_SUCCESS;
}

bool yarp::os::impl::SemaphoreImpl::check() {
    mach_timespec_t timeout = { 0, 0 };
    return semaphore_timedwait(sema,timeout) == KERN_SUCCESS;
}

void yarp::os::impl::SemaphoreImpl::post() {
    semaphore_signal(sema);
}
//END APPLE IMPLEMENTATION


#elif defined(YARP_HAS_ACE)
//BEGIN ACE IMPLEMENTATION
#  include <yarp/os/impl/String.h>
#  include <yarp/os/impl/Logger.h>
#  include <yarp/os/impl/NetType.h>
#  include <yarp/os/impl/PlatformStdlib.h>
#  include <yarp/os/impl/PlatformTime.h>
yarp::os::impl::SemaphoreImpl::SemaphoreImpl(unsigned int initialCount) :
        sema(initialCount) {
}

yarp::os::impl::SemaphoreImpl::~SemaphoreImpl() {
}

void yarp::os::impl::SemaphoreImpl::wait() {
    int result = sema.acquire();
    if (result!=-1) return;
    int ct = 100;
    while (result == -1 && ct>=0) {
        YARP_ERROR(Logger::get(), yarp::os::impl::String("semaphore wait failed (errno ") + (yarp::os::impl::NetType::toString(ACE_OS::last_error())) + yarp::os::impl::String("); gdb problem, or bad YARP+ACE flags"));
        result = sema.acquire();
        ct--;
    }
    if (result==-1) {
        YARP_ERROR(Logger::get(), "semaphore wait failed");
    } else {
        YARP_ERROR(Logger::get(), "semaphore wait eventually succeeded");
    }
}

bool yarp::os::impl::SemaphoreImpl::waitWithTimeout(double timeout) {
    ACE_Time_Value ts = ACE_OS::gettimeofday();
    ACE_Time_Value add;
    add.sec(long(timeout));
    add.usec(long((timeout-long(timeout)) * 1.0e6));
    ts += add;
    int result = sema.acquire(ts);
    return (result!=-1);
}

bool yarp::os::impl::SemaphoreImpl::check() {
    return (sema.tryacquire()<0)?0:1;
}

void yarp::os::impl::SemaphoreImpl::post() {
    sema.release();
}
//END ACE IMPLEMENTATION


#endif
