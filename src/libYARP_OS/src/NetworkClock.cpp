/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/NetworkClock.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/SystemInfo.h>
#include <yarp/os/NestedContact.h>
#include <yarp/os/Os.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Network.h>
#include <yarp/conf/system.h>
#include <list>
#include <utility>
#include <string.h>


using namespace yarp::os;
using namespace yarp::os::impl;

typedef std::list<std::pair<double, Semaphore*> > Waiters;

NetworkClock::NetworkClock()
    : clockName(""), pwaiters(nullptr), sec(0), nsec(0), _time(0), closing(false), initted(false)
{
    pwaiters = new Waiters();
    if(pwaiters==nullptr)
    {
        YARP_FAIL(Logger::get(), "NetworkClock: new Waiters() failed");
    }
}

NetworkClock::~NetworkClock() {
    YARP_WARN(Logger::get(), "Destroying network clock");

    listMutex.lock();
    closing = true;
    port.interrupt();

    Waiters* waiters = static_cast<Waiters*>(pwaiters);
    if (waiters) {
        Waiters::iterator waiter_i;
        waiter_i = waiters->begin();
        while (waiter_i != waiters->end())
        {
            Semaphore *waiterSemaphore = waiter_i->second;
            waiter_i = waiters->erase(waiter_i);
            if (waiterSemaphore)
                waiterSemaphore->post();
        }

        delete waiters;
        pwaiters = nullptr;
    }

    listMutex.unlock();
    yarp::os::ContactStyle style;
    style.persistent = true;
    NetworkBase::disconnect(clockName, port.getName(), style);
}


bool NetworkClock::open(const ConstString& clockSourcePortName, ConstString localPortName)
{
    port.setReadOnly();
    port.setReader(*this);
    NestedContact nc(clockSourcePortName);
    clockName = clockSourcePortName;
    yarp::os::ContactStyle style;
    style.persistent = true;

    if(localPortName == "")
    {
        const int MAX_STRING_SIZE = 255;
        char hostName [MAX_STRING_SIZE];
        yarp::os::gethostname(hostName, MAX_STRING_SIZE);
        hostName[strlen(hostName)] = '\0';      // manually set the null terminator, so it is ok in any case

        yarp::os::SystemInfo::ProcessInfo processInfo = yarp::os::SystemInfo::getProcessInfo();

        localPortName = "/";
        // Ports may be anonymous to not pollute the yarp name list
        localPortName += ConstString(hostName) + "/" + processInfo.name + "/" + ConstString(std::to_string(processInfo.pid)) + "/clock:i";
    }

    // if receiving port cannot be opened, return false.
    bool ret = port.open(localPortName);
    if (!ret)
        return false;

    if (nc.getNestedName()=="")
    {
        Contact src = NetworkBase::queryName(clockSourcePortName);


        ret = NetworkBase::connect(clockSourcePortName, port.getName(), style);

        if(!src.isValid())
            fprintf(stderr,"Cannot find time port \"%s\"; for a time topic specify \"%s@\"\n", clockSourcePortName.c_str(), clockSourcePortName.c_str());
    }

    return ret;
}

double NetworkClock::now() {
    timeMutex.lock();
    double result = _time;
    timeMutex.unlock();
    return result;
}

void NetworkClock::delay(double seconds) {
    if (seconds<=1E-12) {
        return;
    }

    listMutex.lock();
    if (closing) {
        // We are shutting down.  The time signal is no longer available.
        // Make a short delay and return.
        listMutex.unlock();
        SystemClock::delaySystem(seconds);
        return;
    }

    Waiters* waiters = static_cast<Waiters*>(pwaiters);

    std::pair<double, Semaphore*> waiter(now() + seconds, new Semaphore(0));
    waiters->insert(waiters->end(), waiter);
    listMutex.unlock();

    waiter.second->wait();
    delete waiter.second;
    waiter.second = nullptr;
}

bool NetworkClock::isValid() const {
    return initted;
}

bool NetworkClock::read(ConnectionReader& reader) {
    Bottle bot;
    bool ok = bot.read(reader);

    if(closing)
    {
        _time = -1;
        return false;
    }

    if (!ok && !closing)
    {
        YARP_ERROR(Logger::get(), "Error reading clock port");
        return false;
    }

    timeMutex.lock();
    sec = bot.get(0).asInt();
    nsec = bot.get(1).asInt();
    _time = sec + (nsec*1e-9);
    initted = true;
    timeMutex.unlock();

    listMutex.lock();
    Waiters* waiters = static_cast<Waiters*>(pwaiters);
    Waiters::iterator waiter_i;

    waiter_i = waiters->begin();
    while (waiter_i != waiters->end())
    {
        if (waiter_i->first - _time < 1E-12 )
        {
            Semaphore *waiterSemaphore = waiter_i->second;
            waiter_i = waiters->erase(waiter_i);
            if (waiterSemaphore)
                waiterSemaphore->post();
        }
        else
            ++waiter_i;
    }
    listMutex.unlock();
    return true;
}
