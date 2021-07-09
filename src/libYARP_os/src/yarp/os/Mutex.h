/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_MUTEX_H
#define YARP_OS_MUTEX_H

#include <yarp/conf/system.h>

#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/os/Mutex.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.3

#include <yarp/os/api.h>
#include <mutex>

namespace yarp {
namespace os {

/**
 * Basic wrapper for mutual exclusion.
 *
 * Intended to match std::mutex in C++11, for eventual replacement by that
 * class.
 *
 * @deprecated since YARP 3.3. Use \c std::mutex instead.
 */
class YARP_os_DEPRECATED_API_MSG("Use std::mutex instead") Mutex
{
public:
    /**
     * Constructor.
     */
    Mutex();

    /**
     * Destructor.
     */
    ~Mutex();

    /**
     * Lock the associated resource, waiting if necessary.
     * Behavior is undefined if called by the thread currently locking
     * the resource.
     */
    void lock();

    /**
     * Lock the associated resource if possible.  Don't wait.
     * Behavior is undefined if called by the thread currently locking
     * the resource.
     *
     * @return true if the associated resource was successfully locked.
     */
    bool try_lock();

    /**
     * Unlock the associated resource.
     * If the resource is not currently locked by the calling thread,
     * the behavior is undefined.
     */
    void unlock();

    /**
     * Lock the associated resource if possible.  Don't wait.
     * Behavior is undefined if called by the thread currently locking
     * the resource.
     *
     * @return true if the associated resource was successfully locked.
     *
     * @deprecated since YARP 3.0.0. Use try_lock() instead.
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

#endif // YARP_OS_MUTEX_H
