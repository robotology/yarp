/*
 * Copyright (C) 2006, 2010 RobotCub Consortium, Daniel Krieg
 * Authors: Paul Fitzpatrick, Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/TcpFace.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/SocketTwoWayStream.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/impl/NameConfig.h>

#include <yarp/os/impl/PlatformStdio.h>

using namespace yarp::os::impl;
using namespace yarp::os;



TcpFace::~TcpFace() {
    closeFace();
}


bool TcpFace::open(const Contact& address) {
    YARP_DEBUG(Logger::get(),ConstString("opening for address ") + address.toURI());

    this->address = address;
#ifdef YARP_HAS_ACE
    ACE_INET_Addr serverAddr((address.getPort()>0)?address.getPort():0);
    int result = peerAcceptor.open(serverAddr,1);
    if (address.getPort()<=0) {
        ACE_INET_Addr localAddr;
        peerAcceptor.get_local_addr(localAddr);
        this->address = address.addSocket("tcp",
                                          NameConfig::getHostName(),
                                          localAddr.get_port_number());
    }
#else
    int result = peerAcceptor.open(address);
    if (address.getPort()<=0) {
        this->address = address.addSocket("tcp",
                                          NameConfig::getHostName(),
                                          peerAcceptor.get_port_number());
    }
#endif
    if (result==-1) {
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

        address = Contact();
    }
}

static void showError(Logger& log) {
    YARP_ERROR(log,"Authentication failed.");
    YARP_ERROR(log,"Authentication was enabled in the auth.conf file.");
    YARP_ERROR(log,"If you do not want to use authentication, please");
    YARP_ERROR(log,"remove this file.");
    YARP_ERROR(log,"If you do want to set up authentication, check:");
    YARP_ERROR(log,"  http://www.yarp.it/yarp_port_auth.html");
}

/**
 * return NULL on failure.  No exceptions thrown.
 */
InputProtocol *TcpFace::read() {

    SocketTwoWayStream *stream  = new SocketTwoWayStream();
    yAssert(stream!=NULL);

    int result = stream->open(peerAcceptor);
    if (result<0) {
        //ACE_OS::printf("exception on tcp stream read: %s\n", e.toString().c_str());
        stream->close();
        delete stream;
        stream = NULL;
    }

    if (stream!=NULL) {
        stream->setReadTimeout(2.0);
        stream->setWriteTimeout(2.0);

        bool success = auth.authSource(&(stream->getInputStream()), &(stream->getOutputStream()));
        if (! success ) {
            showError(Logger::get());
            return NULL;
        }
        stream->setReadTimeout(0.);
        stream->setWriteTimeout(0.);

        return new Protocol(stream);
    }
    return NULL;


}

OutputProtocol *TcpFace::write(const Contact& address) {
    SocketTwoWayStream *stream  = new SocketTwoWayStream();
    int result = stream->open(address);
    if (result<0) {
        stream->close();
        delete stream;
        return NULL;
    }

    if (stream!=NULL) {
        stream->setReadTimeout(2.0);
        stream->setWriteTimeout(2.0);

        bool success = auth.authDest(&(stream->getInputStream()), &(stream->getOutputStream()));
        if (! success ) {
            showError(Logger::get());
            return NULL;
        }
        stream->setReadTimeout(0.);
        stream->setWriteTimeout(0.);

        return new Protocol(stream);
    }
    return NULL;

}


Contact TcpFace::getLocalAddress() {
    return address;
}
