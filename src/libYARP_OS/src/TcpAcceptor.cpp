/**
 * @file TcpAcceptor.cpp
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
 * @date    Feb 15, 2011
 * @project Replicator FP7
 * @company Almende B.V.
 * @case
 */


#include <yarp/conf/system.h>
#ifndef YARP_HAS_ACE

// General files

#include <yarp/os/impl/TcpAcceptor.h>

using namespace yarp::os::impl;
using namespace yarp::os;

#define BACKLOG                1

/**
 * An error handler that reaps the zombies.
 */
void sigchld_handler(int s) {
    while(waitpid(-1, YARP_NULLPTR, WNOHANG) > 0) {}
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
    inet_aton(address.getHost().c_str(), &servAddr.sin_addr);
    memset(servAddr.sin_zero, '\0', sizeof servAddr.sin_zero);

//    servAddress = Address(inet_ntoa(servAddr.sin_addr),servAddr.sin_port);

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
    if (sigaction(SIGCHLD, &sa, YARP_NULLPTR) == -1) {
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
//    printf("Waiting for client to accept...\n");

    sockaddr *addr = 0;
    int len = 0; int *len_ptr = &len;

    new_stream.set_handle( ::accept(get_handle(), (struct sockaddr *)&addr, (socklen_t*)len_ptr) );
    if (new_stream.get_handle() < 0) {
        perror("At accept(sockfd) there was an error...");
        return -1;
    }

//    clientAddress = Address(inet_ntoa(clientAddr.sin_addr),clientAddr.sin_port);

//    char text[256];
//    sprintf(text, "Connected to client %s", inet_ntoa(clientAddr.sin_addr));
//    sprintf(text, " with server fd %i and client fd %i", sockets.get_serv_sockfd(), sockets.get_client_sockfd());
//    printf("%s\n", text);
//    happy = true;
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
