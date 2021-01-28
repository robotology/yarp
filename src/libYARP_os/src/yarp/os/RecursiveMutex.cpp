/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#include <yarp/os/RecursiveMutex.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE

#include <mutex>

using yarp::os::RecursiveMutex;

class RecursiveMutex::Private
{
public:
    std::recursive_mutex mutex;
};

RecursiveMutex::RecursiveMutex() :
        mPriv(new Private)
{
}

RecursiveMutex::~RecursiveMutex()
{
    delete mPriv;
}

void RecursiveMutex::lock()
{
    mPriv->mutex.lock();
}

bool RecursiveMutex::try_lock()
{
    return mPriv->mutex.try_lock();
}

void RecursiveMutex::unlock()
{
    mPriv->mutex.unlock();
}

bool RecursiveMutex::tryLock()
{
    return mPriv->mutex.try_lock();
}
