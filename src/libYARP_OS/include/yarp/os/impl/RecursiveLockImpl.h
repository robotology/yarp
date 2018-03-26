/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_RECURSIVELOCKIMPL_H
#define YARP_OS_IMPL_RECURSIVELOCKIMPL_H

#include <mutex>


namespace yarp {
namespace os {
namespace impl {

class YARP_OS_impl_API RecursiveLockImpl
{
public:
    RecursiveLockImpl() = default;
    RecursiveLockImpl(RecursiveLockImpl&) = delete;
    RecursiveLockImpl& operator=(RecursiveLockImpl&) = delete;
    ~RecursiveLockImpl() = default;

    void lock()
    {
        mutex.lock();
    }

    bool tryLock()
    {
        return mutex.try_lock();
    }

    // increment
    void unlock()
    {
        mutex.unlock();
    }

private:
    std::recursive_mutex mutex;
};

} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IMPL_RECURSIVELOCKIMPL_H
