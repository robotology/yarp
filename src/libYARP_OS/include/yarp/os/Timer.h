/*
* Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
* All rights reserved.
*
* This software may be modified and distributed under the terms of the
* BSD-3-Clause license. See the accompanying LICENSE file for details.
*/

#include <yarp/os/RateThread.h>
#include <yarp/os/Mutex.h>
#include <functional>
#include <chrono>

namespace yarp {
    namespace os {
        class Timer;
        struct YarpTimerEvent;
        struct TimerSettings;
    }
}

struct yarp::os::YarpTimerEvent
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
     * @brief currentExpected this is when the current callback should have been called
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

struct yarp::os::TimerSettings
{
    TimerSettings(std::chrono::milliseconds inRate) : rate(inRate), totalTime(0.0), totalRunCount(0), tollerance(0.001){}
    TimerSettings(std::chrono::milliseconds inRate, size_t count, double seconds) : rate(inRate), totalTime(seconds), totalRunCount(count), tollerance(0.001){}
    TimerSettings(std::chrono::milliseconds inRate, size_t count, double seconds, double inTollerance)
        : rate(inRate),
          totalTime(seconds),
          totalRunCount(count),
          tollerance(inTollerance){}

    std::chrono::milliseconds rate;
    double                    totalTime;
    size_t                    totalRunCount;
    double                    tollerance;
};

class YARP_OS_API yarp::os::Timer : public yarp::os::RateThread {
    typedef std::function<bool(const yarp::os::YarpTimerEvent&)> TimerCallback;
    class Private;

    Private* impl;

public:

    /**
     * @brief Timer constructor
     * @param rate the call frequency, in milliseconds
     * @param callback the pointer to the function to call
     * @param mutex if not nullptr will be locked before calling callback and released just after
     * @param type sets the timer type, if ONE_SHOT will be called only once. PERIODIC will run periodically
     */
    Timer(const yarp::os::TimerSettings& settings, TimerCallback callback, yarp::os::Mutex* mutex = nullptr);

    /**
     * @brief Timer constructor
     * @param rate the call frequency, in milliseconds
     * @param callback the pointer to the member method to call
     * @param object the pointer to the object
     * @param mutex if not nullptr will be locked before calling callback and released just after
     * @param oneshot if true there will be only one call
     */
    template<class T>
    Timer(const  yarp::os::TimerSettings& settings,
          bool(T::*callback)(const yarp::os::YarpTimerEvent&),
          T* object,
          yarp::os::Mutex* mutex = nullptr) :
        Timer(settings, std::bind(callback, object, std::placeholders::_1), mutex) {}

    /**
     * const version.
     */
    template<class T>
    Timer(const  yarp::os::TimerSettings& settings,
          bool(T::*callback)(const yarp::os::YarpTimerEvent&) const,
          const T* object,
          yarp::os::Mutex* mutex = nullptr) :
        Timer(settings, std::bind(callback, object, std::placeholders::_1), mutex) {}

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

protected:
    virtual void beforeStart() override;
    virtual void run() override;
};

