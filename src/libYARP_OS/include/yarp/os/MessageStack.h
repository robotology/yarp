// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_MESSAGESTACK_
#define _YARP2_MESSAGESTACK_

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

#endif
