/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
