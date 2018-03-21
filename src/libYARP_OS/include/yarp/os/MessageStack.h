/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_MESSAGESTACK_H
#define YARP_OS_MESSAGESTACK_H

#include <yarp/os/Portable.h>


namespace yarp {
    namespace os {
        class MessageStack;
    }
}

/**
 *
 * Maintain a stack of messages to send asynchronously.
 *
 */
class YARP_OS_API yarp::os::MessageStack {
public:
    /**
     *
     * Constructor.
     * @param max_threads maximum number of worker threads allowed (0 means
     * unlimited)
     *
     */
    MessageStack(int max_threads = 0);

    /**
     *
     * Destructor.
     *
     */
    virtual ~MessageStack();

    /**
     *
     * @param owner the destination to send messages to
     *
     */
    void attach(PortReader& owner);

    /**
     *
     * Add a message to the message stack, to be sent whenever the gods
     * see fit.
     * @param msg the message to send
     * @param tag an optional string to prefix the message with
     *
     */
    void stack(PortWriter& msg, const ConstString& tag = "");
private:
    int max_threads;
    void *implementation;
};

#endif // YARP_OS_MESSAGESTACK_H
