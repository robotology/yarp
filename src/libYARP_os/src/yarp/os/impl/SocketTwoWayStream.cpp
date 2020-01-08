/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * Copyright (C) 2006 Anne van Rossum <anne@almende.com>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/SocketTwoWayStream.h>

#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/TcpAcceptor.h>
#include <yarp/os/impl/TcpConnector.h>
#include <yarp/os/impl/TcpStream.h>

#ifdef YARP_HAS_ACE
#    include <ace/INET_Addr.h>
#    include <ace/os_include/netinet/os_tcp.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#elif (__unix__)
#    include <netinet/tcp.h>
#endif

using namespace yarp::os;
using namespace yarp::os::impl;

int SocketTwoWayStream::open(const Contact& address)
{
    if (address.getPort() == -1) {
        return -1;
    }
    std::string host = address.getHost();
    yarp::os::impl::TcpConnector connector;
#ifdef YARP_HAS_ACE
    if (address.getHost() == "localhost") {
        // ACE does not like localhost.  At all.
        NameConfig config;
        host = config.getHostName(true);
    }
    ACE_INET_Addr addr(address.getPort(), host.c_str());
    YARP_timeval openTimeout;
    YARP_timeval* timeout = nullptr;
    if (address.hasTimeout()) {
        openTimeout.set(address.getTimeout());
        timeout = &openTimeout;
    }
    int result = connector.connect(stream, addr, timeout, ACE_Addr::sap_any, 1);
#else
    int result;

    if (address.hasTimeout()) {
        YARP_timeval timeout;
        /* set timeout seconds and microseconds */
        timeout.tv_sec = (int)address.getTimeout();
        timeout.tv_usec = (address.getTimeout() - (int)address.getTimeout()) * 1000000;
        result = connector.connect(stream, address, &timeout);
    } else {
        result = connector.connect(stream, address, nullptr);
    }


#endif
    if (result >= 0) {
        happy = true;
    } else {
        YARP_SPRINTF2(Logger::get(),
                      debug,
                      "TCP connection to tcp://%s:%d failed to open",
                      host.c_str(),
                      address.getPort());
    }
    updateAddresses();
    return result;
}

int SocketTwoWayStream::open(yarp::os::impl::TcpAcceptor& acceptor)
{
    int result = acceptor.accept(stream);
    if (result >= 0) {
        happy = true;
    }
    updateAddresses();
    return result;
}

void SocketTwoWayStream::updateAddresses()
{
    int one = 1;
    stream.set_option(IPPROTO_TCP, TCP_NODELAY, &one, sizeof(int));
#ifdef YARP_HAS_ACE
    ACE_INET_Addr local;
    ACE_INET_Addr remote;
    stream.get_local_addr(local);
    stream.get_remote_addr(remote);
    char localHostAddress[256];
    char remoteHostAddress[256];
    local.get_host_addr(localHostAddress, 256);
    remote.get_host_addr(remoteHostAddress, 256);
    localAddress = Contact(localHostAddress, local.get_port_number());
    remoteAddress = Contact(remoteHostAddress, remote.get_port_number());
#else
    struct sockaddr local;
    struct sockaddr remote;
    memset(&local, 0, sizeof(local));
    memset(&remote, 0, sizeof(remote));
    stream.get_local_addr(local);
    stream.get_remote_addr(remote);
    if (local.sa_family == AF_INET || local.sa_family == AF_INET6) {
        char* localHostAddress = new char[local.sa_family == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN];
        char* remoteHostAddress = new char[remote.sa_family == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN];
        const char* ret = nullptr;
        ret = inet_ntop(local.sa_family,
                        (local.sa_family == AF_INET ? reinterpret_cast<void*>(&reinterpret_cast<struct sockaddr_in*>(&local)->sin_addr) : reinterpret_cast<void*>(&reinterpret_cast<struct sockaddr_in6*>(&local)->sin6_addr)),
                        localHostAddress,
                        (local.sa_family == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN));
        if (ret) {
            localAddress = Contact(localHostAddress, ntohs(reinterpret_cast<struct sockaddr_in*>(&local)->sin_port));
        } else {
            YARP_ERROR(Logger::get(), "SocketTwoWayStream::updateAddresses failed getting local address");
        }
        ret = inet_ntop(remote.sa_family,
                        (remote.sa_family == AF_INET ? reinterpret_cast<void*>(&reinterpret_cast<struct sockaddr_in*>(&remote)->sin_addr) : reinterpret_cast<void*>(&reinterpret_cast<struct sockaddr_in6*>(&remote)->sin6_addr)),
                        remoteHostAddress,
                        (remote.sa_family == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN));
        if (ret) {
            remoteAddress = Contact(remoteHostAddress, ntohs(reinterpret_cast<struct sockaddr_in*>(&remote)->sin_port));
        } else {
            YARP_ERROR(Logger::get(), "SocketTwoWayStream::updateAddresses failed getting local address");
        }
        delete[] localHostAddress;
        delete[] remoteHostAddress;
    } else {
        YARP_ERROR(Logger::get(), "Unknown address type");
    }
#endif
}

bool SocketTwoWayStream::setTypeOfService(int tos)
{
    return (stream.set_option(IPPROTO_IP, IP_TOS, (int*)&tos, (int)sizeof(tos)) == 0);
}

int SocketTwoWayStream::getTypeOfService()
{
    int tos = -1;
    int optlen;
    stream.get_option(IPPROTO_IP, IP_TOS, (int*)&tos, &optlen);
    return tos;
}
