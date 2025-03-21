/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/ApplicationNetworkProtocolVersion.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>

using namespace yarp::os;

ApplicationNetworkProtocolVersion::ApplicationNetworkProtocolVersion()
{
}

std::string ApplicationNetworkProtocolVersion::toString()
{
    std::string s;
    s = std::to_string(protocol_version) + " (" +
        std::to_string(yarp_major) + "." +
        std::to_string(yarp_minor) + "." +
        std::to_string(yarp_patch) + ")";
    return s;
}

bool ApplicationNetworkProtocolVersion::read(yarp::os::ConnectionReader& connection)
{
    connection.convertTextMode();
    protocol_version = connection.expectInt16();
    yarp_major = connection.expectInt16();
    yarp_minor = connection.expectInt16();
    yarp_patch = connection.expectInt16();
    return true;
}

bool ApplicationNetworkProtocolVersion::write(yarp::os::ConnectionWriter& connection) const
{
    connection.convertTextMode();
    connection.appendInt16(this->protocol_version);
    connection.appendInt16(this->yarp_major);
    connection.appendInt16(this->yarp_minor);
    connection.appendInt16(this->yarp_patch);
    return true;
}
