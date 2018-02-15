/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
