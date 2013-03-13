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
    return proto.defaultSendHeader();
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
    return proto.defaultExpectSenderSpecifier();
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
