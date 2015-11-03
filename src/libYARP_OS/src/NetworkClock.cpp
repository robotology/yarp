// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/NetworkClock.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/NestedContact.h>
#include <yarp/os/Network.h>
#include <yarp/os/Log.h>
#include <list>
#include <utility>

using namespace yarp::os;

typedef std::list<std::pair<double, Semaphore*> > Waiters;

NetworkClock::NetworkClock()
: pwaiters(0), sec(0), nsec(0), t(0), closing(false){
    pwaiters = new Waiters();
    yAssert(pwaiters!=NULL);
}

NetworkClock::~NetworkClock() {
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
        listMutex.unlock();

        delete waiters;
        pwaiters = NULL;
    }
    else {
        listMutex.unlock();
    }
}


bool NetworkClock::open(const ConstString& name) {
    port.setReadOnly();
    port.setReader(*this);
    NestedContact nc(name);
    if (nc.getNestedName()=="") {
        Contact src = NetworkBase::queryName(name);
        if (src.isValid()) {
            bool ok = port.open("");
            if (!ok) return false;
            return NetworkBase::connect(name,port.getName());
        } else {
            fprintf(stderr,"Cannot find time port \"%s\"; for a time topic specify \"%s@\"\n", name.c_str(), name.c_str());
            return false;
        }
    }
    return port.open(name);
}

double NetworkClock::now() {
    timeMutex.lock();
    double result = t;
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
    if (waiter.second) {
        delete waiter.second;
        waiter.second = 0;
    }
}

bool NetworkClock::isValid() const {
    return (sec!=0) || (nsec!=0);
}

bool NetworkClock::read(ConnectionReader& reader) {
    Bottle bot;
    bool ok = bot.read(reader);
    if(!ok) return false;

    timeMutex.lock();
    sec = bot.get(0).asInt();
    nsec = bot.get(1).asInt();
    t = sec + (nsec*1e-9);
    timeMutex.unlock();

    listMutex.lock();
    Waiters* waiters = static_cast<Waiters*>(pwaiters);
    Waiters::iterator waiter_i;

    waiter_i = waiters->begin();
    while (waiter_i != waiters->end())
    {
        if (waiter_i->first - t  < 1E-12 )
        {
            Semaphore *waiterSemaphore = waiter_i->second;
            waiter_i = waiters->erase(waiter_i);
            if (waiterSemaphore)
                waiterSemaphore->post();
        }
        else
            waiter_i++;
    }
    listMutex.unlock();
    return true;
}

