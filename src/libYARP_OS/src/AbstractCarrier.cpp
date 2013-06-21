// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/impl/AbstractCarrier.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/ManagedBytes.h>


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

bool yarp::os::impl::AbstractCarrier::sendIndex(Protocol& proto,
                                                SizedWriter& writer) {
    return defaultSendIndex(proto,writer);
}

bool yarp::os::impl::AbstractCarrier::expectExtraHeader(Protocol& proto) {
    return true;
}

bool yarp::os::impl::AbstractCarrier::expectIndex(Protocol& proto) {
    return defaultExpectIndex(proto);
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
    return defaultSendAck(proto);
}

bool yarp::os::impl::AbstractCarrier::expectAck(Protocol& proto) {
    return defaultExpectAck(proto);
}

bool yarp::os::impl::AbstractCarrier::isActive() {
    return true;
}

void yarp::os::impl::AbstractCarrier::setCarrierParams(const yarp::os::Property& params) {
}

void yarp::os::impl::AbstractCarrier::getCarrierParams(yarp::os::Property& params) {
}

int yarp::os::impl::AbstractCarrier::getSpecifier(const Bytes& b) {
    int x = interpretYarpNumber(b);
    if (x>=0) {
        return x-7777;
    }
    return x;
}

void yarp::os::impl::AbstractCarrier::createStandardHeader(int specifier,const yarp::os::Bytes& header) {
    createYarpNumber(7777+specifier,header);
}

bool yarp::os::impl::AbstractCarrier::write(yarp::os::impl::Protocol& proto, yarp::os::impl::SizedWriter& writer) {
    bool ok = sendIndex(proto,writer);
    if (!ok) {
        return false;
    }
    writer.write(proto.os());
    proto.os().flush();
    return proto.os().isOk();
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
    proto.getConnection().getHeader(header);
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

bool yarp::os::impl::AbstractCarrier::defaultSendIndex(Protocol& proto,
                                                       SizedWriter& writer) {
    writeYarpInt(10,proto);
    int len = (int)writer.length();
    char lens[] = { (char)len, 1,
                    -1, -1, -1, -1,
                    -1, -1, -1, -1 };
    Bytes b(lens,10);
    OutputStream& os = proto.os();
    os.write(b);
    NetInt32 numberSrc;
    yarp::os::Bytes number((char*)&numberSrc,sizeof(NetInt32));
    for (int i=0; i<len; i++) {
        NetType::netInt((int)writer.length(i),number);
        os.write(number);
    }
    NetType::netInt(0,number);
    os.write(number);
    return os.isOk();
}


bool yarp::os::impl::AbstractCarrier::defaultExpectAck(Protocol& proto) {
    if (proto.getConnection().requireAck()) {
        char buf[8];
        yarp::os::Bytes header((char*)&buf[0],sizeof(buf));
        ssize_t hdr = NetType::readFull(proto.is(),header);
        if ((size_t)hdr!=header.length()) {
            YARP_DEBUG(proto.getLog(),"did not get acknowledgement header");
            return false;
        }
        int len = interpretYarpNumber(header);
        if (len<0) {
            YARP_DEBUG(proto.getLog(),"acknowledgement header is bad");
            return false;
        }
        size_t len2 = NetType::readDiscard(proto.is(),len);
        if ((size_t)len!=len2) {
            YARP_DEBUG(proto.getLog(),"did not get an acknowledgement of the promised length");
            return false;
        }
    }
    return true;
}




bool yarp::os::impl::AbstractCarrier::defaultExpectIndex(Protocol& proto) {
    Logger& log = proto.getLog();
    YARP_DEBUG(Logger::get(),"expecting an index");
    YARP_SPRINTF1(Logger::get(),
                  debug,
                  "Protocol::expectIndex for %s", 
                  proto.getRoute().toString().c_str());
    // expect index header
    char buf[8];
    yarp::os::Bytes header((char*)&buf[0],sizeof(buf));
    ssize_t r = NetType::readFull(proto.is(),header);
    if ((size_t)r!=header.length()) {
        YARP_DEBUG(log,"broken index");
        return false;
    }
    int len = interpretYarpNumber(header);
    if (len<0) {
        YARP_DEBUG(log,"broken index - header is not a number");
        return false;
    }
    if (len!=10) {
        YARP_DEBUG(log,"broken index - header is wrong length");
        return false;
    }
    YARP_DEBUG(Logger::get(),"index coming in happily...");
    char buf2[10];
    yarp::os::Bytes indexHeader((char*)&buf2[0],sizeof(buf2));
    r = NetType::readFull(proto.is(),indexHeader);
    if ((size_t)r!=indexHeader.length()) {
        YARP_DEBUG(log,"broken index, secondary header");
        return false;
    }
    YARP_DEBUG(Logger::get(),"secondary header came in happily...");
    int inLen = (unsigned char)(indexHeader.get()[0]);
    int outLen = (unsigned char)(indexHeader.get()[1]);
    // Big limit on number of blocks here!  Inherited from QNX.
    // should make it go away if it hurts someone.

    int total = 0;
    NetInt32 numberSrc;
    yarp::os::Bytes number((char*)&numberSrc,sizeof(NetInt32));
    for (int i=0; i<inLen; i++) {
        ssize_t l = NetType::readFull(proto.is(),number);
        if ((size_t)l!=number.length()) {
            YARP_DEBUG(log,"bad input block length");
            return false;
        }
        int x = NetType::netInt(number);
        total += x;
    }
    for (int i2=0; i2<outLen; i2++) {
        ssize_t l = NetType::readFull(proto.is(),number);
        if ((size_t)l!=number.length()) {
            YARP_DEBUG(log,"bad output block length");
            return false;
        }
        int x = NetType::netInt(number);
        total += x;
    }
    proto.setRemainingLength(total);
    YARP_SPRINTF1(Logger::get(),
                  debug,
                  "Total message length: %d",
                  total);
    return true;
}


bool yarp::os::impl::AbstractCarrier::defaultSendAck(Protocol& proto) {
    YARP_DEBUG(Logger::get(),"sending an acknowledgment");
    if (proto.getConnection().requireAck()) {
        writeYarpInt(0,proto);
    }
    return true;
}

int yarp::os::impl::AbstractCarrier::readYarpInt(Protocol& proto) {
    char buf[8];
    yarp::os::Bytes header((char*)&buf[0],sizeof(buf));
    ssize_t len = NetType::readFull(proto.is(),header);
    if ((size_t)len!=header.length()) {
        YARP_DEBUG(proto.getLog(),"data stream died");
        return -1;
    }
    return interpretYarpNumber(header);
}

void yarp::os::impl::AbstractCarrier::writeYarpInt(int n, Protocol& proto) {
    char buf[8];
    yarp::os::Bytes header((char*)&buf[0],sizeof(buf));
    createYarpNumber(n,header);
    proto.os().write(header);
}

