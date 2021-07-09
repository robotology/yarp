/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Anne van Rossum <anne@almende.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/conf/system.h>
#ifndef YARP_HAS_ACE

// General files

#include <yarp/os/impl/TcpAcceptor.h>
#include <yarp/os/impl/PlatformSysWait.h>
#include <yarp/os/impl/PlatformSignal.h>
#include <yarp/os/impl/LogComponent.h>

using namespace yarp::os::impl;
using namespace yarp::os;

#define BACKLOG                1

namespace {
YARP_OS_LOG_COMPONENT(TCPACCEPTOR_POSIX, "yarp.os.impl.TcpAcceptor.posix")
}

/**
 * An error handler that reaps the zombies.
 */
void sigchld_handler(int /*s*/)
{
    while(waitpid(-1, nullptr, WNOHANG) > 0) {}
}


/* **************************************************************************************
 * Implementation of TcpAcceptor
 * **************************************************************************************/

int TcpAcceptor::open(const Contact& address)
{
    yCDebug(TCPACCEPTOR_POSIX, "TCP/IP start in server mode");
    set_handle(socket(AF_INET, SOCK_STREAM, 0));
    if (get_handle() < 0) {
        yCError(TCPACCEPTOR_POSIX, "At TcpAcceptor::open there was an error: %d, %s", errno, strerror(errno));
        return -1;
    }

    int yes=1;
    if (setsockopt(get_handle(), SOL_SOCKET, SO_REUSEADDR, &yes,
            sizeof(int)) == -1) {
        yCError(TCPACCEPTOR_POSIX, "At TcpAcceptor::open there was an error: %d, %s", errno, strerror(errno));
        return -1;
    }

    return shared_open(address);
}

/**
 * Open the server port and listen for clients
 */
int TcpAcceptor::shared_open(const Contact& address)
{
    struct sockaddr_in servAddr;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons((address.getPort()>0)?address.getPort():0);
    inet_pton(AF_INET, address.getHost().c_str(), &servAddr.sin_addr);
    memset(servAddr.sin_zero, '\0', sizeof servAddr.sin_zero);

    if (bind(get_handle(), (struct sockaddr *)&servAddr, sizeof (struct sockaddr)) < 0) {
        yCError(TCPACCEPTOR_POSIX, "At bind(sockfd) there was an error: %d, %s", errno, strerror(errno));
        return -1;
    }

    if (listen(get_handle(), BACKLOG) < 0) {
        yCError(TCPACCEPTOR_POSIX, "At listen(sockfd) there was an error: %d, %s", errno, strerror(errno));
        return -1;
    }

    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, nullptr) < 0) {
        yCError(TCPACCEPTOR_POSIX, "At sigaction(address) there was an error: %d, %s", errno, strerror(errno));
        return -1;
    }

    struct sockaddr_in sin;
    socklen_t addrlen = sizeof(sin);
    if (getsockname(get_handle(), (struct sockaddr *)&sin, &addrlen) == 0 &&
            sin.sin_family == AF_INET &&
            addrlen == sizeof(sin)) {
        port_number = static_cast<int>(ntohs(sin.sin_port));
    } else {
        yCError(TCPACCEPTOR_POSIX, "At getsockname(address) there was an error: %d, %s", errno, strerror(errno));
        return -1;
    }

    return 1;
}


/**
 * Accept connection and set field for clientAddress.
 */
int TcpAcceptor::accept(TcpStream &new_stream)
{
    sockaddr* addr = nullptr;
    int len = 0;
    int* len_ptr = &len;

    new_stream.set_handle(::accept(get_handle(), reinterpret_cast<struct sockaddr*>(&addr), reinterpret_cast<socklen_t*>(len_ptr)));
    if (new_stream.get_handle() < 0) {
        yCError(TCPACCEPTOR_POSIX, "At accept(sockfd) there was an error: %d, %s", errno, strerror(errno));
        return -1;
    }

    return 1;
}

int TcpAcceptor::close()
{
    int result = 0;

    if (get_handle() != -1) {
        result = ::close(get_handle());
        set_handle (-1);
    }
    return result;
}

#endif
