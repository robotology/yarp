/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/TcpFace.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/impl/SocketTwoWayStream.h>

#include <cstdio>

using namespace yarp::os::impl;
using namespace yarp::os;


TcpFace::TcpFace() = default;

TcpFace::~TcpFace()
{
    closeFace();
}


bool TcpFace::open(const Contact& address)
{
    YARP_DEBUG(Logger::get(), std::string("opening for address ") + address.toURI());

    this->address = address;
#ifdef YARP_HAS_ACE
    ACE_INET_Addr serverAddr((address.getPort() > 0) ? address.getPort() : 0);
    int result = peerAcceptor.open(serverAddr, 1);
    if (address.getPort() <= 0) {
        ACE_INET_Addr localAddr;
        peerAcceptor.get_local_addr(localAddr);
        this->address = address;
        this->address.setSocket("tcp",
                                NameConfig::getHostName(),
                                localAddr.get_port_number());
    }
#else
    int result = peerAcceptor.open(address);
    if (address.getPort() <= 0) {
        this->address = address;
        this->address.setSocket("tcp",
                                NameConfig::getHostName(),
                                peerAcceptor.get_port_number());
    }
#endif
    return result != -1;
}

void TcpFace::close()
{
    closeFace();
}

void TcpFace::closeFace()
{
    if (address.isValid()) {
        peerAcceptor.close();

        address = Contact();
    }
}

static void showError(Logger& log)
{
    YARP_ERROR(log, "Authentication failed.");
    YARP_ERROR(log, "Authentication was enabled in the auth.conf file.");
    YARP_ERROR(log, "If you do not want to use authentication, please");
    YARP_ERROR(log, "remove this file.");
    YARP_ERROR(log, "If you do want to set up authentication, check:");
    YARP_ERROR(log, "  http://www.yarp.it/yarp_port_auth.html");
}

/**
 * return nullptr on failure.  No exceptions thrown.
 */
InputProtocol* TcpFace::read()
{
    auto* stream = new SocketTwoWayStream();
    yAssert(stream != nullptr);

    int result = stream->open(peerAcceptor);
    if (result < 0) {
        //printf("exception on tcp stream read: %s\n", e.toString().c_str());
        stream->close();
        delete stream;
        stream = nullptr;
    }

    if (stream != nullptr) {
        stream->setReadTimeout(2.0);
        stream->setWriteTimeout(2.0);

        bool success = auth.authSource(&(stream->getInputStream()), &(stream->getOutputStream()));
        if (!success) {
            showError(Logger::get());
            return nullptr;
        }
        stream->setReadTimeout(0.);
        stream->setWriteTimeout(0.);

        return new Protocol(stream);
    }
    return nullptr;
}

OutputProtocol* TcpFace::write(const Contact& address)
{
    auto* stream = new SocketTwoWayStream();
    int result = stream->open(address);
    if (result < 0) {
        stream->close();
        delete stream;
        return nullptr;
    }

    if (stream != nullptr) {
        stream->setReadTimeout(2.0);
        stream->setWriteTimeout(2.0);

        bool success = auth.authDest(&(stream->getInputStream()), &(stream->getOutputStream()));
        if (!success) {
            showError(Logger::get());
            return nullptr;
        }
        stream->setReadTimeout(0.);
        stream->setWriteTimeout(0.);

        return new Protocol(stream);
    }
    return nullptr;
}


Contact TcpFace::getLocalAddress() const
{
    return address;
}
