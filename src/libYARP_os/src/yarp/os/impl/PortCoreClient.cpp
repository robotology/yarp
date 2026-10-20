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

ContactStyle quietStyle(const Contact& contact, double timeout)
{
    ContactStyle style;
    style.quiet = true;
    style.timeout = timeout;
    style.carrier = contact.getCarrier();
    return style;
}
} // namespace

PortCoreClient::PortCoreClient(const std::string& remoteportName, double timeout) :
        PortCoreClient(Contact::fromString(remoteportName),
                       quietStyle(Contact::fromString(remoteportName), timeout))
{
}

PortCoreClient::PortCoreClient(const Contact& remotePort, const ContactStyle& style) :
        m_link(remotePort, style)
{
    yarp().attachAsClient(m_link);
}

PortCoreClient::PortCoreClient(UnbufferedContactable& adminPort) :
        m_link(Contact(), ContactStyle())
{
    yarp().attachAsClient(adminPort);
}

PortCoreClient::AdminPortLink::AdminPortLink(const Contact& remotePort, const ContactStyle& style) :
        m_contact(remotePort),
        m_style(style)
{
    m_style.admin = true;
}

bool PortCoreClient::AdminPortLink::read(ConnectionReader& connection)
{
    Bottle cmd;
    Bottle reply;
    cmd.read(connection);
    //yCDebug(PORTCORECLIENT, "asking %s: %s", m_contact.toString().c_str(), cmd.toString().c_str());
    if (!NetworkBase::write(m_contact, cmd, reply, m_style)) {
        if (!m_style.quiet) {
            yCError(PORTCORECLIENT, "Cannot write to %s", m_contact.getName().c_str());
        }
        return false;
    }
    ConnectionWriter* writer = connection.getWriter();
    if (writer != nullptr) {
        reply.write(*writer);
    }
    return true;
}
