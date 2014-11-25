// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

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
    yAssert(implementation!=NULL);
}

Semaphore::~Semaphore() {
    if (implementation!=NULL) {
        delete ((SemaphoreImpl*)implementation);
        implementation = NULL;
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

