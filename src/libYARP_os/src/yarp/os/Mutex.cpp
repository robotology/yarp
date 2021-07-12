/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#include <yarp/os/Mutex.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE

YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING

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

bool Mutex::tryLock()
{
    return mPriv->try_lock();
}

YARP_WARNING_POP
