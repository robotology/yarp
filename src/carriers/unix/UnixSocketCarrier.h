/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    UnixSocketCarrier(const UnixSocketCarrier&) = delete;
    UnixSocketCarrier(UnixSocketCarrier&&) = delete;
    UnixSocketCarrier& operator=(const UnixSocketCarrier&) = delete;
    UnixSocketCarrier& operator=(UnixSocketCarrier&&) = delete;

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

    bool expectAck(yarp::os::ConnectionState& proto) override;
    bool sendAck(yarp::os::ConnectionState& proto) override;

    // The initiator reads the carrier parameters from the connection string (i.e. unix_stream+ack)
    bool configure(yarp::os::ConnectionState& proto) override;
    bool configureFromProperty(yarp::os::Property& options) override;

    // The recipients must decipher the parameters from the first 8 bytes (i.e. UNIX_ACK)
    void setParameters(const yarp::os::Bytes& header) override;

private:
    static constexpr const char* name = "unix_stream";
    static constexpr int specifierCode = 11;
    static constexpr const char* headerCode = "UNIX_STR";

    static constexpr const char* name_ack = "unix_stream_ack";
    static constexpr int specifierCode_ack = 12;
    static constexpr const char* headerCode_ack = "UNIX_ACK";

    static constexpr size_t headerSize = 8;

    static constexpr const char* ack_string = "ACK";
    static constexpr size_t ack_string_size = 4;

    std::string socketPath;
    bool requireAckFlag{false};
    UnixSockTwoWayStream* stream{nullptr};

    bool becomeUnixSocket(yarp::os::ConnectionState& proto, bool sender = false);
};

#endif // YARP_UNIX_UNIXSOCKETCARRIER_H
