/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PORTCOREPACKET_H
#define YARP_OS_IMPL_PORTCOREPACKET_H

#include <yarp/os/PortWriter.h>
#include <yarp/os/NetType.h>

namespace yarp {
    namespace os {
        namespace impl {
            class PortCorePacket;
        }
    }
}

/**
 * A single message, potentially being transmitted on multiple connections.
 * This tracks uses of the message for memory management purposes.
 */
class yarp::os::impl::PortCorePacket
{
public:
    PortCorePacket *prev_; ///< this packet will be in a list of active packets
    PortCorePacket *next_; ///< this packet will be in a list of active packets
    yarp::os::PortWriter *content;  ///< the object being sent
    yarp::os::PortWriter *callback; ///< where to send event notifications
    int ct;                ///< number of uses of the messagae
    bool owned;            ///< should we memory-manage the content object
    bool ownedCallback;    ///< should we memory-manage the callback object
    bool completed;        ///< has a notification of completion been sent

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
    yarp::os::PortWriter *getContent()
    {
        return content;
    }

    /**
     * @return the object to which notifications should be sent.
     */
    yarp::os::PortWriter *getCallback()
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
    void setContent(yarp::os::PortWriter *writable, bool owned = false,
                    yarp::os::PortWriter *callback = nullptr,
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
            if (getContent()!=nullptr) {
                getCallback()->onCompletion();
                completed = true;
            }
        }
    }
};

#endif // YARP_OS_IMPL_PORTCOREPACKET_H
