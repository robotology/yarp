/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2006 Anne van Rossum <anne@almende.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/SocketTwoWayStream.h>

#include <yarp/os/impl/LogComponent.h>
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
#elif defined(__unix__) || defined(__APPLE__)
#    include <netinet/tcp.h>
#endif

using namespace yarp::os;
using namespace yarp::os::impl;

YARP_OS_LOG_COMPONENT(SOCKETTWOWAYSTREAM, "yarp.os.impl.SocketTwoWayStream")

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
        host = yarp::os::impl::NameConfig::getHostName(true);
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
        timeout.tv_sec = static_cast<int>(address.getTimeout());
        timeout.tv_usec = (address.getTimeout() - timeout.tv_sec) * 1000000;
        result = connector.connect(stream, address, &timeout);
    } else {
        result = connector.connect(stream, address, nullptr);
    }


#endif
    if (result >= 0) {
        happy = true;
    } else {
        yCDebug(SOCKETTWOWAYSTREAM,
                "TCP connection to tcp:/%s failed to open",
                address.toURI(false).c_str());
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
            yCError(SOCKETTWOWAYSTREAM, "SocketTwoWayStream::updateAddresses failed getting local address");
        }
        ret = inet_ntop(remote.sa_family,
                        (remote.sa_family == AF_INET ? reinterpret_cast<void*>(&reinterpret_cast<struct sockaddr_in*>(&remote)->sin_addr) : reinterpret_cast<void*>(&reinterpret_cast<struct sockaddr_in6*>(&remote)->sin6_addr)),
                        remoteHostAddress,
                        (remote.sa_family == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN));
        if (ret) {
            remoteAddress = Contact(remoteHostAddress, ntohs(reinterpret_cast<struct sockaddr_in*>(&remote)->sin_port));
        } else {
            yCError(SOCKETTWOWAYSTREAM, "SocketTwoWayStream::updateAddresses failed getting local address");
        }
        delete[] localHostAddress;
        delete[] remoteHostAddress;
    } else {
        yCError(SOCKETTWOWAYSTREAM, "Unknown address type");
    }
#endif

    yCDebug(SOCKETTWOWAYSTREAM, "updateAddresses: local address = %s", localAddress.getHost().c_str());
    yCDebug(SOCKETTWOWAYSTREAM, "updateAddresses: remote address = %s", remoteAddress.getHost().c_str());
}

bool SocketTwoWayStream::setTypeOfService(int tos)
{
    yCDebug(SOCKETTWOWAYSTREAM, "Setting tos = %d", tos);
    return (stream.set_option(IPPROTO_IP, IP_TOS, &tos, static_cast<int>(sizeof(tos))) == 0);
}

int SocketTwoWayStream::getTypeOfService()
{
    // tos is the "Type Of Service" byte of the IP header of the packets sent
    // through this socket. Its upper 6 bits are the DSCP (Differentiated
    // Services Code Point) used by the network to prioritize the traffic,
    // thus tos = DSCP * 4 (e.g. NORMAL = 0, LOW = 40, HIGH = 144, CRITICAL = 176).
    // -1 is returned if the value cannot be read.
    int tos = -1;
    // optlen is both an input and an output parameter of getsockopt():
    // on input it must contain the size of the buffer pointed by &tos,
    // on output it contains the number of bytes actually written.
    // If it is not initialized, getsockopt() may write nothing (optlen == 0)
    // or fail with EINVAL (optlen < 0), leaving tos to -1.
    int optlen = static_cast<int>(sizeof(tos));
    if (stream.get_option(IPPROTO_IP, IP_TOS, &tos, &optlen) != 0) {
        return -1;
    }
    return tos;
}
