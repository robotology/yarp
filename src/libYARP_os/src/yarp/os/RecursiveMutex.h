/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_RECURSIVELOCK_H
#define YARP_OS_RECURSIVELOCK_H

#include <yarp/conf/system.h>

#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/os/RecursiveMutex.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.3

#include <yarp/os/api.h>
#include <mutex>

namespace yarp {
namespace os {

/**
 * RecursiveMutex offers exclusive, recursive ownership semantics:
 *   - A calling thread owns a RecursiveMutex for a period of time that starts
 *     when it successfully calls either lock or tryLock. During this period,
 *     the thread may make additional calls to lock or tryLock.
 *     The period of ownership ends when the thread makes a matching number of
 *     calls to unlock.
 *   - When a thread owns a RecursiveMutex, all other threads will block (for
 *     calls to lock) or receive a false return value (for tryLock) if they
 *     attempt to claim ownership of the RecursiveLock.
 * The behavior of a program is undefined if a RecursiveMutex is destroyed while
 * still owned by some thread.
 * The behavior of a program is undefined if a RecursiveMutex is unlocked by a
 * thread which is not currently owning the RecursiveMutex
 *
 * @deprecated since YARP 3.3. Use \c std::recursive_mutex instead.
 */
class YARP_os_DEPRECATED_API_MSG("Use std::recursive_mutex instead") RecursiveMutex
{
public:
    /**
     * Constructor.
     */
    RecursiveMutex();

    /**
     * Destructor.
     */
    ~RecursiveMutex();

    /**
     * @brief Lock the associated resource, waiting if the resource is busy.
     *
     * If the thread which is currently owning the resource calls
     * this function, it will not block, and a reference count will be increased
     * Thu number of calls to lock() must be balanced by the same number of
     * calls to unlock()
     */
    void lock();

    /**
     * @brief Lock the associated resource if it is free.
     *
     * @see RecursiveLock#lock() for more detailed description
     * @return true if the associated resource was successfully locked. False
     *         otherwise
     */
    bool try_lock();

    /**
     * @brief Unlock the associated resource thus freeing waiting threads.
     *
     * If the resource is not currently locked by the calling thread,
     * the behavior is undefined.
     */
    void unlock();

    /**
     * @brief Lock the associated resource if it is free.
     *
     * @see RecursiveLock#lock() for more detailed description
     * @return true if the associated resource was successfully locked. False
     *         otherwise
     *
     * @deprecated since YARP 3.0.0. Use \c try_lock() instead.
     */
    YARP_DEPRECATED_MSG("Use try_lock() instead")
    bool tryLock();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_NO_DEPRECATED

#endif // YARP_OS_RECURSIVELOCK_H
