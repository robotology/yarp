/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_SEMAPHOREIMPL_H
#define YARP_OS_IMPL_SEMAPHOREIMPL_H

#include <yarp/os/api.h>
#include <condition_variable>
#include <mutex>


namespace yarp {
namespace os {
namespace impl {

class YARP_OS_impl_API SemaphoreImpl
{
public:
    SemaphoreImpl(unsigned int initialCount = 1) :
            count(initialCount),
            wakeups(0)
    {
    }

    SemaphoreImpl(SemaphoreImpl&) = delete;
    SemaphoreImpl& operator=(SemaphoreImpl&) = delete;
    virtual ~SemaphoreImpl() = default;

    // blocking wait
    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex);
        count--;
        if (count < 0) {
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
        if (count < 0) {
            std::chrono::duration<double> ctime(timeout);
            cond.wait_for(lock, ctime, [this] { return wakeups > 0; });

            if (wakeups <= 0) {
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
        if (count <= 0) {
            wakeups++;
            cond.notify_one();
        }
    }

private:
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::mutex) mutex;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::condition_variable) cond;
    int count;
    int wakeups;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_SEMAPHOREIMPL_H
