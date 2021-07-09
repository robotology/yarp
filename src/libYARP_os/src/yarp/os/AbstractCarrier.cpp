/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/AbstractCarrier.h>

#include <yarp/os/ConnectionState.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/Route.h>
#include <yarp/os/SizedWriter.h>
#include <yarp/os/impl/LogComponent.h>

using namespace yarp::os;
using namespace yarp::os::impl;

namespace {
YARP_OS_LOG_COMPONENT(ABSTRACTCARRIER, "yarp.os.AbstractCarrier")
} // namespace

void AbstractCarrier::setParameters(const Bytes& header)
{
    YARP_UNUSED(header);
    // default - no parameters
}

bool AbstractCarrier::isConnectionless() const
{
    // conservative choice - shortcuts are taken for connection
    return true;
}

bool AbstractCarrier::supportReply() const
{
    return !isConnectionless();
}

bool AbstractCarrier::canAccept() const
{
    return true;
}

bool AbstractCarrier::canOffer() const
{
    return true;
}

bool AbstractCarrier::isTextMode() const
{
    return false;
}

bool AbstractCarrier::requireAck() const
{
    return false;
}

bool AbstractCarrier::canEscape() const
{
    return true;
}

bool AbstractCarrier::isLocal() const
{
    return false;
}

std::string AbstractCarrier::toString() const
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
    yarp::conf::ssize_t r = proto.is().readFull(number);
    if ((size_t)r != number.length()) {
        yCDebug(ABSTRACTCARRIER, "did not get sender name length");
        return false;
    }
    len = NetType::netInt(number);
    if (len > 1000) {
        len = 1000;
    }
    if (len < 1) {
        len = 1;
    }
    ManagedBytes b(len + 1);
    Bytes bytes(b.get(), len);
    r = proto.is().readFull(bytes);
    if ((int)r != len) {
        yCDebug(ABSTRACTCARRIER, "did not get sender name");
        return false;
    }
    std::string s = b.get();
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

bool AbstractCarrier::isActive() const
{
    return true;
}

void AbstractCarrier::setCarrierParams(const Property& params)
{
    YARP_UNUSED(params);
}

void AbstractCarrier::getCarrierParams(Property& params) const
{
    YARP_UNUSED(params);
}

int AbstractCarrier::getSpecifier(const Bytes& b) const
{
    int x = interpretYarpNumber(b);
    if (x >= 0) {
        return x - 7777;
    }
    return x;
}

void AbstractCarrier::createStandardHeader(int specifier, Bytes& header) const
{
    createYarpNumber(7777 + specifier, header);
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
    const std::string senderName = proto.getSenderSpecifier();
    //const std::string& senderName = getRoute().getFromName();
    NetType::netInt((int)senderName.length() + 1, number);
    OutputStream& os = proto.os();
    os.write(number);
    Bytes b((char*)senderName.c_str(), senderName.length() + 1);
    os.write(b);
    os.flush();
    return os.isOk();
}

bool AbstractCarrier::defaultSendIndex(ConnectionState& proto, SizedWriter& writer)
{
    writeYarpInt(10, proto);
    int len = (int)writer.length();
    char lens[] = {(char)len, (char)1, (char)-1, (char)-1, (char)-1, (char)-1, (char)-1, (char)-1, (char)-1, (char)-1};
    Bytes b(lens, 10);
    OutputStream& os = proto.os();
    os.write(b);
    NetInt32 numberSrc;
    Bytes number((char*)&numberSrc, sizeof(NetInt32));
    for (int i = 0; i < len; i++) {
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
        yarp::conf::ssize_t hdr = proto.is().readFull(header);
        if ((size_t)hdr != header.length()) {
            yCDebug(ABSTRACTCARRIER, "did not get acknowledgement header");
            return false;
        }
        int len = interpretYarpNumber(header);
        if (len < 0) {
            yCDebug(ABSTRACTCARRIER, "acknowledgement header is bad");
            return false;
        }
        size_t len2 = proto.is().readDiscard(len);
        if ((size_t)len != len2) {
            yCDebug(ABSTRACTCARRIER, "did not get an acknowledgement of the promised length");
            return false;
        }
    }
    return true;
}

bool AbstractCarrier::defaultExpectIndex(ConnectionState& proto)
{
    yCDebug(ABSTRACTCARRIER, "expecting an index");
    yCDebug(ABSTRACTCARRIER, "ConnectionState::expectIndex for %s", proto.getRoute().toString().c_str());
    // expect index header
    char buf[8];
    Bytes header((char*)&buf[0], sizeof(buf));
    yarp::conf::ssize_t r = proto.is().readFull(header);
    if ((size_t)r != header.length()) {
        yCDebug(ABSTRACTCARRIER, "broken index");
        return false;
    }
    int len = interpretYarpNumber(header);
    if (len < 0) {
        yCDebug(ABSTRACTCARRIER, "broken index - header is not a number");
        return false;
    }
    if (len != 10) {
        yCDebug(ABSTRACTCARRIER, "broken index - header is wrong length");
        return false;
    }
    yCDebug(ABSTRACTCARRIER, "index coming in happily...");
    char buf2[10];
    Bytes indexHeader((char*)&buf2[0], sizeof(buf2));
    r = proto.is().readFull(indexHeader);
    if ((size_t)r != indexHeader.length()) {
        yCDebug(ABSTRACTCARRIER, "broken index, secondary header");
        return false;
    }
    yCDebug(ABSTRACTCARRIER, "secondary header came in happily...");
    int inLen = (unsigned char)(indexHeader.get()[0]);
    int outLen = (unsigned char)(indexHeader.get()[1]);
    // Big limit on number of blocks here!  Inherited from QNX.
    // should make it go away if it hurts someone.

    int total = 0;
    NetInt32 numberSrc;
    Bytes number((char*)&numberSrc, sizeof(NetInt32));
    for (int i = 0; i < inLen; i++) {
        yarp::conf::ssize_t l = proto.is().readFull(number);
        if ((size_t)l != number.length()) {
            yCDebug(ABSTRACTCARRIER, "bad input block length");
            return false;
        }
        int x = NetType::netInt(number);
        total += x;
    }
    for (int i2 = 0; i2 < outLen; i2++) {
        yarp::conf::ssize_t l = proto.is().readFull(number);
        if ((size_t)l != number.length()) {
            yCDebug(ABSTRACTCARRIER, "bad output block length");
            return false;
        }
        int x = NetType::netInt(number);
        total += x;
    }
    proto.setRemainingLength(total);
    yCDebug(ABSTRACTCARRIER, "Total message length: %d", total);
    return true;
}


bool AbstractCarrier::defaultSendAck(ConnectionState& proto)
{
    yCDebug(ABSTRACTCARRIER, "sending an acknowledgment");
    if (proto.getConnection().requireAck()) {
        writeYarpInt(0, proto);
    }
    return true;
}

int AbstractCarrier::readYarpInt(ConnectionState& proto)
{
    char buf[8];
    Bytes header(&(buf[0]), sizeof(buf));
    yarp::conf::ssize_t len = proto.is().readFull(header);
    if ((size_t)len != header.length()) {
        yCDebug(ABSTRACTCARRIER, "data stream died");
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
    if (b.length() == 8) {
        const char* base = b.get();
        if (base[0] == 'Y' && base[1] == 'A' && base[6] == 'R' && base[7] == 'P') {
            yarp::os::Bytes b2(const_cast<char*>(b.get()) + 2, 4);
            int x = NetType::netInt(b2);
            return x;
        }
    }
    return -1;
}

void AbstractCarrier::createYarpNumber(int x, yarp::os::Bytes& header)
{
    if (header.length() != 8) {
        return;
    }
    char* base = header.get();
    base[0] = 'Y';
    base[1] = 'A';
    base[6] = 'R';
    base[7] = 'P';
    yarp::os::Bytes code(base + 2, 4);
    NetType::netInt(x, code);
}
