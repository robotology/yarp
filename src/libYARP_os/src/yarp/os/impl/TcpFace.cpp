/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/TcpFace.h>

#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/impl/SocketTwoWayStream.h>

#include <cstdio>

using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(TCPFACE, "yarp.os.impl.TcpFace")
} // namespace


TcpFace::TcpFace() = default;

TcpFace::~TcpFace()
{
    closeFace();
}


bool TcpFace::open(const Contact& address)
{
    yCDebug(TCPFACE, "opening for address %s", address.toURI().c_str());

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
    yCDebug(TCPFACE, "Opened at address %s", this->address.toURI().c_str());

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

static void showError()
{
    yCError(TCPFACE, "Authentication failed.");
    yCError(TCPFACE, "Authentication was enabled in the auth.conf file.");
    yCError(TCPFACE, "If you do not want to use authentication, please");
    yCError(TCPFACE, "remove this file.");
    yCError(TCPFACE, "If you do want to set up authentication, check:");
    yCError(TCPFACE, "  http://www.yarp.it/yarp_port_auth.html");
}

/**
 * return nullptr on failure.  No exceptions thrown.
 */
InputProtocol* TcpFace::read()
{
    auto* stream = new SocketTwoWayStream();
    yCAssert(TCPFACE, stream != nullptr);

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
            showError();
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
            showError();
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
