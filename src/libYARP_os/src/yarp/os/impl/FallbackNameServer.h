/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_FALLBACKNAMESERVER_H
#define YARP_OS_IMPL_FALLBACKNAMESERVER_H

#include <yarp/os/Contact.h>
#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/impl/NameServer.h>

namespace yarp {
namespace os {
namespace impl {

/**
 * Multi-cast server, for last resort information sharing about
 * name information -- when config files are missing or wrong
 */
class YARP_os_impl_API FallbackNameServer :
        public ThreadImpl
{
public:
    FallbackNameServer(NameServerStub& owner) :
            owner(owner),
            closed(false)
    {
    }

    void run() override;
    void close() override;

    static Contact getAddress();

private:
    NameServerStub& owner;
    DgramTwoWayStream listen;
    bool closed;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_FALLBACKNAMESERVER_H
