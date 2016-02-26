/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/TextCarrier.h>

using namespace yarp::os;
using namespace yarp::os::impl;

yarp::os::impl::TextCarrier::TextCarrier(bool ackVariant) {
    this->ackVariant = ackVariant;
}

yarp::os::impl::String yarp::os::impl::TextCarrier::getName() {
    if (ackVariant) {
        return "text_ack";
    }
    return "text";
}

yarp::os::impl::String yarp::os::impl::TextCarrier::getSpecifierName() {
    if (ackVariant) {
        return "CONNACK ";
    }
    return "CONNECT ";
}

bool yarp::os::impl::TextCarrier::checkHeader(const Bytes& header) {
    if (header.length()==8) {
        String target = getSpecifierName();
        for (int i=0; i<8; i++) {
            if (!(target[i]==header.get()[i])) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void yarp::os::impl::TextCarrier::getHeader(const Bytes& header) {
    if (header.length()==8) {
        String target = getSpecifierName();
        for (int i=0; i<8; i++) {
            header.get()[i] = target[i];
        }
    }
}

yarp::os::Carrier *yarp::os::impl::TextCarrier::create() {
    return new TextCarrier(ackVariant);
}

bool yarp::os::impl::TextCarrier::requireAck() {
    return ackVariant;
}

bool yarp::os::impl::TextCarrier::isTextMode() {
    return true;
}


bool yarp::os::impl::TextCarrier::supportReply() {
    return requireAck();
}

bool yarp::os::impl::TextCarrier::sendHeader(ConnectionState& proto) {
    yarp::os::impl::String target = getSpecifierName();
    yarp::os::Bytes b((char*)target.c_str(),8);
    proto.os().write(b);
    yarp::os::impl::String from = proto.getSenderSpecifier();
    yarp::os::Bytes b2((char*)from.c_str(),from.length());
    proto.os().write(b2);
    proto.os().write('\r');
    proto.os().write('\n');
    proto.os().flush();
    return proto.os().isOk();
}

bool yarp::os::impl::TextCarrier::expectReplyToHeader(ConnectionState& proto) {
    if (ackVariant) {
        // expect and ignore welcome line
        yarp::os::impl::String result = proto.is().readLine();
    }
    return true;
}

bool yarp::os::impl::TextCarrier::expectSenderSpecifier(ConnectionState& proto) {
    YARP_SPRINTF0(Logger::get(),debug,"TextCarrier::expectSenderSpecifier");
    proto.setRoute(proto.getRoute().addFromName(proto.is().readLine()));
    return true;
}

bool yarp::os::impl::TextCarrier::sendIndex(ConnectionState& proto, SizedWriter& writer) {
    return true;
}

bool yarp::os::impl::TextCarrier::expectIndex(ConnectionState& proto) {
    return true;
}

bool yarp::os::impl::TextCarrier::sendAck(ConnectionState& proto) {
    if (ackVariant) {
        String from = "<ACK>\r\n";
        Bytes b2((char*)from.c_str(),from.length());
        proto.os().write(b2);
        proto.os().flush();
    }
    return proto.os().isOk();
}

bool yarp::os::impl::TextCarrier::expectAck(ConnectionState& proto) {
    if (ackVariant) {
        // expect and ignore acknowledgement
        String result = proto.is().readLine();
    }
    return true;
}

bool yarp::os::impl::TextCarrier::respondToHeader(ConnectionState& proto) {
    yarp::os::impl::String from = "Welcome ";
    from += proto.getRoute().getFromName();
    from += "\r\n";
    yarp::os::Bytes b2((char*)from.c_str(),from.length());
    proto.os().write(b2);
    proto.os().flush();
    return proto.os().isOk();
}
