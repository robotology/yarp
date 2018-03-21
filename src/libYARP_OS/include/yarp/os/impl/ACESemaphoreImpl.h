/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_ACESEMAPHOREIMPL_H
#define YARP_OS_IMPL_ACESEMAPHOREIMPL_H

#include <ace/Synch.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/NetType.h>
#include <cstdlib>
#include <yarp/os/impl/PlatformTime.h>

#ifdef ACE_WIN32
// for WIN32 MM functions
#include <mmsystem.h>
#endif

class YARP_OS_impl_API yarp::os::impl::SemaphoreImpl
{
public:
    SemaphoreImpl(unsigned int initialCount = 1)
        : sema(initialCount)
    {
    }

    virtual ~SemaphoreImpl()
    {
    }

    // blocking wait
    void wait()
    {
        int result = sema.acquire();
        if (result!=-1) {
            return;
        }
        int ct = 100;
        while (result == -1 && ct>=0) {
            YARP_ERROR(Logger::get(), yarp::os::ConstString("semaphore wait failed (errno ") + (yarp::os::NetType::toString(ACE_OS::last_error())) + yarp::os::ConstString("); gdb problem, or bad YARP+ACE flags"));
            result = sema.acquire();
            ct--;
        }
        if (result==-1) {
            YARP_ERROR(Logger::get(), "semaphore wait failed");
        } else {
            YARP_ERROR(Logger::get(), "semaphore wait eventually succeeded");
        }
    }

    // blocking wait with timeout
    bool waitWithTimeout(double timeout)
    {
        ACE_Time_Value ts = ACE_OS::gettimeofday();
        ACE_Time_Value add;
        add.sec(long(timeout));
        add.usec(long((timeout-long(timeout)) * 1.0e6));
        ts += add;
        int result = sema.acquire(ts);
        return (result!=-1);
    }

    // polling wait
    bool check()
    {
        return (sema.tryacquire()<0)?0:1;
    }

    // increment
    void post()
    {
        sema.release();
    }

private:
    ACE_Semaphore sema;
};

#endif // YARP_OS_IMPL_ACESEMAPHOREIMPL_H
