/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/UdpCarrier.h>
#include <yarp/os/ConstString.h>

using namespace yarp::os;
using namespace yarp::os::impl;

yarp::os::impl::UdpCarrier::UdpCarrier() {
}

yarp::os::Carrier *yarp::os::impl::UdpCarrier::create() {
    return new UdpCarrier();
}

yarp::os::ConstString yarp::os::impl::UdpCarrier::getName() {
    return "udp";
}

int yarp::os::impl::UdpCarrier::getSpecifierCode() {
    return 0;
}

bool yarp::os::impl::UdpCarrier::checkHeader(const Bytes& header) {
    return getSpecifier(header)%16 == getSpecifierCode();
}

void yarp::os::impl::UdpCarrier::getHeader(const Bytes& header) {
    createStandardHeader(getSpecifierCode(), header);
}

void yarp::os::impl::UdpCarrier::setParameters(const Bytes& header) {
    YARP_UNUSED(header);
}

bool yarp::os::impl::UdpCarrier::requireAck() {
    return false;
}

bool yarp::os::impl::UdpCarrier::isConnectionless() {
    return true;
}


bool yarp::os::impl::UdpCarrier::respondToHeader(ConnectionState& proto) {
    // I am the receiver

    // issue: need a fresh port number...
    DgramTwoWayStream *stream = new DgramTwoWayStream();
    yAssert(stream!=nullptr);

    Contact remote = proto.getStreams().getRemoteAddress();
    bool ok = stream->open(remote);
    if (!ok) {
        delete stream;
        return false;
    }

    int myPort = stream->getLocalAddress().getPort();
    writeYarpInt(myPort, proto);
    proto.takeStreams(stream);

    return true;
}

bool yarp::os::impl::UdpCarrier::expectReplyToHeader(ConnectionState& proto) {
    // I am the sender
    int myPort = proto.getStreams().getLocalAddress().getPort();
    ConstString myName = proto.getStreams().getLocalAddress().getHost();
    ConstString altName = proto.getStreams().getRemoteAddress().getHost();

    int altPort = readYarpInt(proto);

    if (altPort==-1) {
        return false;
    }

    DgramTwoWayStream *stream = new DgramTwoWayStream();
    yAssert(stream!=nullptr);

    proto.takeStreams(nullptr); // free up port from tcp
    bool ok =
        stream->open(Contact(myName, myPort), Contact(altName, altPort));
    if (!ok) {
        delete stream;
        return false;
    }
    proto.takeStreams(stream);
    return true;
}
