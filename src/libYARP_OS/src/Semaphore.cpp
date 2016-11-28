/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/SemaphoreImpl.h>

using namespace yarp::os::impl;
using namespace yarp::os;

Semaphore::Semaphore(unsigned int initialCount) {
    implementation = new SemaphoreImpl(initialCount);
    yAssert(implementation!=YARP_NULLPTR);
}

Semaphore::~Semaphore() {
    if (implementation!=YARP_NULLPTR) {
        delete ((SemaphoreImpl*)implementation);
        implementation = YARP_NULLPTR;
    }
}

void Semaphore::wait() {
    ((SemaphoreImpl*)implementation)->wait();
}

bool Semaphore::waitWithTimeout(double timeoutInSeconds) {
    return ((SemaphoreImpl*)implementation)->waitWithTimeout(timeoutInSeconds);
}

bool Semaphore::check() {
    return ((SemaphoreImpl*)implementation)->check();
}

void Semaphore::post() {
    ((SemaphoreImpl*)implementation)->post();
}

