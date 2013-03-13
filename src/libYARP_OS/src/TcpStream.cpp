/**
 * @file TcpStream.cpp
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
#include <sys/socket.h>
#include <stdio.h>

#include <yarp/os/impl/TcpStream.h>

using namespace yarp::os::impl;

/* **************************************************************************************
 * Implementation of TcpStream
 * **************************************************************************************/

TcpStream::TcpStream() {
    sd = -1;
}

TcpStream::~TcpStream() {

}

int TcpStream::open() {
    set_handle(socket(AF_INET, SOCK_STREAM, 0));
    if (get_handle() == -1) {
        perror("At TcpStream::open there was an error...");
        return -1;
    }
    return 0;
}

int TcpStream::get_local_addr (sockaddr & sa) {

    int len = sizeof(sa);

    if (::getsockname (get_handle (), &sa, (socklen_t*)&len) == -1) {
        return -1;
    }
    return 0;
}

int TcpStream::get_remote_addr (sockaddr & sa) {
    int len = sizeof(sa);

    if (::getpeername (get_handle(), &sa, (socklen_t*) &len) == -1) {
        return -1;
    }
    return 0;
}


#endif
