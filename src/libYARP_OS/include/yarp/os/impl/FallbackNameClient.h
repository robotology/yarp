// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_FALLBACKNAMECLIENT_
#define _YARP2_FALLBACKNAMECLIENT_

#include <yarp/ThreadImpl.h>
#include <yarp/Address.h>
#include <yarp/DgramTwoWayStream.h>

namespace yarp {
    namespace os {
        namespace impl {
            class FallbackNameClient;
        }
    }
}

/**
 * A client for the FallbackNameServer class.  Provides a last-resort
 * means of searching for the name server.
 */
class yarp::os::impl::FallbackNameClient : public ThreadImpl {
public:
    FallbackNameClient() {
        closed = false;
    }

    virtual void run();
    virtual void close();

    Address getAddress();

    static Address seek();

private:
    Address address;
    DgramTwoWayStream listen;
    bool closed;
};

#endif

