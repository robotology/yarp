// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/TcpCarrier.h>

using namespace yarp::os;


yarp::os::impl::TcpCarrier::TcpCarrier(bool requireAckFlag) {
    this->requireAckFlag = requireAckFlag;
}

yarp::os::Carrier *yarp::os::impl::TcpCarrier::create() {
    return new TcpCarrier(requireAckFlag);
}

yarp::os::impl::String yarp::os::impl::TcpCarrier::getName() {
    return requireAckFlag?"tcp":"fast_tcp";
}

int yarp::os::impl::TcpCarrier::getSpecifierCode() {
    return 3;
}

bool yarp::os::impl::TcpCarrier::checkHeader(const yarp::os::Bytes& header) {
    int spec = getSpecifier(header);
    if (spec%16 == getSpecifierCode()) {
        if (((spec&128)!=0) == requireAckFlag) {
            return true;
        }
    }
    return false;
}

void yarp::os::impl::TcpCarrier::getHeader(const yarp::os::Bytes& header) {
    createStandardHeader(getSpecifierCode()+(requireAckFlag?128:0), header);
}

void yarp::os::impl::TcpCarrier::setParameters(const yarp::os::Bytes& header) {
    //int specifier = getSpecifier(header);
    //requireAckFlag = (specifier&128)!=0;
    // Now prefilter by ack flag
}

bool yarp::os::impl::TcpCarrier::requireAck() {
    return requireAckFlag;
}

bool yarp::os::impl::TcpCarrier::isConnectionless() {
    return false;
}

bool yarp::os::impl::TcpCarrier::respondToHeader(ConnectionState& proto) {
    int cport = proto.getStreams().getLocalAddress().getPort();
    writeYarpInt(cport,proto);
    return proto.checkStreams();
}

bool yarp::os::impl::TcpCarrier::expectReplyToHeader(ConnectionState& proto) {
    readYarpInt(proto); // ignore result
    return proto.checkStreams();
}
