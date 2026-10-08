/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/PortCoreClient.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Network.h>
#include <yarp/os/impl/LogComponent.h>

using namespace yarp::os;
using namespace yarp::os::impl;

namespace {
YARP_OS_LOG_COMPONENT(PORTCORECLIENT, "yarp.os.impl.PortCoreClient")
} // namespace

PortCoreClient::PortCoreClient(const std::string& remoteportName, double timeout) :
        m_link(remoteportName, timeout)
{
    yarp().attachAsClient(m_link);
}

PortCoreClient::AdminPortLink::AdminPortLink(const std::string& remoteportName, double timeout) :
        m_contact(Contact::fromString(remoteportName)),
        m_timeout(timeout)
{
}

bool PortCoreClient::AdminPortLink::read(ConnectionReader& connection)
{
    Bottle cmd;
    Bottle reply;
    cmd.read(connection);
    if (!NetworkBase::write(m_contact, cmd, reply, true, true, m_timeout)) {
        yCError(PORTCORECLIENT, "Cannot write to %s", m_contact.getName().c_str());
        return false;
    }
    ConnectionWriter* writer = connection.getWriter();
    if (writer != nullptr) {
        reply.write(*writer);
    }
    return true;
}
