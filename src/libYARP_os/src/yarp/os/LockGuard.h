/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_LOCKGUARD_H
#define YARP_OS_LOCKGUARD_H

#include <yarp/conf/system.h>

#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/os/LockGuard.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // since YARP 3.3

#include <yarp/os/api.h>
#include <mutex>

namespace yarp {
namespace os {

class Mutex;
class RecursiveMutex;

/**
 * This class is a mutex wrapper that provides a convenient RAII-style mechanism for owning
 * a mutex for the duration of a scoped block. When a LockGuard object is created,
 * it attempts to take ownership of the mutex it is given.
 * When control leaves the scope in which the LockGuard object was created,
 * the LockGuard is destructed and the mutex is released.
 * The lock_guard class is non-copyable.
 *
 * @deprecated since YARP 3.3
 */
template <typename Lockable>
class YARP_DEPRECATED_MSG("Use std::lock_guard or std::scoped_lock instead") AbstractLockGuard
{
public:
    /**
     * Acquires ownership of the given mutex _mutex.
     * The behavior is undefined if _mutex is destroyed before the LockGuard object is.
     * @param _mutex the mutex which will be locked
     */
    explicit AbstractLockGuard(Lockable& _lock);

    /**
     * destructs the LockGuard object, unlocks the underlying mutex
     */
    ~AbstractLockGuard();

private:
    /** Copy constructor is disabled */
    AbstractLockGuard(const AbstractLockGuard&);

    /** Assignment operator is disabled */
    AbstractLockGuard& operator=(const AbstractLockGuard&);

    Lockable& lock; /*!< underlining mutex */
};

// Implementation

YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING

template <typename Lockable>
yarp::os::AbstractLockGuard<Lockable>::AbstractLockGuard(Lockable& _lock) :
        lock(_lock)
{
    lock.lock();
}

template <typename Lockable>
yarp::os::AbstractLockGuard<Lockable>::~AbstractLockGuard()
{
    lock.unlock();
}

template <typename Lockable>
yarp::os::AbstractLockGuard<Lockable>::AbstractLockGuard(const AbstractLockGuard& lg) :
        lock(lg.lock)
{
}

template <typename Lockable>
yarp::os::AbstractLockGuard<Lockable>& yarp::os::AbstractLockGuard<Lockable>::operator=(const AbstractLockGuard&)
{
    return *this;
}

YARP_DEPRECATED_TYPEDEF_MSG("use std::lock_guard or std::scoped_lock instead") AbstractLockGuard<Mutex> LockGuard;
YARP_DEPRECATED_TYPEDEF_MSG("use std::lock_guard or std::scoped_lock instead") AbstractLockGuard<RecursiveMutex> RecursiveLockGuard;

YARP_WARNING_POP

} // namespace os
} // namespace yarp

#endif // YARP_NO_DEPRECATED

#endif // YARP_OS_LOCKGUARD_H
