/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_TIMER_H
#define YARP_OS_TIMER_H

#include <yarp/os/api.h>

#ifndef YARP_NO_DEPRECATED // since YARP 3.3
#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#include <yarp/os/Mutex.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#endif

#include <functional>
#include <mutex>

namespace yarp {
namespace os {

struct YARP_os_API YarpTimerEvent
{
    /**
     * @brief lastExpected when the last callback actually happened
     */
    double lastExpected;

    /**
     * @brief lastReal when the last callback actually happened
     */
    double lastReal;

    /**
     * @brief currentExpected this is when the current callback should have been
     *        called
     */
    double currentExpected;

    /**
     * @brief currentReal When the current callback is actually being called
     */
    double currentReal;

    /**
     * @brief lastDuration Contains the duration of the last callback
     */
    double lastDuration;

    /**
     * @brief runCount the count of calls
     */
    unsigned int runCount;
};

struct YARP_os_API TimerSettings
{
    TimerSettings(double inPeriod) :
            period(inPeriod),
            totalTime(0.0),
            totalRunCount(0),
            tolerance(0.001)
    {
    }
    TimerSettings(double inPeriod, size_t count, double seconds) :
            period(inPeriod),
            totalTime(seconds),
            totalRunCount(count),
            tolerance(0.001)
    {
    }
    TimerSettings(double inPeriod, size_t count, double seconds, double inTollerance) :
            period(inPeriod),
            totalTime(seconds),
            totalRunCount(count),
            tolerance(inTollerance)
    {
    }

    bool operator==(const TimerSettings& rhs) const
    {
        return period == rhs.period && totalTime == rhs.totalTime && totalRunCount == rhs.totalRunCount && tolerance == rhs.tolerance;
    }


    /**
     * @param period the period of the timer in seconds
     * @param totalTime the life of the timer in seconds. 0 == infinite.
     *        The lower between totalTime and totalRunCount*rate + execution
     *        delay will stop the timer
     * @param totalRunCount the total count of execution. 0 == infinite.
     *        The lower between totalTime and totalRunCount*rate + execution
     *        delay will stop the timer
     * @param tolerance the tolerance while checking the timer life
     */
    double period;
    double totalTime;
    size_t totalRunCount;
    double tolerance;
};

class YARP_os_API Timer
{
public:
    typedef std::function<bool(const yarp::os::YarpTimerEvent&)> TimerCallback;
    Timer(const Timer&) = delete;
    Timer operator=(const Timer&) = delete;

#ifndef YARP_NO_DEPRECATED // Since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    /**
     * @brief Timer constructor
     * @param settings the timer settings. see TimerSettings documentation
     * @param callback the pointer to the function to call. the signature should
     *        be "bool foo(const yarp::os::YarpTimerEvent&)" and if it return
     *        false the timer will stop
     * @param mutex if not nullptr will be locked before calling callback and
     *        released just after
     * @param newThread whether the timer should be executed in a dedicated thread
     *        or with all the timers with newThread == false (in any case they
     *        will not run in the main thread)
     * @deprecated since YARP 3.3
     */
    Timer(const yarp::os::TimerSettings& settings,
          const TimerCallback& callback,
          bool newThread,
          yarp::os::Mutex* mutex);

    /**
     * @brief Timer constructor
     * @param settings the timer settings. see TimerSettings documentation
     * @param callback the pointer to the member method to call
     * @param object the pointer to the object
     * @param mutex if not nullptr will be locked before calling callback and
     *        released just after
     * @param newThread whether the timer should be executed in a his own thread
     *        or with all the timers with newThread == false (in any case they
     *        will not run in the main thread)
     * @deprecated since YARP 3.3
     */
    template <class T>
    Timer(const yarp::os::TimerSettings& settings,
          bool (T::*callback)(const yarp::os::YarpTimerEvent&),
          T* object,
          bool newThread,
          yarp::os::Mutex* mutex) :

            Timer(settings, std::bind(callback, object, std::placeholders::_1), newThread, mutex)
    {
    }

    /**
     * const version.
     * @deprecated since YARP 3.3
     */
    template <class T>
    Timer(const yarp::os::TimerSettings& settings,
          bool (T::*callback)(const yarp::os::YarpTimerEvent&) const,
          const T* object,
          bool newThread,
          yarp::os::Mutex* mutex) :

            Timer(settings, std::bind(callback, object, std::placeholders::_1), newThread, mutex)
    {
    }
YARP_WARNING_POP
#endif

    /**
     * @brief Timer constructor
     * @param settings the timer settings. see TimerSettings documentation
     * @param callback the pointer to the function to call. the signature should
     *        be "bool foo(const yarp::os::YarpTimerEvent&)" and if it return
     *        false the timer will stop
     * @param mutex if not nullptr will be locked before calling callback and
     *        released just after
     * @param newThread whether the timer should be executed in a dedicated thread
     *        or with all the timers with newThread == false (in any case they
     *        will not run in the main thread)
     */
    Timer(const yarp::os::TimerSettings& settings,
          const TimerCallback& callback,
          bool newThread,
          std::mutex* mutex = nullptr);

    /**
     * @brief Timer constructor
     * @param settings the timer settings. see TimerSettings documentation
     * @param callback the pointer to the member method to call
     * @param object the pointer to the object
     * @param mutex if not nullptr will be locked before calling callback and
     *        released just after
     * @param newThread whether the timer should be executed in a his own thread
     *        or with all the timers with newThread == false (in any case they
     *        will not run in the main thread)
     */
    template <class T>
    Timer(const yarp::os::TimerSettings& settings,
          bool (T::*callback)(const yarp::os::YarpTimerEvent&),
          T* object,
          bool newThread,
          std::mutex* mutex = nullptr) :

            Timer(settings, std::bind(callback, object, std::placeholders::_1), newThread, mutex)
    {
    }

    /**
     * const version.
     */
    template <class T>
    Timer(const yarp::os::TimerSettings& settings,
          bool (T::*callback)(const yarp::os::YarpTimerEvent&) const,
          const T* object,
          bool newThread,
          std::mutex* mutex = nullptr) :

            Timer(settings, std::bind(callback, object, std::placeholders::_1), newThread, mutex)
    {
    }

    virtual ~Timer();

    /**
     * @brief setSettings
     * @param settings the new settings
     */
    void setSettings(const yarp::os::TimerSettings& settings);

    /**
     * @brief getSettings
     * @return the current settings
     */
    const yarp::os::TimerSettings getSettings();

    virtual bool start();

    virtual bool step();

    virtual void stop();

    virtual bool isRunning();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    class PrivateImpl;

private:
    PrivateImpl* impl;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_TIMER_H
