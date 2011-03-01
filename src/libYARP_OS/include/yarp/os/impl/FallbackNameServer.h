// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_FALLBACKNAMESERVER_
#define _YARP2_FALLBACKNAMESERVER_

#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/impl/Address.h>
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

    static const Address& getAddress() {
        return mcastLastResort;
    }

private:
    NameServerStub& owner;
    static const Address mcastLastResort;
    DgramTwoWayStream listen;
    bool closed;
};

#endif

