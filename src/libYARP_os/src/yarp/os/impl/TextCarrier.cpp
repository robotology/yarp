/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/TextCarrier.h>

#include <yarp/os/ConnectionState.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/Route.h>
#include <yarp/os/impl/LogComponent.h>

using namespace yarp::os;
using namespace yarp::os::impl;

namespace {
YARP_OS_LOG_COMPONENT(TEXTCARRIER, "yarp.os.impl.TextCarrier")
} // namespace

yarp::os::impl::TextCarrier::TextCarrier(bool ackVariant)
{
    this->ackVariant = ackVariant;
}

std::string yarp::os::impl::TextCarrier::getName() const
{
    if (ackVariant) {
        return "text_ack";
    }
    return "text";
}

std::string yarp::os::impl::TextCarrier::getSpecifierName() const
{
    if (ackVariant) {
        return "CONNACK ";
    }
    return "CONNECT ";
}

bool yarp::os::impl::TextCarrier::checkHeader(const Bytes& header)
{
    if (header.length() == 8) {
        std::string target = getSpecifierName();
        for (int i = 0; i < 8; i++) {
            if (!(target[i] == header.get()[i])) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void yarp::os::impl::TextCarrier::getHeader(Bytes& header) const
{
    if (header.length() == 8) {
        std::string target = getSpecifierName();
        for (int i = 0; i < 8; i++) {
            header.get()[i] = target[i];
        }
    }
}

yarp::os::Carrier* yarp::os::impl::TextCarrier::create() const
{
    return new TextCarrier(ackVariant);
}

bool yarp::os::impl::TextCarrier::requireAck() const
{
    return ackVariant;
}

bool yarp::os::impl::TextCarrier::isTextMode() const
{
    return true;
}


bool yarp::os::impl::TextCarrier::supportReply() const
{
    return requireAck();
}

bool yarp::os::impl::TextCarrier::sendHeader(ConnectionState& proto)
{
    std::string target = getSpecifierName();
    yarp::os::Bytes b((char*)target.c_str(), 8);
    proto.os().write(b);
    std::string from = proto.getSenderSpecifier();
    yarp::os::Bytes b2((char*)from.c_str(), from.length());
    proto.os().write(b2);
    proto.os().write('\r');
    proto.os().write('\n');
    proto.os().flush();
    return proto.os().isOk();
}

bool yarp::os::impl::TextCarrier::expectReplyToHeader(ConnectionState& proto)
{
    if (ackVariant) {
        // expect and ignore welcome line
        std::string result = proto.is().readLine();
    }
    return true;
}

bool yarp::os::impl::TextCarrier::expectSenderSpecifier(ConnectionState& proto)
{
    yCDebug(TEXTCARRIER, "TextCarrier::expectSenderSpecifier");
    Route route = proto.getRoute();
    route.setFromName(proto.is().readLine());
    proto.setRoute(route);
    return true;
}

bool yarp::os::impl::TextCarrier::sendIndex(ConnectionState& proto, SizedWriter& writer)
{
    YARP_UNUSED(proto);
    YARP_UNUSED(writer);
    return true;
}

bool yarp::os::impl::TextCarrier::expectIndex(ConnectionState& proto)
{
    YARP_UNUSED(proto);
    return true;
}

bool yarp::os::impl::TextCarrier::sendAck(ConnectionState& proto)
{
    if (ackVariant) {
        std::string from = "<ACK>\r\n";
        Bytes b2((char*)from.c_str(), from.length());
        proto.os().write(b2);
        proto.os().flush();
    }
    return proto.os().isOk();
}

bool yarp::os::impl::TextCarrier::expectAck(ConnectionState& proto)
{
    if (ackVariant) {
        // expect and ignore acknowledgement
        std::string result = proto.is().readLine();
    }
    return true;
}

bool yarp::os::impl::TextCarrier::respondToHeader(ConnectionState& proto)
{
    std::string from = "Welcome ";
    from += proto.getRoute().getFromName();
    from += "\r\n";
    yarp::os::Bytes b2((char*)from.c_str(), from.length());
    proto.os().write(b2);
    proto.os().flush();
    return proto.os().isOk();
}
