// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/impl/AbstractCarrier.h>
#include <yarp/os/impl/Protocol.h>


void yarp::os::impl::AbstractCarrier::setParameters(const yarp::os::Bytes& header) {
    // default - no parameters
}

bool yarp::os::impl::AbstractCarrier::isConnectionless() {
    // conservative choice - shortcuts are taken for connection
    return true;
}

bool yarp::os::impl::AbstractCarrier::supportReply() {
    return !isConnectionless();
}

bool yarp::os::impl::AbstractCarrier::canAccept() {
    return true;
}

bool yarp::os::impl::AbstractCarrier::canOffer() {
    return true;
}

bool yarp::os::impl::AbstractCarrier::isTextMode() {
    return false;
}

bool yarp::os::impl::AbstractCarrier::requireAck() {
    return false;
}

bool yarp::os::impl::AbstractCarrier::canEscape() {
    return true;
}

bool yarp::os::impl::AbstractCarrier::isLocal() {
    return false;
}

yarp::os::impl::String yarp::os::impl::AbstractCarrier::toString() {
    return getName();
}

bool yarp::os::impl::AbstractCarrier::prepareSend(Protocol& proto) {
    return true;
}

bool yarp::os::impl::AbstractCarrier::sendHeader(Protocol& proto) {
    return defaultSendHeader(proto);
}

bool yarp::os::impl::AbstractCarrier::expectReplyToHeader(Protocol& proto) {
    return true;
}

bool yarp::os::impl::AbstractCarrier::sendIndex(Protocol& proto) {
    return proto.defaultSendIndex();
}

bool yarp::os::impl::AbstractCarrier::expectExtraHeader(Protocol& proto) {
    return true;
}

bool yarp::os::impl::AbstractCarrier::expectIndex(Protocol& proto) {
    return proto.defaultExpectIndex();
}

bool yarp::os::impl::AbstractCarrier::expectSenderSpecifier(Protocol& proto) {
    NetInt32 numberSrc;
    yarp::os::Bytes number((char*)&numberSrc,sizeof(NetInt32));
    int len = 0;
    ssize_t r = NetType::readFull(proto.is(),number);
    if ((size_t)r!=number.length()) {
        YARP_DEBUG(Logger::get(),"did not get sender name length");
        return false;
    }
    len = NetType::netInt(number);
    if (len>1000) len = 1000;
    if (len<1) len = 1;
    // expect a string -- these days null terminated, but not in YARP1
    yarp::os::ManagedBytes b(len+1);
    r = NetType::readFull(proto.is(),yarp::os::Bytes(b.get(),len));
    if ((int)r!=len) {
        YARP_DEBUG(Logger::get(),"did not get sender name");
        return false;
    }
    // add null termination for YARP1
    b.get()[len] = '\0';
    String s = b.get();
    proto.setRoute(proto.getRoute().addFromName(s));
    return true;
}

bool yarp::os::impl::AbstractCarrier::sendAck(Protocol& proto) {
    return proto.defaultSendAck();
}

bool yarp::os::impl::AbstractCarrier::expectAck(Protocol& proto) {
    return proto.defaultExpectAck();
}

bool yarp::os::impl::AbstractCarrier::isActive() {
    return true;
}

void yarp::os::impl::AbstractCarrier::setCarrierParams(const yarp::os::Property& params) {
}

void yarp::os::impl::AbstractCarrier::getCarrierParams(yarp::os::Property& params) {
}

int yarp::os::impl::AbstractCarrier::getSpecifier(const Bytes& b) {
    int x = yarp::os::impl::Protocol::interpretYarpNumber(b);
    if (x>=0) {
        return x-7777;
    }
    return x;
}

void yarp::os::impl::AbstractCarrier::createStandardHeader(int specifier,const yarp::os::Bytes& header) {
    yarp::os::impl::Protocol::createYarpNumber(7777+specifier,header);
}

bool yarp::os::impl::AbstractCarrier::write(yarp::os::impl::Protocol& proto, yarp::os::impl::SizedWriter& writer) {
    // default behavior upon a write request
    //ACE_UNUSED_ARG(writer);
    bool ok = proto.sendIndex();
    if (!ok) {
        return false;
    }
    ok = proto.sendContent();
    if (!ok) {
        return false;
    }
    // proto.expectAck(); //MOVE ack to after reply, if present
    return true;
}

bool yarp::os::impl::AbstractCarrier::defaultSendHeader(Protocol& proto) {
    bool ok = sendProtocolSpecifier(proto);
    if (!ok) return false;
    return sendSenderSpecifier(proto);
}

bool yarp::os::impl::AbstractCarrier::sendProtocolSpecifier(Protocol& proto) {
    char buf[8];
    yarp::os::Bytes header((char*)&buf[0],sizeof(buf));
    OutputStream& os = proto.os();
    proto.getHeader(header);
    os.write(header);
    os.flush();
    return os.isOk();
}

bool yarp::os::impl::AbstractCarrier::sendSenderSpecifier(Protocol& proto) {
    NetInt32 numberSrc;
    yarp::os::Bytes number((char*)&numberSrc,sizeof(NetInt32));
    const String senderName = proto.getSenderSpecifier();
    //const String& senderName = getRoute().getFromName();
    NetType::netInt((int)senderName.length()+1,number);
    OutputStream& os = proto.os();
    os.write(number);
    yarp::os::Bytes b((char*)senderName.c_str(),senderName.length()+1);
    os.write(b);
    os.flush();
    return os.isOk();
}

