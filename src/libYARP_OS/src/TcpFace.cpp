// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/TcpFace.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/SocketTwoWayStream.h>
#include <yarp/os/impl/Protocol.h>

#include <yarp/os/impl/PlatformStdio.h>

using namespace yarp::os::impl;

static Logger tcpFaceLog("TcpFace", Logger::get());


TcpFace::~TcpFace() {
    closeFace();
}


bool TcpFace::open(const Address& address) {
    YARP_DEBUG(tcpFaceLog,String("opening for address ") + address.toString());

    this->address = address;
#ifdef YARP_HAS_ACE
    ACE_INET_Addr  serverAddr(address.getPort());
    int result = peerAcceptor.open(serverAddr,1);
#else
    int result = peerAcceptor.open(address);
#endif
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
    if (address.isValid()) {
        peerAcceptor.close();

        /*
          // a test for an arcane yarprun issue

        ACE_INET_Addr  serverAddr(address.getPort());
        ACE_SOCK_Acceptor peerAcceptor2;
        result = peerAcceptor2.open(serverAddr,1);
        printf("TcpFace close %s %d, %s, %d\n", __FILE__, __LINE__,
               address.toString().c_str(), result);
        peerAcceptor2.close();
        */

        address = Address();
    }
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
#ifdef YARP_HAS_ACE
        stream->setReadTimeout(2.0);
        stream->setWriteTimeout(2.0);
#endif

        bool success = auth.authSource(&(stream->getInputStream()), &(stream->getOutputStream()));
        if (! success ) {
            YARP_ERROR(tcpFaceLog,"authentication failed");
            return NULL;
        }
#ifdef YARP_HAS_ACE
        stream->setReadTimeout(0.);
        stream->setWriteTimeout(0.);
#endif

        return new Protocol(stream);
    }
    return NULL;


}

OutputProtocol *TcpFace::write(const Address& address) {
    SocketTwoWayStream *stream  = new SocketTwoWayStream();
    int result = stream->open(address);
    if (result<0) {
        stream->close();
        delete stream;
        return NULL;
    }

    if (stream!=NULL) {
#ifdef YARP_HAS_ACE
        stream->setReadTimeout(2.0);
        stream->setWriteTimeout(2.0);
#endif

        bool success = auth.authDest(&(stream->getInputStream()), &(stream->getOutputStream()));
        if (! success ) {
            YARP_ERROR(tcpFaceLog,"authentication failed");
            return NULL;
        }
#ifdef YARP_HAS_ACE
        stream->setReadTimeout(0.);
        stream->setWriteTimeout(0.);
#endif

        return new Protocol(stream);
    }
    return NULL;

}

