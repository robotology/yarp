/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_PORTCOREPACKET_H
#define YARP_OS_IMPL_PORTCOREPACKET_H

#include <yarp/os/NetType.h>
#include <yarp/os/PortWriter.h>

namespace yarp {
namespace os {
namespace impl {

/**
 * A single message, potentially being transmitted on multiple connections.
 * This tracks uses of the message for memory management purposes.
 */
class PortCorePacket
{
public:
    PortCorePacket* prev_;                ///< this packet will be in a list of active packets
    PortCorePacket* next_;                ///< this packet will be in a list of active packets
    const yarp::os::PortWriter* content;  ///< the object being sent
    const yarp::os::PortWriter* callback; ///< where to send event notifications
    int ct;                               ///< number of uses of the messagae
    bool owned;                           ///< should we memory-manage the content object
    bool ownedCallback;                   ///< should we memory-manage the callback object
    bool completed;                       ///< has a notification of completion been sent

    /**
     * Constructor.
     */
    PortCorePacket() :
            prev_(nullptr),
            next_(nullptr),
            content(nullptr),
            callback(nullptr),
            ct(0),
            owned(false),
            ownedCallback(false),
            completed(false)
    {
        reset();
    }

    /**
     * Destructor.  Destroy any owned objects unconditionally.
     */
    virtual ~PortCorePacket()
    {
        complete();
        reset();
    }

    /**
     * @return number of users of this message.
     */
    int getCount()
    {
        return ct;
    }

    /**
     * Increment the usage count for this messagae.
     */
    void inc()
    {
        ct++;
    }

    /**
     * Decrement the usage count for this messagae.
     */
    void dec()
    {
        ct--;
    }

    /**
     * @return the object being sent.
     */
    const yarp::os::PortWriter* getContent()
    {
        return content;
    }

    /**
     * @return the object to which notifications should be sent.
     */
    const yarp::os::PortWriter* getCallback()
    {
        return (callback != nullptr) ? callback : content;
    }

    /**
     * Configure the object being sent and where to send notifications.
     *
     * @param writable the object being sent
     * @param owned should we memory-manage `writable`
     * @param callback where to send notifications
     * @param ownedCallback should we memory-manage `callback`
     */
    void setContent(const yarp::os::PortWriter* writable,
                    bool owned = false,
                    const yarp::os::PortWriter* callback = nullptr,
                    bool ownedCallback = false)
    {
        content = writable;
        this->callback = callback;
        ct = 1;
        this->owned = owned;
        this->ownedCallback = ownedCallback;
        completed = false;
    }

    /**
     * Delete anything we own and enter a clean state, as if freshly created.
     */
    void reset()
    {
        if (owned) {
            delete content;
        }
        if (ownedCallback) {
            delete callback;
        }
        content = nullptr;
        callback = nullptr;
        ct = 0;
        owned = false;
        ownedCallback = false;
        completed = false;
    }

    /**
     * Send a completion notification if we haven't already, and there's
     * somewhere to send it to.
     */
    void complete()
    {
        if (!completed) {
            if (getContent() != nullptr) {
                getCallback()->onCompletion();
                completed = true;
            }
        }
    }
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PORTCOREPACKET_H
