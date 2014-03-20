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
#define WAITERS(x) (*((Waiters*)(x)))

NetworkClock::NetworkClock() {
    sec = 0;
    nsec = 0;
    t = 0;
    pwaiters = new Waiters();
    YARP_ASSERT(pwaiters!=NULL);
}

NetworkClock::~NetworkClock() {
    if (pwaiters) {
        delete &WAITERS(pwaiters);
        pwaiters = NULL;
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

    Waiters& waiters = WAITERS(pwaiters);
    Waiters::iterator waiterIterator;
    std::pair<double, Semaphore*> waiter;
    waiter.second = new Semaphore(0);
    
    listMutex.lock();
    waiter.first = now() + seconds;
    waiterIterator = waiters.insert(waiters.end(), waiter);
    listMutex.unlock();

    waiter.second->wait();

    listMutex.lock();
    delete(waiter.second);
    waiters.erase(waiterIterator);
    listMutex.unlock();

}

bool NetworkClock::isValid() const {
    return (sec!=0) || (nsec!=0);
}

bool NetworkClock::read(ConnectionReader& reader) {
    Bottle bot;
    bool ok = bot.read(reader);
    if(!ok) return false;

    listMutex.lock();

    timeMutex.lock();
    sec = bot.get(0).asInt();
    nsec = bot.get(1).asInt();
    t = sec + (nsec*1e-9);
    timeMutex.unlock();

    Waiters& waiters = WAITERS(pwaiters);
    Waiters::iterator waiter_i;

    for(waiter_i = waiters.begin(); waiter_i != waiters.end(); waiter_i++) {
        if(waiter_i->first - t  < 1E-12 ) // t - waiter_i->seconds >= 0
            waiter_i->second->post();
    }

    listMutex.unlock();

    return true;
}

