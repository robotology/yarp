// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick, Miguel Sarabia del Castillo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_ACESEMAPHOREIMPL_
#define _YARP2_ACESEMAPHOREIMPL_

#include <ace/Synch.h>

#include <yarp/os/impl/String.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/PlatformTime.h>

#ifdef ACE_WIN32
// for WIN32 MM functions
#include <mmsystem.h>
#endif

class YARP_OS_impl_API yarp::os::impl::SemaphoreImpl {
public:
    SemaphoreImpl(unsigned int initialCount = 1)
        : sema(initialCount)
    {
    }

    virtual ~SemaphoreImpl() {
    }

    // blocking wait
    void wait() {
        int result = sema.acquire();
        if (result!=-1) return;
        int ct = 100;
        while (result == -1 && ct>=0) {
            YARP_ERROR(Logger::get(), yarp::os::impl::String("semaphore wait failed (errno ") + (yarp::os::NetType::toString(ACE_OS::last_error())) + yarp::os::impl::String("); gdb problem, or bad YARP+ACE flags"));
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
    bool waitWithTimeout(double timeout) {
        ACE_Time_Value ts = ACE_OS::gettimeofday();
        ACE_Time_Value add;
        add.sec(long(timeout));
        add.usec(long((timeout-long(timeout)) * 1.0e6));
        ts += add;
        int result = sema.acquire(ts);
        return (result!=-1);
    }

    // polling wait
    bool check() {
        return (sema.tryacquire()<0)?0:1;
    }

    // increment
    void post() {
        sema.release();
    }

private:
    ACE_Semaphore sema;
};

#endif

