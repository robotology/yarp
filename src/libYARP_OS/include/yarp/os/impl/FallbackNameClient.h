/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_FALLBACKNAMECLIENT_H
#define YARP_OS_IMPL_FALLBACKNAMECLIENT_H

#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/Contact.h>
#include <yarp/os/impl/DgramTwoWayStream.h>

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
class YARP_OS_impl_API yarp::os::impl::FallbackNameClient : public ThreadImpl
{
public:
    FallbackNameClient() :
            closed(false)
    {
    }

    virtual void run() override;
    virtual void close() override;

    Contact getAddress();

    static Contact seek();

private:
    Contact address;
    DgramTwoWayStream listen;
    bool closed;
};

#endif // YARP_OS_IMPL_FALLBACKNAMECLIENT_H
