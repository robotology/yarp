/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_CXX11SEMAPHOREIMPL_H
#define YARP_OS_IMPL_CXX11SEMAPHOREIMPL_H

#include <mutex>
#include <condition_variable>


class YARP_OS_impl_API yarp::os::impl::SemaphoreImpl
{
public:
    SemaphoreImpl(unsigned int initialCount = 1) :
            count(initialCount),
            wakeups(0)
    {
    }

    virtual ~SemaphoreImpl()
    {
    }

    // blocking wait
    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex);
        count--;
        if (count<0) {
            cond.wait(lock,
                      [this] { return wakeups > 0; });
            wakeups--;
        }
    }

    // blocking wait with timeout
    bool waitWithTimeout(double timeout)
    {
        std::unique_lock<std::mutex> lock(mutex);
        count--;
        if (count<0) {
#if defined _MSC_VER && _MSC_VER <= 1800
            std::chrono::nanoseconds ctime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(timeout));
#else
            std::chrono::duration<double> ctime(timeout);
#endif
            cond.wait_for(lock, ctime, [this] { return wakeups > 0; });

            if (wakeups<=0) {
                count++;
                return false;
            }
            wakeups--;
        }
        return true;
    }

    // polling wait
    bool check()
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (count) {
            count--;
            return true;
        }
        return false;
    }

    // increment
    void post()
    {
        std::lock_guard<std::mutex> lock(mutex);
        count++;
        if (count<=0) {
            wakeups++;
            cond.notify_one();
        }
    }

private:
    std::mutex mutex;
    std::condition_variable cond;
    int count;
    int wakeups;
};

#endif // YARP_OS_IMPL_CXX11SEMAPHOREIMPL_H
