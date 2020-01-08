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

// General files

#include <yarp/os/impl/TcpAcceptor.h>
#include <yarp/os/impl/PlatformSysWait.h>
#include <yarp/os/impl/PlatformSignal.h>

using namespace yarp::os::impl;
using namespace yarp::os;

#define BACKLOG                1

/**
 * An error handler that reaps the zombies.
 */
void sigchld_handler(int s) {
    while(waitpid(-1, nullptr, WNOHANG) > 0) {}
}


/* **************************************************************************************
 * Implementation of TcpAcceptor
 * **************************************************************************************/

TcpAcceptor::TcpAcceptor() {
    ad = -1;
    port_number = -1;
}

int TcpAcceptor::open(const Contact& address) {

//    printf("TCP/IP start in server mode\n");
    set_handle(socket(AF_INET, SOCK_STREAM, 0));
    if (get_handle() == -1) {
        perror("At TcpAcceptor::open there was an error...");
        return -1;
    }

    int yes=1;
    if (setsockopt(get_handle(), SOL_SOCKET, SO_REUSEADDR, &yes,
            sizeof(int)) == -1) {
        perror("At TcpAcceptor::open there was an error...");
        return -1;
    }

    return shared_open(address);
}

/**
 * Open the server port and listen for clients
 */
int TcpAcceptor::shared_open(const Contact& address) {

    struct sockaddr_in servAddr;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons((address.getPort()>0)?address.getPort():0);
    inet_pton(AF_INET, address.getHost().c_str(), &servAddr.sin_addr);
    memset(servAddr.sin_zero, '\0', sizeof servAddr.sin_zero);

    if (bind(get_handle(), (struct sockaddr *)&servAddr,
            sizeof (struct sockaddr)) == -1) {
        perror("At bind(sockfd) there was an error...");
        return -1;
    }

    if (listen(get_handle(), BACKLOG) == -1) {
        perror("At listen(sockfd) there was an error...");
        return -1;
    }

    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
        perror("At sigaction(address) there was an error...");
        return -1;
    }

    struct sockaddr_in sin;
    socklen_t addrlen = sizeof(sin);
    if (getsockname(get_handle(), (struct sockaddr *)&sin, &addrlen) == 0 &&
            sin.sin_family == AF_INET &&
            addrlen == sizeof(sin)) {
        port_number = (int)ntohs(sin.sin_port);
    } else {
        perror("At getsockname(address) there was an error...");
        return -1;
    }

    return 1;
}


/**
 * Accept connection and set field for clientAddress.
 */
int TcpAcceptor::accept(TcpStream &new_stream) {
    sockaddr *addr = nullptr;
    int len = 0; int *len_ptr = &len;

    new_stream.set_handle( ::accept(get_handle(), (struct sockaddr *)&addr, (socklen_t*)len_ptr) );
    if (new_stream.get_handle() < 0) {
        perror("At accept(sockfd) there was an error...");
        return -1;
    }

    return 1;
}

int TcpAcceptor::close() {
    int result = 0;

    if (get_handle() != -1) {
        result = ::close(get_handle ());
        set_handle (-1);
    }
    return result;
}

#endif
