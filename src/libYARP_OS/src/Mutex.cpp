/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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
    yAssert(implementation!=YARP_NULLPTR);
}

Mutex::~Mutex() {
    if (implementation!=YARP_NULLPTR) {
        delete ((SemaphoreImpl*)implementation);
        implementation = YARP_NULLPTR;
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

