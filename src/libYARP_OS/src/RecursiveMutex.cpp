/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/RecursiveMutex.h>
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

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
bool RecursiveMutex::tryLock()
{
    return mPriv->mutex.try_lock();
}
#endif // YARP_NO_DEPRECATED
