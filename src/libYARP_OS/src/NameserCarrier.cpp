// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/impl/NameserCarrier.h>

using namespace yarp::os;

// just to keep linkers from complaining about empty archive
bool dummyNameserCarrierMethod() {
    return false;
}

yarp::os::impl::NameserTwoWayStream::NameserTwoWayStream(TwoWayStream *delegate) :
        delegate(delegate),
        pendingRead(""),
        swallowRead("VER ") {
}

yarp::os::impl::NameserTwoWayStream::~NameserTwoWayStream() {
    if (delegate!=NULL) {
        delete delegate;
        delegate = NULL;
    }
}

yarp::os::InputStream& yarp::os::impl::NameserTwoWayStream::getInputStream() {
    return *this;
}

yarp::os::OutputStream& yarp::os::impl::NameserTwoWayStream::getOutputStream() {
    return delegate->getOutputStream();
}

const Contact& yarp::os::impl::NameserTwoWayStream::getLocalAddress() {
    return delegate->getLocalAddress();
}

const Contact& yarp::os::impl::NameserTwoWayStream::getRemoteAddress() {
    return delegate->getRemoteAddress();
}

bool yarp::os::impl::NameserTwoWayStream::isOk() {
    return delegate->isOk();
}

void yarp::os::impl::NameserTwoWayStream::reset() {
    delegate->reset();
}

void yarp::os::impl::NameserTwoWayStream::close() {
    delegate->close();
}

void yarp::os::impl::NameserTwoWayStream::beginPacket() {
    delegate->beginPacket();
}

void yarp::os::impl::NameserTwoWayStream::endPacket() {
    delegate->endPacket();
}

YARP_SSIZE_T yarp::os::impl::NameserTwoWayStream::read(const Bytes& b) {
    // assume it is ok for name_ser to go byte-by-byte
    // since this protocol will be phased out
    if (b.length()<=0) {
        return 0;
    }
    Bytes tmp(b.get(),1);
    while (swallowRead.length()>0) {
        YARP_SSIZE_T r = delegate->getInputStream().read(tmp);
        if (r<=0) { return r; }
        swallowRead = swallowRead.substr(1,swallowRead.length()-1);
    }
    if (pendingRead.length()>0) {
        b.get()[0] = pendingRead[0];
        pendingRead = pendingRead.substr(1,pendingRead.length()-1);
        return 1;
    }
    YARP_SSIZE_T r = delegate->getInputStream().read(tmp);
    if (r<=0) { return r; }
    if (tmp.get()[0]=='\n') {
        pendingRead = "";
        swallowRead = "NAME_SERVER ";
    }
    return r;
}


yarp::os::impl::NameserCarrier::NameserCarrier() {
    firstSend = true;
}

yarp::os::impl::String yarp::os::impl::NameserCarrier::getName() {
    return "name_ser";
}

yarp::os::impl::String yarp::os::impl::NameserCarrier::getSpecifierName() {
    return "NAME_SER";
}

yarp::os::Carrier *yarp::os::impl::NameserCarrier::create() {
    return new NameserCarrier();
}

bool yarp::os::impl::NameserCarrier::checkHeader(const yarp::os::Bytes& header) {
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

void yarp::os::impl::NameserCarrier::getHeader(const Bytes& header) {
    if (header.length()==8) {
        String target = getSpecifierName();
        for (int i=0; i<8; i++) {
            header.get()[i] = target[i];
        }
    }
}


bool yarp::os::impl::NameserCarrier::requireAck() {
    return false;
}

bool yarp::os::impl::NameserCarrier::isTextMode() {
    return true;
}

bool yarp::os::impl::NameserCarrier::supportReply() {
    return true;
}

bool yarp::os::impl::NameserCarrier::canEscape() {
    return false;
}

bool yarp::os::impl::NameserCarrier::sendHeader(ConnectionState& proto) {
    yarp::os::impl::String target = getSpecifierName();
    yarp::os::Bytes b((char*)target.c_str(),8);
    proto.os().write(b);
    proto.os().flush();
    return proto.os().isOk();
}

bool yarp::os::impl::NameserCarrier::expectSenderSpecifier(ConnectionState& proto) {
    proto.setRoute(proto.getRoute().addFromName("anon"));
    return true;
}

bool yarp::os::impl::NameserCarrier::expectIndex(ConnectionState& proto) {
    return true;
}

bool yarp::os::impl::NameserCarrier::sendAck(ConnectionState& proto) {
    return true;
}

bool yarp::os::impl::NameserCarrier::expectAck(ConnectionState& proto) {
    return true;
}

bool yarp::os::impl::NameserCarrier::respondToHeader(ConnectionState& proto) {
    // I am the receiver
    NameserTwoWayStream *stream =
        new NameserTwoWayStream(proto.giveStreams());
    proto.takeStreams(stream);
    return true;
}

bool yarp::os::impl::NameserCarrier::expectReplyToHeader(ConnectionState& proto) {
    // I am the sender
    return true;
}

bool yarp::os::impl::NameserCarrier::write(ConnectionState& proto, SizedWriter& writer) {
    String target = firstSend?"VER ":"NAME_SERVER ";
    Bytes b((char*)target.c_str(),target.length());
    proto.os().write(b);
    String txt;
    // ancient nameserver can't deal with quotes
    for (size_t i=0; i<writer.length(); i++) {
        for (size_t j=0; j<writer.length(i); j++) {
            char ch = writer.data(i)[j];
            if (ch!='\"') {
                txt += ch;
            }
        }
    }
    Bytes b2((char*)txt.c_str(),txt.length());
    proto.os().write(b2);
    proto.os().flush();
    firstSend = false;
    return proto.os().isOk();
}
