/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Mutex.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/SemaphoreImpl.h>

using namespace yarp::os::impl;
using namespace yarp::os;

Mutex::Mutex() {
    // should optimize implementation when better building blocks are
    // available, but for now stick with semaphores
    implementation = new SemaphoreImpl(1);
    yAssert(implementation!=nullptr);
}

Mutex::~Mutex() {
    if (implementation!=nullptr) {
        delete ((SemaphoreImpl*)implementation);
        implementation = nullptr;
    }
}

void Mutex::lock() {
    ((SemaphoreImpl*)implementation)->wait();
}

bool Mutex::tryLock() {
    return ((SemaphoreImpl*)implementation)->check();
}

void Mutex::unlock() {
    ((SemaphoreImpl*)implementation)->post();
}
