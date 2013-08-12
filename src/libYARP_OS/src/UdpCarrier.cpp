// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/UdpCarrier.h>
#include <yarp/os/impl/Protocol.h>

yarp::os::impl::UdpCarrier::UdpCarrier() {
}

yarp::os::impl::Carrier *yarp::os::impl::UdpCarrier::create() {
    return new UdpCarrier();
}

yarp::os::impl::String yarp::os::impl::UdpCarrier::getName() {
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
}

bool yarp::os::impl::UdpCarrier::requireAck() {
    return false;
}

bool yarp::os::impl::UdpCarrier::isConnectionless() {
    return true;
}


bool yarp::os::impl::UdpCarrier::respondToHeader(Protocol& proto) {
    // I am the receiver

    // issue: need a fresh port number...
    DgramTwoWayStream *stream = new DgramTwoWayStream();
    YARP_ASSERT(stream!=NULL);

    Contact remote = proto.getStreams().getRemoteAddress();
    bool ok = stream->open(remote);
    if (!ok) {
        delete stream;
        return false;
    }

    int myPort = stream->getLocalAddress().getPort();
    writeYarpInt(myPort,proto);
    proto.takeStreams(stream);

    return true;
}

bool yarp::os::impl::UdpCarrier::expectReplyToHeader(Protocol& proto) {
    // I am the sender
    int myPort = proto.getStreams().getLocalAddress().getPort();
    String myName = proto.getStreams().getLocalAddress().getHost();
    String altName = proto.getStreams().getRemoteAddress().getHost();

    int altPort = readYarpInt(proto);

    if (altPort==-1) {
        return false;
    }

    DgramTwoWayStream *stream = new DgramTwoWayStream();
    YARP_ASSERT(stream!=NULL);

    proto.takeStreams(NULL); // free up port from tcp
    bool ok =
        stream->open(Contact(myName,myPort),Contact(altName,altPort));
    if (!ok) {
        delete stream;
        return false;
    }
    proto.takeStreams(stream);
    return true;
}
