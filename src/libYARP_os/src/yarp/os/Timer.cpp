/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Timer.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Time.h>

#include <cmath>
#include <map>
#include <mutex>
#include <utility>

using namespace yarp::os;

//disclaimer: the following inheritance little madness is for avoiding critical copy and paste code and
//avoiding data inconsistence(example: RateThread::GetIterations() and runTimes)

class yarp::os::Timer::PrivateImpl
{
protected:
    yarp::os::YarpTimerEvent getEventNow(unsigned int iteration);

    bool runTimer(unsigned int iteration, YarpTimerEvent event);

public:
    using TimerCallback = yarp::os::Timer::TimerCallback;

#ifndef YARP_NO_DEPRECATED // since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    PrivateImpl(const TimerSettings& sett,
                TimerCallback call,
                yarp::os::Mutex* mutex) :
            m_settings(sett),
            m_callback(std::move(call)),
            m_old_mutex(mutex)
    {
    }
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED

    PrivateImpl(const TimerSettings& sett,
                TimerCallback call,
                std::mutex* mutex = nullptr) :
            m_settings(sett),
            m_callback(std::move(call)),
            m_mutex(mutex)
    {
    }

    virtual ~PrivateImpl() = default;

    virtual bool startTimer() = 0;

    virtual void stopTimer() = 0;

    virtual bool stepTimer() = 0;

    virtual bool timerIsRunning() = 0;


    TimerSettings m_settings;
    TimerCallback m_callback;
    double m_startStamp{0.0};
    double m_lastReal{0.0};
    std::mutex* m_mutex{nullptr};
#ifndef YARP_NO_DEPRECATED // since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    yarp::os::Mutex* m_old_mutex{nullptr};
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED
};

class MonoThreadTimer : public yarp::os::Timer::PrivateImpl
{
public:

#ifndef YARP_NO_DEPRECATED // since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    MonoThreadTimer(const TimerSettings& sett,
                    const TimerCallback& call,
                    yarp::os::Mutex* mutex);
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED

    MonoThreadTimer(const TimerSettings& sett,
                    const TimerCallback& call,
                    std::mutex* mutex = nullptr);

    ~MonoThreadTimer() override;
    bool m_active{false};
    unsigned int m_runTimes{1};
    size_t m_id{(size_t)-1};

    virtual yarp::os::YarpTimerEvent getEventNow()
    {
        return PrivateImpl::getEventNow(m_runTimes);
    }

    bool startTimer() override
    {
        m_startStamp = yarp::os::Time::now();
        m_active = true;
        return true;
    }

    void stopTimer() override
    {
        m_active = false;
    }

    bool stepTimer() override
    {
        return step(getEventNow(), true);
    }

    virtual bool step(YarpTimerEvent event, bool singleStep)
    {
        bool m_active = runTimer(m_runTimes, event);
        if (!singleStep) {
            m_runTimes++;
        }
        return m_active;
    }

    bool timerIsRunning() override
    {
        return m_active;
    }
};

double gcd(double a, double b)
{
    if (a < b) {
        return gcd(b, a);
    }

    // base case
    if (fabs(b) < 0.001) {
        return a;
    } else {
        return (gcd(b, a - floor(a / b) * b));
    }
}

class TimerSingleton : public yarp::os::PeriodicThread
{
    std::mutex mu;
    std::map<size_t, MonoThreadTimer*> timers;
    TimerSingleton() :
            PeriodicThread(10)
    {
    }

    void run() override;

    ~TimerSingleton() override
    {
        stop();
    }

public:
    //reminder: int c++11 static variables'inside function are guaranteed to be lazy initialized and atomic
    static TimerSingleton& self()
    {
        static TimerSingleton instance;
        return instance;
    }

    size_t addTimer(MonoThreadTimer* t)
    {
        if (timers.size()) {
            setPeriod(gcd(getPeriod(), t->m_settings.period));
        } else {
            setPeriod(t->m_settings.period);
        }
        mu.lock();
        timers[timers.size()] = t;
        mu.unlock();

        return timers.size() - 1;
    }

    void removeTimer(size_t id)
    {
        mu.lock();
        timers.erase(id);
        mu.unlock();
        if (!timers.size()) {
            return;
        }

        double new_gcd = timers.begin()->second->m_settings.period;
        for (auto& i : timers) {
            new_gcd = gcd(new_gcd, i.second->m_settings.period);
        }
        setPeriod(new_gcd);
    }

    size_t getTimerCount()
    {
        return timers.size();
    }
};

#ifndef YARP_NO_DEPRECATED // since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
MonoThreadTimer::MonoThreadTimer(const TimerSettings& sett,
                                 const TimerCallback& call,
                                 yarp::os::Mutex* mutex) :
        PrivateImpl(sett, call, mutex)
{
    TimerSingleton& singlInstance = TimerSingleton::self();
    m_id = singlInstance.addTimer(this);
    if (!singlInstance.isRunning()) {
        singlInstance.start();
    }
}
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED

MonoThreadTimer::MonoThreadTimer(const TimerSettings& sett,
                                 const TimerCallback& call,
                                 std::mutex* mutex) :
        PrivateImpl(sett, call, mutex)
{
    TimerSingleton& singlInstance = TimerSingleton::self();
    m_id = singlInstance.addTimer(this);
    if (!singlInstance.isRunning()) {
        singlInstance.start();
    }
}

MonoThreadTimer::~MonoThreadTimer()
{
    TimerSingleton& singlInstance = TimerSingleton::self();
    singlInstance.removeTimer(m_id);
    if (singlInstance.getTimerCount() == 0) {
        singlInstance.stop();
    }
}

void TimerSingleton::run()
{
    mu.lock();
    for (auto t : timers) {
        MonoThreadTimer& timer = *t.second;
        YarpTimerEvent tEvent = timer.getEventNow();
        if (timer.m_active && tEvent.currentReal > tEvent.currentExpected) {
            timer.m_active = timer.step(tEvent, false);
            timer.m_lastReal = tEvent.currentReal;
        }
    }
    mu.unlock();
}

class ThreadedTimer :
        public yarp::os::Timer::PrivateImpl,
        public yarp::os::PeriodicThread
{
    using TimerCallback = yarp::os::Timer::TimerCallback;
    void run() override;
    bool threadInit() override;
    bool singleStep{false};

public:
#ifndef YARP_NO_DEPRECATED // since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    ThreadedTimer(const TimerSettings& sett,
                  const TimerCallback& call,
                  yarp::os::Mutex* mutex) :
            PrivateImpl(sett, call, mutex),
            PeriodicThread(sett.period)
    {
    }
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED

    ThreadedTimer(const TimerSettings& sett,
                  const TimerCallback& call,
                  std::mutex* mutex = nullptr) :
            PrivateImpl(sett, call, mutex),
            PeriodicThread(sett.period)
    {
    }

    ~ThreadedTimer() override
    {
        stop();
    }

    bool startTimer() override
    {
        setPeriod(m_settings.period);
        m_startStamp = yarp::os::Time::now();
        return start();
    }

    bool stepTimer() override
    {
        singleStep = true;
        step();
        return true;
    }

    void stopTimer() override
    {
        return askToStop();
    }

    bool timerIsRunning() override
    {
        return isRunning();
    }
};

bool ThreadedTimer::threadInit()
{
    m_startStamp = yarp::os::Time::now();
    return true;
}

#ifndef YARP_NO_DEPRECATED // since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
//the newThread parameter is not in the settings for it to be unmutable and only checked by the constructor
Timer::Timer(const TimerSettings& settings, const TimerCallback& callback, bool newThread, Mutex* mutex) :
        //added cast for incompatible operand error
        impl(newThread ? dynamic_cast<PrivateImpl*>(new ThreadedTimer(settings, callback, mutex))
                       : dynamic_cast<PrivateImpl*>(new MonoThreadTimer(settings, callback, mutex)))
{
}
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED

//the newThread parameter is not in the settings for it to be unmutable and only checked by the constructor
Timer::Timer(const TimerSettings& settings, const TimerCallback& callback, bool newThread, std::mutex* mutex) :
        //added cast for incompatible operand error
        impl(newThread ? dynamic_cast<PrivateImpl*>(new ThreadedTimer(settings, callback, mutex))
                       : dynamic_cast<PrivateImpl*>(new MonoThreadTimer(settings, callback, mutex)))
{
}

bool Timer::start()
{

    return impl->startTimer();
}

bool Timer::step()
{
    return impl->stepTimer();
}

void Timer::stop()
{
    impl->stopTimer();
}

YarpTimerEvent yarp::os::Timer::PrivateImpl::getEventNow(unsigned int iteration)
{
    YarpTimerEvent event;

    event.currentReal = yarp::os::Time::now();
    event.currentExpected = m_startStamp + iteration * m_settings.period;
    event.lastExpected = event.currentExpected - m_settings.period;
    event.lastReal = m_lastReal;
    event.lastDuration = event.currentReal - m_lastReal;
    event.runCount = iteration;
    return event;
}

bool yarp::os::Timer::PrivateImpl::runTimer(unsigned int iteration, YarpTimerEvent event)
{
    if (m_mutex != nullptr) {
        m_mutex->lock();
    }

#ifndef YARP_NO_DEPRECATED // since YARP 3.3
    if (m_old_mutex != nullptr) {
        m_old_mutex->lock();
    }
#endif // YARP_NO_DEPRECATED

    bool ret = m_callback(event);

#ifndef YARP_NO_DEPRECATED // since YARP 3.3
    if (m_old_mutex != nullptr) {
        m_old_mutex->unlock();
    }
#endif // YARP_NO_DEPRECATED

    if (m_mutex != nullptr) {
        m_mutex->unlock();
    }

    if (!ret) {
        return false;
    }

    m_lastReal = event.currentReal;

    double timerAge = (yarp::os::Time::now() - m_startStamp);

    //totalRunCount == 0 ----> infinite run count. follows the run count of the timer
    bool stop(m_settings.totalRunCount != 0 && m_settings.totalRunCount <= iteration);

    //totalTime == 0 ----> infinite time. follows the age check for the timer
    stop |= m_settings.totalTime > 0.00001 && (m_settings.totalTime - timerAge) < m_settings.tolerance;

    return !stop;
}

void ThreadedTimer::run()
{
    if (getIterations() == 0 && !singleStep) {
        return;
    }
    singleStep = false;
    YarpTimerEvent event = getEventNow(this->getIterations());
    if (!runTimer(this->getIterations(), event)) {
        askToStop();
    }
}

void Timer::setSettings(const TimerSettings& settings)
{
    impl->m_settings = settings;
}

const TimerSettings Timer::getSettings()
{
    return impl->m_settings;
}

bool Timer::isRunning()
{
    return impl->timerIsRunning();
}

Timer::~Timer()
{
    delete impl;
}
