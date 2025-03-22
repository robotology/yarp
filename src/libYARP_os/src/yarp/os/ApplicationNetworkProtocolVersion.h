/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ApplicationNetworkProtocolVersion_H
#define ApplicationNetworkProtocolVersion_H

#include <yarp/os/api.h>
#include <yarp/os/Log.h>
#include <yarp/os/Portable.h>

namespace yarp::os {

class YARP_os_API ApplicationNetworkProtocolVersion : public yarp::os::Portable
{
    public:
    short int protocol_version = -1;
    short int yarp_major = -1;
    short int yarp_minor = -1;
    short int yarp_patch = -1;

    public:
    ApplicationNetworkProtocolVersion();
    ~ApplicationNetworkProtocolVersion() = default;

    public:
    std::string toString();

    public:
    bool read(yarp::os::ConnectionReader& connection) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
};

}

#endif // ApplicationNetworkProtocolVersion_H
