/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#ifndef YARP_UNIX_UNIXSOCKETCARRIER_H
#define YARP_UNIX_UNIXSOCKETCARRIER_H

#include <yarp/os/AbstractCarrier.h>

#include "UnixSockTwoWayStream.h"

// The compile pre-defines the "unix" macro, but we don't use it, and it
// conflicts with the generated yarp_plugin_unix.cpp file
#ifdef unix
#    undef unix
#endif

/**
 * Communicating between two ports(IPC) via Unix Socket.
 */
class UnixSocketCarrier :
        public yarp::os::AbstractCarrier
{
public:
    UnixSocketCarrier() = default;
    ~UnixSocketCarrier() override = default;

    yarp::os::Carrier* create() const override;

    std::string getName() const override;

    bool requireAck() const override;
    bool isConnectionless() const override;

    bool checkHeader(const yarp::os::Bytes& header) override;
    void getHeader(yarp::os::Bytes& header) const override;

    bool respondToHeader(yarp::os::ConnectionState& proto) override;
    bool expectReplyToHeader(yarp::os::ConnectionState& proto) override;

    bool expectIndex(yarp::os::ConnectionState& proto) override;
    bool sendIndex(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer) override;


private:
    static constexpr const char* name = "unix_stream";
    static constexpr int specifierCode = 11;
    static constexpr const char* headerCode = "UNIX_STR";
    static constexpr size_t headerSize = 8;

    std::string socketPath;
    UnixSockTwoWayStream* stream{nullptr};

    bool becomeUnixSocket(yarp::os::ConnectionState& proto, bool sender = false);
};

#endif // YARP_UNIX_UNIXSOCKETCARRIER_H
