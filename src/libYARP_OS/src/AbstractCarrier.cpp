/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/AbstractCarrier.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os;
using namespace yarp::os::impl;

void AbstractCarrier::setParameters(const Bytes& header)
{
    YARP_UNUSED(header);
    // default - no parameters
}

bool AbstractCarrier::isConnectionless()
{
    // conservative choice - shortcuts are taken for connection
    return true;
}

bool AbstractCarrier::supportReply()
{
    return !isConnectionless();
}

bool AbstractCarrier::canAccept()
{
    return true;
}

bool AbstractCarrier::canOffer()
{
    return true;
}

bool AbstractCarrier::isTextMode()
{
    return false;
}

bool AbstractCarrier::requireAck()
{
    return false;
}

bool AbstractCarrier::canEscape()
{
    return true;
}

bool AbstractCarrier::isLocal()
{
    return false;
}

ConstString AbstractCarrier::toString()
{
    return getName();
}

bool AbstractCarrier::prepareSend(ConnectionState& proto)
{
    YARP_UNUSED(proto);
    return true;
}

bool AbstractCarrier::sendHeader(ConnectionState& proto)
{
    YARP_UNUSED(proto);
    return defaultSendHeader(proto);
}

bool AbstractCarrier::expectReplyToHeader(ConnectionState& proto)
{
    YARP_UNUSED(proto);
    return true;
}

bool AbstractCarrier::sendIndex(ConnectionState& proto, SizedWriter& writer)
{
    return defaultSendIndex(proto, writer);
}

bool AbstractCarrier::expectExtraHeader(ConnectionState& proto)
{
    YARP_UNUSED(proto);
    return true;
}

bool AbstractCarrier::expectIndex(ConnectionState& proto)
{
    return defaultExpectIndex(proto);
}

bool AbstractCarrier::expectSenderSpecifier(ConnectionState& proto)
{
    NetInt32 numberSrc;
    Bytes number((char*)&numberSrc, sizeof(NetInt32));
    int len = 0;
    YARP_SSIZE_T r = proto.is().readFull(number);
    if ((size_t)r!=number.length()) {
        YARP_DEBUG(Logger::get(), "did not get sender name length");
        return false;
    }
    len = NetType::netInt(number);
    if (len>1000) {
        len = 1000;
    }
    if (len<1) {
        len = 1;
    }
    ManagedBytes b(len+1);
    r = proto.is().readFull(Bytes(b.get(), len));
    if ((int)r!=len) {
        YARP_DEBUG(Logger::get(), "did not get sender name");
        return false;
    }
    ConstString s = b.get();
    Route route = proto.getRoute();
    route.setFromName(s);
    proto.setRoute(route);
    return true;
}

bool AbstractCarrier::sendAck(ConnectionState& proto)
{
    return defaultSendAck(proto);
}

bool AbstractCarrier::expectAck(ConnectionState& proto)
{
    return defaultExpectAck(proto);
}

bool AbstractCarrier::isActive()
{
    return true;
}

void AbstractCarrier::setCarrierParams(const Property& params)
{
    YARP_UNUSED(params);
}

void AbstractCarrier::getCarrierParams(Property& params)
{
    YARP_UNUSED(params);
}

int AbstractCarrier::getSpecifier(const Bytes& b)
{
    int x = interpretYarpNumber(b);
    if (x>=0) {
        return x-7777;
    }
    return x;
}

void AbstractCarrier::createStandardHeader(int specifier, const Bytes& header)
{
    createYarpNumber(7777+specifier, header);
}

bool AbstractCarrier::write(ConnectionState& proto, SizedWriter& writer)
{
    bool ok = sendIndex(proto, writer);
    if (!ok) {
        return false;
    }
    writer.write(proto.os());
    proto.os().flush();
    return proto.os().isOk();
}

bool AbstractCarrier::defaultSendHeader(ConnectionState& proto)
{
    bool ok = sendConnectionStateSpecifier(proto);
    if (!ok) {
        return false;
    }
    return sendSenderSpecifier(proto);
}

bool AbstractCarrier::sendConnectionStateSpecifier(ConnectionState& proto)
{
    char buf[8];
    Bytes header((char*)&buf[0], sizeof(buf));
    OutputStream& os = proto.os();
    proto.getConnection().getHeader(header);
    os.write(header);
    os.flush();
    return os.isOk();
}

bool AbstractCarrier::sendSenderSpecifier(ConnectionState& proto)
{
    NetInt32 numberSrc;
    Bytes number((char*)&numberSrc, sizeof(NetInt32));
    const ConstString senderName = proto.getSenderSpecifier();
    //const ConstString& senderName = getRoute().getFromName();
    NetType::netInt((int)senderName.length()+1, number);
    OutputStream& os = proto.os();
    os.write(number);
    Bytes b((char*)senderName.c_str(), senderName.length()+1);
    os.write(b);
    os.flush();
    return os.isOk();
}

bool AbstractCarrier::defaultSendIndex(ConnectionState& proto, SizedWriter& writer)
{
    writeYarpInt(10, proto);
    int len = (int)writer.length();
    char lens[] = { (char)len, (char)1,
                    (char)-1, (char)-1, (char)-1, (char)-1,
                    (char)-1, (char)-1, (char)-1, (char)-1 };
    Bytes b(lens, 10);
    OutputStream& os = proto.os();
    os.write(b);
    NetInt32 numberSrc;
    Bytes number((char*)&numberSrc, sizeof(NetInt32));
    for (int i=0; i<len; i++) {
        NetType::netInt((int)writer.length(i), number);
        os.write(number);
    }
    NetType::netInt(0, number);
    os.write(number);
    return os.isOk();
}

bool AbstractCarrier::defaultExpectAck(ConnectionState& proto)
{
    if (proto.getConnection().requireAck()) {
        char buf[8];
        Bytes header((char*)&buf[0], sizeof(buf));
        YARP_SSIZE_T hdr = proto.is().readFull(header);
        if ((size_t)hdr!=header.length()) {
            YARP_DEBUG(proto.getLog(), "did not get acknowledgement header");
            return false;
        }
        int len = interpretYarpNumber(header);
        if (len<0) {
            YARP_DEBUG(proto.getLog(), "acknowledgement header is bad");
            return false;
        }
        size_t len2 = proto.is().readDiscard(len);
        if ((size_t)len!=len2) {
            YARP_DEBUG(proto.getLog(), "did not get an acknowledgement of the promised length");
            return false;
        }
    }
    return true;
}

bool AbstractCarrier::defaultExpectIndex(ConnectionState& proto)
{
    Log& log = proto.getLog();
    YARP_DEBUG(Logger::get(), "expecting an index");
    YARP_SPRINTF1(Logger::get(),
                  debug,
                  "ConnectionState::expectIndex for %s",
                  proto.getRoute().toString().c_str());
    // expect index header
    char buf[8];
    Bytes header((char*)&buf[0], sizeof(buf));
    YARP_SSIZE_T r = proto.is().readFull(header);
    if ((size_t)r!=header.length()) {
        YARP_DEBUG(log, "broken index");
        return false;
    }
    int len = interpretYarpNumber(header);
    if (len<0) {
        YARP_DEBUG(log, "broken index - header is not a number");
        return false;
    }
    if (len!=10) {
        YARP_DEBUG(log, "broken index - header is wrong length");
        return false;
    }
    YARP_DEBUG(Logger::get(), "index coming in happily...");
    char buf2[10];
    Bytes indexHeader((char*)&buf2[0], sizeof(buf2));
    r = proto.is().readFull(indexHeader);
    if ((size_t)r!=indexHeader.length()) {
        YARP_DEBUG(log, "broken index, secondary header");
        return false;
    }
    YARP_DEBUG(Logger::get(), "secondary header came in happily...");
    int inLen = (unsigned char)(indexHeader.get()[0]);
    int outLen = (unsigned char)(indexHeader.get()[1]);
    // Big limit on number of blocks here!  Inherited from QNX.
    // should make it go away if it hurts someone.

    int total = 0;
    NetInt32 numberSrc;
    Bytes number((char*)&numberSrc, sizeof(NetInt32));
    for (int i=0; i<inLen; i++) {
        YARP_SSIZE_T l = proto.is().readFull(number);
        if ((size_t)l!=number.length()) {
            YARP_DEBUG(log, "bad input block length");
            return false;
        }
        int x = NetType::netInt(number);
        total += x;
    }
    for (int i2=0; i2<outLen; i2++) {
        YARP_SSIZE_T l = proto.is().readFull(number);
        if ((size_t)l!=number.length()) {
            YARP_DEBUG(log, "bad output block length");
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


bool AbstractCarrier::defaultSendAck(ConnectionState& proto)
{
    YARP_DEBUG(Logger::get(), "sending an acknowledgment");
    if (proto.getConnection().requireAck()) {
        writeYarpInt(0, proto);
    }
    return true;
}

int AbstractCarrier::readYarpInt(ConnectionState& proto)
{
    char buf[8];
    Bytes header(&(buf[0]), sizeof(buf));
    YARP_SSIZE_T len = proto.is().readFull(header);
    if ((size_t)len!=header.length()) {
        YARP_DEBUG(proto.getLog(), "data stream died");
        return -1;
    }
    return interpretYarpNumber(header);
}

void AbstractCarrier::writeYarpInt(int n, ConnectionState& proto)
{
    char buf[8];
    Bytes header(&(buf[0]), sizeof(buf));
    createYarpNumber(n, header);
    proto.os().write(header);
}

int AbstractCarrier::interpretYarpNumber(const yarp::os::Bytes& b)
{
    if (b.length()==8) {
        char *base = b.get();
        if (base[0]=='Y' &&
            base[1]=='A' &&
            base[6]=='R' &&
            base[7]=='P') {
            yarp::os::Bytes b2(b.get()+2, 4);
            int x = NetType::netInt(b2);
            return x;
        }
    }
    return -1;
}

void AbstractCarrier::createYarpNumber(int x, const yarp::os::Bytes& header)
{
    if (header.length()!=8) {
        return;
    }
    char *base = header.get();
    base[0] = 'Y';
    base[1] = 'A';
    base[6] = 'R';
    base[7] = 'P';
    yarp::os::Bytes code(base+2, 4);
    NetType::netInt(x, code);
}
