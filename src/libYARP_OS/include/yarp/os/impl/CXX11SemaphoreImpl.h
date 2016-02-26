/*
 * Copyright (C) 2006, 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick, Miguel Sarabia del Castillo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_CXX11SEMAPHOREIMPL
#define YARP2_CXX11SEMAPHOREIMPL

#include <mutex>
#include <condition_variable>


class YARP_OS_impl_API yarp::os::impl::SemaphoreImpl {
public:
    SemaphoreImpl(unsigned int initialCount = 1) : count(initialCount),
        wakeups(0) {
    }

    virtual ~SemaphoreImpl() {
    }

    // blocking wait
    void wait() {
        std::unique_lock<std::mutex> lock(mutex);
        count--;
        if (count<0) {
            cond.wait(lock,
                      [this] { return wakeups > 0; });
            wakeups--;
        }
    }

    // blocking wait with timeout
    bool waitWithTimeout(double timeout) {
        std::unique_lock<std::mutex> lock(mutex);
        count--;
        if (count<0) {
            auto target = std::chrono::system_clock::now() +
                std::chrono::duration<double>(timeout);
            cond.wait_until(lock,
                            target,
                            [this] { return wakeups > 0; });
            if (wakeups<=0) {
                count++;
                return false;
            }
            wakeups--;
        }
        return true;
    }

    // polling wait
    bool check() {
        std::unique_lock<std::mutex> lock(mutex);
        if (count) {
            count--;
            return true;
        }
        return false;
    }

    // increment
    void post() {
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

#endif

