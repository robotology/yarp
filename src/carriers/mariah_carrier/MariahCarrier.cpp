/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/ConnectionState.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/OutputStream.h>
#include "MariahCarrier.h"

#include<iostream>

using namespace yarp::os;

bool MariahCarrier::sendHeader(yarp::os::ConnectionState& proto) {
    // Send the "magic number" for this carrier
    ManagedBytes header(8);
    getHeader(header.bytes());
    proto.os().write(header.bytes());
    if (!proto.os().isOk()) return false;

    // Now we can do whatever we want, as long as somehow
    // we also send the name of the originating port

    // let's just send the port name in plain text terminated with a
    // carriage-return / line-feed
    std::string from = proto.getRoute().getFromName();
    Bytes b2((char*)from.c_str(),from.length());
    proto.os().write(b2);
    proto.os().write('\r');
    proto.os().write('\n');
    proto.os().flush();
    return proto.os().isOk();
}


Carrier* MariahCarrier::create() const {
    return new MariahCarrier();
}

std::string MariahCarrier::getName() const {
    return "mariah";
}

bool MariahCarrier::isConnectionless() const {
    return true;
}

bool MariahCarrier::canAccept() const {
    return true;
}

bool MariahCarrier::canOffer() const {
    return true;
}

bool MariahCarrier::isTextMode() const {
    // let's be text mode, for human-friendliness
    return true;
}

bool MariahCarrier::canEscape() const  {
    return true;
}

bool MariahCarrier::requireAck() const {
    return true;
}

bool MariahCarrier::supportReply() const {
    return true;
}

bool MariahCarrier::isLocal() const {
    return false;
}

std::string MariahCarrier::toString() const {
    return "All I want for Xmas is Y...ARP";
}

void MariahCarrier::getHeader(Bytes& header) const {
    const char *target = "MARIAH";
    for (size_t i=0; i<6 && i<header.length(); i++) {
        header.get()[i] = target[i];
    }
}

bool MariahCarrier::checkHeader(const Bytes& header) {
    if (header.length()!=6) {
        return false;
    }
    const char *target = "MARIAH";
    for (size_t i=0; i<6; i++) {
        if (header.get()[i] != target[i]) {
            return false;
        }
    }
    return true;
}

void MariahCarrier::setParameters(const Bytes& header) {
    // no parameters - no carrier variants
}


// Now, the initial hand-shaking

bool MariahCarrier::prepareSend(ConnectionState& proto) {
    // nothing special to do
    return true;
}

bool MariahCarrier::expectSenderSpecifier(ConnectionState& proto) {
    // interpret everything that sendHeader wrote
    Route route = proto.getRoute();
    route.setFromName(proto.is().readLine());
    proto.setRoute(route);
    return proto.is().isOk();
}

bool MariahCarrier::expectExtraHeader(ConnectionState& proto) {
    // interpret any extra header information sent - optional
    return true;
}

bool MariahCarrier::respondToHeader(ConnectionState& proto) {
    return true;
}

bool MariahCarrier::expectReplyToHeader(ConnectionState& proto) {
    // SWITCH TO NEW STREAM TYPE
    return true;
}

bool MariahCarrier::isActive() const {
    return true;
}


// Payload time!

bool MariahCarrier::write(ConnectionState& proto, SizedWriter& writer) {
    bool ok = sendIndex(proto,writer);
    if (!ok) return false;
    writer.write(proto.os());
    return proto.os().isOk();
}

bool MariahCarrier::sendIndex(ConnectionState& proto,SizedWriter& writer) {
    std::string prefix = "Mariah says ";
    Bytes b2((char*)prefix.c_str(),prefix.length());
    proto.os().write(b2);
    return true;
}

bool MariahCarrier::expectIndex(ConnectionState& proto) {
    std::string prefix = "Mariah says ";
    std::string compare = prefix;
    Bytes b2((char*)prefix.c_str(),prefix.length());
    proto.is().read(b2);
    bool ok = proto.is().isOk() && (prefix==compare);
    if (!ok) std::cout << "YOU DID NOT SAY 'Mariah says '" << std::endl;
    return ok;
}

// Acknowledgements, we don't do them

bool MariahCarrier::sendAck(ConnectionState& proto) {
    std::string prefix = "All I want for Xmas is Y...ARP\r\n";
    Bytes b2((char*)prefix.c_str(),prefix.length());
    proto.os().write(b2);
    return true;
}

bool MariahCarrier::expectAck(ConnectionState& proto) {
    std::string prefix = "All I want for Xmas is Y...ARP\r\n";
    std::string compare = prefix;
    Bytes b2((char*)prefix.c_str(),prefix.length());
    proto.is().read(b2);
    bool ok = proto.is().isOk() && (prefix==compare);
    if (!ok) std::cout << "YOU DID NOT SAY 'All I want for Xmas is Y...ARP'" << std::endl;
    return ok;
}
