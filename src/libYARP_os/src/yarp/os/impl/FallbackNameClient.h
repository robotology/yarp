/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_FALLBACKNAMECLIENT_H
#define YARP_OS_IMPL_FALLBACKNAMECLIENT_H

#include <yarp/os/Contact.h>
#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/os/impl/ThreadImpl.h>

namespace yarp {
namespace os {
namespace impl {

/**
 * A client for the FallbackNameServer class.  Provides a last-resort
 * means of searching for the name server.
 */
class YARP_os_impl_API FallbackNameClient :
        public ThreadImpl
{
public:
    FallbackNameClient() = default;

    void run() override;
    void close() override;

    Contact getAddress();

    static Contact seek();

private:
    Contact address;
    DgramTwoWayStream listen;
    bool closed{false};
};


} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_FALLBACKNAMECLIENT_H
