// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

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
    YARP_ASSERT(implementation!=NULL);
}

Mutex::~Mutex() {
    if (implementation!=NULL) {
        delete ((SemaphoreImpl*)implementation);
        implementation = NULL;
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

