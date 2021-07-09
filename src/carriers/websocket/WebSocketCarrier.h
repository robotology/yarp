/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef WEBSOCKETCARRIER_H
#define WEBSOCKETCARRIER_H

#include "WebSocket/WebSocket.h"

#include <yarp/os/Carrier.h>
#include <yarp/os/ConnectionState.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/NetType.h>
#include <yarp/os/SizedWriter.h>
#include <yarp/os/impl/TcpCarrier.h>

#include <cstring>

using namespace yarp::os;
using namespace yarp::os::impl;

class WebSocketCarrier :
        public TcpCarrier
{
public:
    WebSocketCarrier();

    Carrier* create() const override;

    std::string getName() const override;

    bool checkHeader(const Bytes& header) override;
    void getHeader(Bytes& header) const override;
    bool requireAck() const override;
    bool isTextMode() const override;
    bool supportReply() const override;
    bool sendHeader(ConnectionState& proto) override;
    bool expectReplyToHeader(ConnectionState& proto) override;
    bool expectSenderSpecifier(ConnectionState& proto) override;
    bool sendIndex(ConnectionState& proto, SizedWriter& writer) override;
    bool expectIndex(ConnectionState& proto) override;
    bool sendAck(ConnectionState& proto) override;
    bool expectAck(ConnectionState& proto) override;
    bool respondToHeader(ConnectionState& proto) override;
    bool write(ConnectionState& proto, yarp::os::SizedWriter& writer) override;
    bool canOffer() const override;

private:
    static constexpr size_t header_lenght {8};
    WebSocket messageHandler;
};

#endif // WEBSOCKETCARRIER_H
