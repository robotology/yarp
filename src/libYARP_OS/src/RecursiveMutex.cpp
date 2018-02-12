/*
 * Copyright (C) 2015 Istituto Italiano di Tecnologia (IIT)
 * Authors: Francesco Romano
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/RecursiveMutex.h>
#include <yarp/os/impl/LockImpl.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os::impl;
using namespace yarp::os;

RecursiveMutex::RecursiveMutex() {
    implementation = new RecursiveLockImpl();
    yAssert(implementation!=nullptr);
}

RecursiveMutex::~RecursiveMutex() {
    RecursiveLockImpl *lock = static_cast<RecursiveLockImpl*>(implementation);
    if (lock) {
        delete lock;
        implementation = nullptr;
    }
}

void RecursiveMutex::lock() {
    RecursiveLockImpl *lock = static_cast<RecursiveLockImpl*>(implementation);
    lock->lock();
}

bool RecursiveMutex::tryLock() {
    RecursiveLockImpl *lock = static_cast<RecursiveLockImpl*>(implementation);
    return lock->tryLock();
}

void RecursiveMutex::unlock() {
    RecursiveLockImpl *lock = static_cast<RecursiveLockImpl*>(implementation);
    lock->unlock();
}
