// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_FALLBACKNAMESERVER_
#define _YARP2_FALLBACKNAMESERVER_

#include <yarp/ThreadImpl.h>
#include <yarp/Address.h>
#include <yarp/DgramTwoWayStream.h>

namespace yarp {
    namespace os {
        namespace impl {
            class FallbackNameServer;
            class NameServer;
        }
    }
}

/**
 * Multi-cast server, for last resort information sharing about
 * name information -- when config files are missing or wrong
 */
class yarp::os::impl::FallbackNameServer : public ThreadImpl {
public:
    FallbackNameServer(NameServer& owner) : owner(owner) {
        closed = false;
    }

    virtual void run();
    virtual void close();

    static const Address& getAddress() {
        return mcastLastResort;
    }

private:
    NameServer& owner;
    static const Address mcastLastResort;
    DgramTwoWayStream listen;
    bool closed;
};

#endif

