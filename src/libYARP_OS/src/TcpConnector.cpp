/**
 * @file TcpConnector.cpp
 * @brief
 *
 * This file is created at Almende B.V. It is open-source software and part of the Common
 * Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from
 * thread pools and TCP/IP components to control architectures and learning algorithms.
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless,
 * we personally strongly object against this software used by the military, in the
 * bio-industry, for animal experimentation, or anything that violates the Universal
 * Declaration of Human Rights.
 *
 * Copyright © 2010 Anne van Rossum <anne@almende.com>
 *
 * @author  Anne C. van Rossum
 * @date    Feb 17, 2011
 * @project Replicator FP7
 * @company Almende B.V.
 * @case
 */

#include <yarp/conf/system.h>
#ifndef YARP_HAS_ACE

// General files
#include <yarp/os/impl/TcpConnector.h>

#include <yarp/os/Log.h>
#include <yarp/os/impl/PlatformNetdb.h>

#include <iostream>
#include <cstdio>

#include <sys/socket.h>

using namespace yarp::os::impl;
using namespace yarp::os;

/* **************************************************************************************
 * Implementation of TcpConnector
 * **************************************************************************************/

TcpConnector::TcpConnector() {

}

TcpConnector::~TcpConnector() {

}

int TcpConnector::open(TcpStream &stream) {
    if ((stream.get_handle() == -1) && (stream.open() == -1)) return -1;
    return 0;
}

/**
 * Connect to server
 */
int TcpConnector::connect(TcpStream &new_stream, const Contact& address) {
//     printf("TCP/IP start in client mode\n");
//     sockets.set_as_client();
//     sockets.set_client_sockfd(sockfd);
    if (open (new_stream) == -1)
        return -1;

    // Write sockaddr struct with given address...
    sockaddr_in servAddr;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(address.getPort());
    memset(servAddr.sin_zero, '\0', sizeof servAddr.sin_zero);

    struct hostent *hostInfo = yarp::os::impl::gethostbyname(address.getHost().c_str());
    if (hostInfo) {
        bcopy(hostInfo->h_addr,(char *)(&servAddr.sin_addr),hostInfo->h_length);
    } else {
        inet_pton(AF_INET, address.getHost().c_str(), &servAddr.sin_addr);
    }

    yAssert(new_stream.get_handle() != -1);

    int result = ::connect(new_stream.get_handle(), (sockaddr*) &servAddr, sizeof(servAddr));

    if (result < 0) {
        perror("TcpConnector::connect fail");
        char buf[INET_ADDRSTRLEN];
        std::cerr << "Connect [handle=" << new_stream.get_handle() << "] at " << inet_ntop(AF_INET, &servAddr.sin_addr, buf, INET_ADDRSTRLEN) << ":" << servAddr.sin_port << std::endl;
    }
    return result;
}

#endif
