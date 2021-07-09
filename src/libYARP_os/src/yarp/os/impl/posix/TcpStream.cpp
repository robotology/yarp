/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Anne van Rossum <anne@almende.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/conf/system.h>
#ifndef YARP_HAS_ACE


// General files
#include <sys/socket.h>
#include <cstdio>

#include <yarp/os/impl/TcpStream.h>
#include <yarp/os/impl/LogComponent.h>

using namespace yarp::os::impl;

namespace {
YARP_OS_LOG_COMPONENT(TCPSTREAM_POSIX, "yarp.os.impl.TcpStream.posix")
}

/* **************************************************************************************
 * Implementation of TcpStream
 * **************************************************************************************/

TcpStream::TcpStream() = default;

TcpStream::~TcpStream() = default;

int TcpStream::open()
{
    set_handle(socket(AF_INET, SOCK_STREAM, 0));
    if (get_handle() == -1) {
        yCError(TCPSTREAM_POSIX, "At TcpStream::open there was an error: %d, %s", errno, strerror(errno));
        return -1;
    }
    return 0;
}

int TcpStream::get_local_addr(sockaddr & sa)
{
    int len = sizeof(sa);
    if (::getsockname(get_handle(), &sa, reinterpret_cast<socklen_t*>(&len)) == -1) {
        return -1;
    }
    return 0;
}

int TcpStream::get_remote_addr (sockaddr & sa)
{
    int len = sizeof(sa);
    if (::getpeername(get_handle(), &sa, reinterpret_cast<socklen_t*>(&len)) == -1) {
        return -1;
    }
    return 0;
}


#endif
