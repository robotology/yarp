/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_HIERARCHICALMUTEX_H
#define YARP_OS_HIERARCHICALMUTEX_H

#include <yarp/os/api.h>
#include <mutex>

namespace yarp {
namespace os {

/**
 * @brief The HierarchicalMutex class, implementation taken from
 * C++ Concurrency in Action of Antony Williams
 * (see https://www.manning.com/books/c-plus-plus-concurrency-in-action).
 * It defines a hierarchy among mutexes in order to enforce the lock of
 * mutexes from the higher value of the lowest value of hierarchy.
 */
class YARP_OS_API HierarchicalMutex
{
public:
    /**
     * @brief Constructor
     * @param hierarchy value.
     */
    explicit HierarchicalMutex(unsigned long value);

    /**
     * Destructor.
     */
    ~HierarchicalMutex();

    /**
     * Lock the associated resource, waiting if necessary.
     * Behavior is undefined if called by the thread currently locking
     * the resource.
     * Throws a logic_error exception if the decreasing order of locking is violated.
     */
    void lock();

    /**
     * Lock the associated resource if possible.  Don't wait.
     * Behavior is undefined if called by the thread currently locking
     * the resource.
     * Throws a logic_error exception if the decreasing order of locking is violated.
     * @return true if the associated resource was successfully locked.
     */
    bool try_lock();

    /**
     * Unlock the associated resource.
     * If the resource is not currently locked by the calling thread,
     * the behavior is undefined.
     */
    void unlock();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_HIERARCHICALMUTEX_H
