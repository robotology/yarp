// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

///
/// based on Id: YARPSemaphore.cpp,v 2.0 2005/11/06 22:21:26 gmetta Exp

/// $Id: YARPSemaphore.cpp,v 1.4 2008-10-14 15:56:48 eshuy Exp $
///
///


#include <yarp/os/impl/SemaphoreImpl.h>
#include <yarp/os/impl/Logger.h>

#define DEBUG_H_INC
#include <yarp/YARPSemaphore.h>

using namespace yarp::os::impl;

// skip a level of indirection
#define Semaphore SemaphoreImpl

YARPSemaphore::YARPSemaphore (int initial_count) {
    Semaphore *sema = new Semaphore(initial_count);
    system_resource = (void *)sema;
    YARP_ASSERT(system_resource!=NULL);
}

///
/// clone the semaphore 
///		polls the copied sema, 
/// 
YARPSemaphore::YARPSemaphore (const YARPSemaphore& yt)
{
    Semaphore *sema = new Semaphore (0);
    system_resource = (void *)sema;
    YARP_ASSERT (system_resource != NULL);
	
    if (yt.system_resource != NULL) {
        int ct = 0;
        YARPSemaphore& danger_yt = *((YARPSemaphore *)&yt);

        while (danger_yt.PollingWait()) {
            ct++;
        }

        while (ct) {
            danger_yt.Post();
            Post();
            ct--;
        }
    }
}

YARPSemaphore::~YARPSemaphore ()
{
    if (system_resource!=NULL) {
        //CloseHandle(system_resource);
        if (system_resource != NULL) delete (Semaphore *)system_resource; 
        system_resource = NULL; // not necessary, just coding style
    }
}

void YARPSemaphore::BlockingWait() {
    ((Semaphore *)system_resource)->wait();
}

int YARPSemaphore::PollingWait() {
    return ((Semaphore *)system_resource)->check();
}


void YARPSemaphore::Post () {
    ((Semaphore *)system_resource)->post();
}
