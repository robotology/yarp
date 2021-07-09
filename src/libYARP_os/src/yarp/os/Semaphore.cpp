/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Semaphore.h>

#include <condition_variable>
#include <mutex>

using yarp::os::Semaphore;

class Semaphore::Private
{
public:
    Private(unsigned int initialCount = 1) :
            count(initialCount)
    {
    }

    Private(Private&) = delete;
    Private& operator=(Private&) = delete;
    virtual ~Private() = default;

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
        if (count != 0) {
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
    std::mutex mutex;
    std::condition_variable cond;
    int count;
    int wakeups{0};
};


Semaphore::Semaphore(unsigned int initialCount) :
        mPriv(new Private(initialCount))
{
}

Semaphore::~Semaphore()
{
    delete mPriv;
}

void Semaphore::wait()
{
    mPriv->wait();
}

bool Semaphore::waitWithTimeout(double timeoutInSeconds)
{
    return mPriv->waitWithTimeout(timeoutInSeconds);
}

bool Semaphore::check()
{
    return mPriv->check();
}

void Semaphore::post()
{
    mPriv->post();
}
