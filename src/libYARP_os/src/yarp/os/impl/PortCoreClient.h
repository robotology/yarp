/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_PORTCORECLIENT_H
#define YARP_OS_IMPL_PORTCORECLIENT_H

#include <yarp/os/api.h>
#include <yarp/os/Contact.h>
#include <yarp/os/PortCoreMsgs.h>
#include <yarp/os/PortReader.h>

#include <string>

namespace yarp::os::impl {

/**
 * Client side of the administrative commands defined in PortCoreMsgs.thrift
 * and handled by PortCore::adminBlock() of a remote port.
 */
class YARP_os_impl_API PortCoreClient :
        public yarp::os::PortCoreMsgs
{
public:
    explicit PortCoreClient(const std::string& remoteportName, double timeout = 2.0);

    PortCoreClient(const PortCoreClient&) = delete;
    PortCoreClient& operator=(const PortCoreClient&) = delete;

private:
    // Forwards the commands of the thrift client to the administrative
    // interface of the remote port, and passes back its reply.
    class AdminPortLink :
            public yarp::os::PortReader
    {
    public:
        AdminPortLink(const std::string& remoteportName, double timeout);
        bool read(yarp::os::ConnectionReader& connection) override;

    private:
        yarp::os::Contact m_contact;
        double m_timeout;
    };

    AdminPortLink m_link;
};

} // namespace yarp::os::impl

#endif // YARP_OS_IMPL_PORTCORECLIENT_H
