/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_FALLBACKNAMESERVER
#define YARP2_FALLBACKNAMESERVER

#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/Contact.h>
#include <yarp/os/impl/DgramTwoWayStream.h>

namespace yarp {
    namespace os {
        namespace impl {
            class FallbackNameServer;
            class NameServerStub;
        }
    }
}

/**
 * Multi-cast server, for last resort information sharing about
 * name information -- when config files are missing or wrong
 */
class YARP_OS_impl_API yarp::os::impl::FallbackNameServer : public ThreadImpl {
public:
    FallbackNameServer(NameServerStub& owner) : owner(owner) {
        closed = false;
    }

    virtual void run();
    virtual void close();

    static Contact getAddress();

private:
    NameServerStub& owner;
    DgramTwoWayStream listen;
    bool closed;
};

#endif

