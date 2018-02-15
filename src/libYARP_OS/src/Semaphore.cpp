/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/SemaphoreImpl.h>

using namespace yarp::os::impl;
using namespace yarp::os;

Semaphore::Semaphore(unsigned int initialCount) {
    implementation = new SemaphoreImpl(initialCount);
    yAssert(implementation!=nullptr);
}

Semaphore::~Semaphore() {
    if (implementation!=nullptr) {
        delete ((SemaphoreImpl*)implementation);
        implementation = nullptr;
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
