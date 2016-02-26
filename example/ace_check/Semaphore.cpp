/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "Semaphore.h"
#include "SemaphoreImpl.h"

Semaphore::Semaphore(int initialCount) {
    implementation = new SemaphoreImpl(initialCount);
    if (implementation==NULL) {
        ACE_OS::printf("Could not allocate thread, exitting\n");
        ACE_OS::exit(1);
    }
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

bool Semaphore::check() {
    return ((SemaphoreImpl*)implementation)->check();
}

void Semaphore::post() {
    ((SemaphoreImpl*)implementation)->post();
}

