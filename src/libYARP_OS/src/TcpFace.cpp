// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/impl/TcpFace.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/SocketTwoWayStream.h>
#include <yarp/os/impl/Protocol.h>

#include <ace/OS_NS_stdio.h>

using namespace yarp::os::impl;

static Logger tcpFaceLog("TcpFace", Logger::get());


TcpFace::~TcpFace() {
    closeFace();
}


bool TcpFace::open(const Address& address) {
    YARP_DEBUG(tcpFaceLog,String("opening for address ") + address.toString());

    this->address = address;
    ACE_INET_Addr	serverAddr(address.getPort());
    int result = peerAcceptor.open(serverAddr,1);
    if (result==-1) {
        //throw IOException("cannot listen on specified tcp address");
        return false;
    }
    return true;
}

void TcpFace::close() {
    closeFace();
}

void TcpFace::closeFace() {
    peerAcceptor.close();
}


/**
 * return NULL on failure.  No exceptions thrown.
 */
InputProtocol *TcpFace::read() {

    SocketTwoWayStream *stream  = new SocketTwoWayStream();
    YARP_ASSERT(stream!=NULL);

    int result = stream->open(peerAcceptor);
    if (result<0) {
        //ACE_OS::printf("exception on tcp stream read: %s\n", e.toString().c_str());
        stream->close();
        delete stream;
        stream = NULL;
    }

    if (stream!=NULL) {
        return new Protocol(stream);
    }
    return NULL;
}


OutputProtocol *TcpFace::write(const Address& address) {
    SocketTwoWayStream *stream  = new SocketTwoWayStream();
    int result = stream->open(address);
    if (result==-1) {
        stream->close();
        delete stream;
        return NULL;
    }
    return new Protocol(stream);
}

