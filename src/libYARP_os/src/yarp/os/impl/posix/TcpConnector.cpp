/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Anne van Rossum <anne@almende.com>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/conf/system.h>
#ifndef YARP_HAS_ACE

#    include <yarp/os/impl/TcpConnector.h>

// General files
#    include <yarp/os/Log.h>
#    include <yarp/os/impl/PlatformNetdb.h>

#    include <cstdio>
#    include <fcntl.h>
#    include <iostream>
#    include <sys/socket.h>

using namespace yarp::os::impl;
using namespace yarp::os;

/* **************************************************************************************
 * Implementation of TcpConnector
 * **************************************************************************************/

TcpConnector::TcpConnector() = default;

TcpConnector::~TcpConnector() = default;

int TcpConnector::open(TcpStream& stream)
{
    if ((stream.get_handle() == -1) && (stream.open() == -1)) {
        return -1;
    }
    return 0;
}

/**
 * Connect to server
 */
int TcpConnector::connect(TcpStream& new_stream, const Contact& address, YARP_timeval* timeout)
{
    //     printf("TCP/IP start in client mode\n");
    //     sockets.set_as_client();
    //     sockets.set_client_sockfd(sockfd);
    if (open(new_stream) == -1) {
        return -1;
    }

    // Write sockaddr struct with given address...
    sockaddr_in servAddr;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(address.getPort());
    memset(servAddr.sin_zero, '\0', sizeof servAddr.sin_zero);

    struct hostent* hostInfo = yarp::os::impl::gethostbyname(address.getHost().c_str());
    if (hostInfo) {
        bcopy(hostInfo->h_addr, (char*)(&servAddr.sin_addr), hostInfo->h_length);
    } else {
        inet_pton(AF_INET, address.getHost().c_str(), &servAddr.sin_addr);
    }

    auto handle = new_stream.get_handle();

    yAssert(handle != -1);

    int res;
    long arg;
    fd_set myset;
    int valopt;
    socklen_t lon;

    // Set non-blocking
    if ((arg = fcntl(handle, F_GETFL, NULL)) < 0) {
        std::cerr << "TcpConnector::connect fail: Error fcntl(..., F_GETFL) " << strerror(errno) << std::endl;
        return -1;
    }
    arg |= O_NONBLOCK;
    if (fcntl(handle, F_SETFL, arg) < 0) {
        std::cerr << "TcpConnector::connect fail: Error fcntl(..., F_SETFL) " << strerror(errno) << std::endl;
        return -1;
    }
    // Trying to connect with timeout
    res = ::connect(handle, (sockaddr*)&servAddr, sizeof(servAddr));

    if (res < 0) {
        if (errno == EINPROGRESS) {
            FD_ZERO(&myset);
            FD_SET(handle, &myset);
            res = select(handle + 1, nullptr, &myset, nullptr, timeout);
            if (res < 0 && errno != EINTR) {
                std::cerr << "TcpConnector::connect fail: Error connecting " << errno << " " << strerror(errno) << std::endl;
                res = -1;
            } else if (res > 0) {
                res = 0;
                // Socket selected for write
                lon = sizeof(int);
                if (getsockopt(handle, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) {
                    std::cerr << "TcpConnector::connect fail: Error in getsockopt() " << errno << " " << strerror(errno) << std::endl;
                    res = -1;
                }
                // Check the value returned...
                if (valopt) {
                    // connect fail: Error in delayed connection() -> the port doesn't exist
                    res = -1;
                }
            } else {
                std::cerr << "TcpConnector::connect fail: Timeout in select() - Cancelling!" << std::endl;
                res = -1;
            }
        } else {
            std::cerr << "TcpConnector::connect fail: Error connecting " << errno << " " << strerror(errno) << std::endl;
            res = -1;
        }
    }

    if (res != 0) {
        char buf[INET_ADDRSTRLEN];
        std::cerr << "Connect [handle=" << new_stream.get_handle() << "] at " << inet_ntop(AF_INET, &servAddr.sin_addr, buf, INET_ADDRSTRLEN) << ":" << servAddr.sin_port << std::endl;
        return -1;
    }

    // Set to blocking mode again...
    if ((arg = fcntl(handle, F_GETFL, nullptr)) < 0) {
        std::cerr << "TcpConnector::connect fail: Error fcntl(..., F_GETFL) " << strerror(errno) << std::endl;
        return -1;
    }
    arg &= (~O_NONBLOCK);
    if (fcntl(handle, F_SETFL, arg) < 0) {
        std::cerr << "TcpConnector::connect fail: Error fcntl(..., F_SETFL) " << strerror(errno) << std::endl;
        return -1;
    }

    return res;
}

#endif
