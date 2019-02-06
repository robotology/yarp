/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Mutex.h>
#include <mutex>

using yarp::os::Mutex;

class Mutex::Private : public std::mutex
{
};

Mutex::Mutex() :
        mPriv(new Private)
{
}

Mutex::~Mutex()
{
    delete mPriv;
}

void Mutex::lock()
{
    mPriv->lock();
}

bool Mutex::try_lock()
{
    return mPriv->try_lock();
}

void Mutex::unlock()
{
    mPriv->unlock();
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
bool Mutex::tryLock()
{
    return mPriv->try_lock();
}
#endif // YARP_NO_DEPRECATED
