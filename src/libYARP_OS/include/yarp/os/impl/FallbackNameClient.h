/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

    void run() override;
    void close() override;

    Contact getAddress();

    static Contact seek();

private:
    Contact address;
    DgramTwoWayStream listen;
    bool closed;
};

#endif // YARP_OS_IMPL_FALLBACKNAMECLIENT_H
