/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_EVENT_H
#define YARP_OS_EVENT_H

#include <yarp/os/api.h>

namespace yarp {
namespace os {

/**
 * \ingroup key_class
 *
 * A class for thread synchronization and mutual exclusion.
 * An event can be in a signaled or reset state.  Threads
 * can wait for an event to enter a signaled state.
 *
 */
class YARP_os_API Event
{
public:
    /**
     * Constructor.  Sets the initial value of the counter.
     * @param autoResetAfterWait if set, reset() will be called
     * automatically after wait().
     */
    Event(bool autoResetAfterWait = true);

    /**
     * Destructor.
     */
    virtual ~Event();

    /**
     *
     * Wait for the event to be signaled.  If the event was created
     * with autoResetAfterWait set, then after a wait the
     * event will automatically be reset.  That means that any
     * other thread waiting for the same event will not be woken up
     * by the signal that wakes up this call to wait().
     *
     */
    void wait();


    /**
     * Put the event in a signaled state. A thread wait()ing for
     * the event will wake up.  If the event was created with
     * autoResetAfterWait set, then at most one thread will wake up
     * (and then automatically reset the event), otherwise all
     * waiting threads will wake up.
     *
     */
    void signal();

    /**
     *
     * Put the event in a reset state.
     *
     */
    void reset();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_EVENT_H
