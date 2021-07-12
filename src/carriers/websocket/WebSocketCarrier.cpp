/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "WebSocketCarrier.h"
#include "WebSocketStream.h"

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Route.h>


using namespace yarp::os;


YARP_LOG_COMPONENT(WEBSOCKETCARRIER,
                   "yarp.carrier.websocket",
                   yarp::os::Log::TraceType,
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)


WebSocketCarrier::WebSocketCarrier()
{
}

Carrier* WebSocketCarrier::create() const
{
    yCTrace(WEBSOCKETCARRIER);
    return new WebSocketCarrier();
}


std::string WebSocketCarrier::getName() const
{
    yCTrace(WEBSOCKETCARRIER);
    return "websocket";
}


bool WebSocketCarrier::checkHeader(const yarp::os::Bytes& header)
{
    yCTrace(WEBSOCKETCARRIER);
    if (header.length() != header_lenght) {
        return false;
    }
    const char* target = "GET /?ws";
    for (size_t i = 0; i < header_lenght; i++) {
        if (header.get()[i] != target[i]) {
            return false;
        }
    }
    return true;
}


void WebSocketCarrier::getHeader(Bytes& header) const
{
    yCTrace(WEBSOCKETCARRIER);
    const char* target = "GET /?ws";
    if (header.length() == 8) {
        for (int i = 0; i < 8; i++) {
            header.get()[i] = target[i];
        }
    }
}


bool WebSocketCarrier::requireAck() const
{
    yCTrace(WEBSOCKETCARRIER);
    return false;
}


bool WebSocketCarrier::isTextMode() const
{
    yCTrace(WEBSOCKETCARRIER);
    return false;
}


bool WebSocketCarrier::supportReply() const
{
    yCTrace(WEBSOCKETCARRIER);
    return false;
}


bool WebSocketCarrier::sendHeader(ConnectionState& proto)
{
    yCTrace(WEBSOCKETCARRIER);
    YARP_UNUSED(proto);
    return true;
}


bool WebSocketCarrier::expectReplyToHeader(yarp::os::ConnectionState& proto)
{
    yCTrace(WEBSOCKETCARRIER);
    YARP_UNUSED(proto);
    return true;
}


bool WebSocketCarrier::expectSenderSpecifier(yarp::os::ConnectionState& proto)
{
    yCTrace(WEBSOCKETCARRIER);
    std::string url;
    Route route = proto.getRoute();
    route.setFromName("web");
    proto.setRoute(route);
    std::string remainder = proto.is().readLine();
    std::string result = remainder;
    result += "\r\n";

    for (char i : remainder) {
        if (i != ' ') {
            url += i;
        } else {
            break;
        }
    }


    bool done = false;
    while (!done) {
        std::string line = proto.is().readLine();
        result += line;
        result += "\r\n";
        if (line.empty()) {
            done = true;
        }
    }
    auto messagetype = messageHandler.parseHandshake(reinterpret_cast<unsigned char*>(const_cast<char*>(result.c_str())), result.size());
    if (messagetype != WebSocketFrameType::OPENING_FRAME) {
        yCError(WEBSOCKETCARRIER) << "error parsing handshake";
        return false;
    }
    return true;
}


bool WebSocketCarrier::sendIndex(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer)
{
    yCTrace(WEBSOCKETCARRIER);
    YARP_UNUSED(proto);
    YARP_UNUSED(writer);
    return true;
}


bool WebSocketCarrier::expectIndex(yarp::os::ConnectionState& proto)
{
    yCTrace(WEBSOCKETCARRIER);
    YARP_UNUSED(proto);
    return true;
}


bool WebSocketCarrier::sendAck(yarp::os::ConnectionState& proto)
{
    yCTrace(WEBSOCKETCARRIER);
    YARP_UNUSED(proto);
    return true;
}


bool WebSocketCarrier::expectAck(yarp::os::ConnectionState& proto)
{
    yCTrace(WEBSOCKETCARRIER);
    YARP_UNUSED(proto);
    return true;
}


bool WebSocketCarrier::respondToHeader(yarp::os::ConnectionState& proto)
{
    yCTrace(WEBSOCKETCARRIER);
    auto& outputStream = proto.os();
    std::string reply = messageHandler.answerHandshake();
    yarp::os::Bytes replySerialized(&reply[0], reply.length());
    outputStream.write(replySerialized);
    outputStream.flush();
    WebSocketStream* stream = new WebSocketStream(proto.giveStreams());
    proto.takeStreams(stream);
    return proto.os().isOk();
}


bool WebSocketCarrier::write(ConnectionState& proto, yarp::os::SizedWriter& writer)
{
    yCTrace(WEBSOCKETCARRIER);
    writer.write(proto.getOutputStream());
    return true;
}


bool WebSocketCarrier::canOffer() const
{
    yCTrace(WEBSOCKETCARRIER);
    return true;
}
