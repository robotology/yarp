/*
 * Copyright (C) 2006 RobotCub Consortium, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/SocketTwoWayStream.h>
#include <yarp/os/impl/NameConfig.h>

#ifdef YARP_HAS_ACE
#  include <ace/INET_Addr.h>
#  include <ace/os_include/netinet/os_tcp.h>
#  include <ace/os_include/os_netdb.h>
#else
#  include <yarp/os/impl/TcpAcceptor.h>
#  include <yarp/os/impl/TcpStream.h>
#  include <yarp/os/impl/TcpConnector.h>
#  include <netinet/tcp.h>
#endif

using namespace yarp::os;
using namespace yarp::os::impl;

int SocketTwoWayStream::open(const Contact& address) {
    if (address.getPort()==-1) {
        return -1;
    }
    ConstString host = address.getHost();
#ifdef YARP_HAS_ACE
    ACE_SOCK_Connector connector;
    if (address.getHost() == "localhost") {
        // ACE does not like localhost.  At all.
        NameConfig config;
        host = config.getHostName(true);
    }
    ACE_INET_Addr addr(address.getPort(),host.c_str());
    ACE_Time_Value openTimeout;
    ACE_Time_Value *timeout = YARP_NULLPTR;
    if (address.hasTimeout()) {
        openTimeout.set(address.getTimeout());
        timeout = &openTimeout;
    }
    int result = connector.connect(stream,addr,timeout,ACE_Addr::sap_any,1);
#else
    TcpConnector connector;
    int result = connector.connect(stream, address);
#endif
    if (result>=0) {
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

int SocketTwoWayStream::open(ACE_SOCK_Acceptor& acceptor) {
    int result = acceptor.accept(stream);
    if (result>=0) {
        happy = true;
    }
    updateAddresses();
    return result;
}

void SocketTwoWayStream::updateAddresses() {
    //int zero = 0;
    int one = 1;

#ifdef YARP_HAS_ACE
    stream.set_option (ACE_IPPROTO_TCP, TCP_NODELAY, &one,
                       sizeof(int));
    ACE_INET_Addr local, remote;
    stream.get_local_addr(local);
    stream.get_remote_addr(remote);
    char localHostAddress[256];
    char remoteHostAddress[256];
    local.get_host_addr(localHostAddress, 256);
    remote.get_host_addr(remoteHostAddress, 256);
    localAddress = Contact(localHostAddress,local.get_port_number());
    remoteAddress = Contact(remoteHostAddress,remote.get_port_number());
#else
    stream.set_option (IPPROTO_TCP, TCP_NODELAY, &one,
                       sizeof(int));
    struct sockaddr local;
    struct sockaddr remote;
    memset(&local, 0, sizeof(local));
    memset(&remote, 0, sizeof(remote));
    stream.get_local_addr(local);
    stream.get_remote_addr(remote);
    if (local.sa_family == AF_INET) {
        struct sockaddr_in *ipv4local = (struct sockaddr_in *) &local;
        struct sockaddr_in *ipv4remote = (struct sockaddr_in *) &remote;
        localAddress = Contact(inet_ntoa(ipv4local->sin_addr),ntohs(ipv4local->sin_port));
        remoteAddress = Contact(inet_ntoa(ipv4remote->sin_addr),ntohs(ipv4remote->sin_port));
    } else {
        YARP_ERROR(Logger::get(),"ipv6 address type not propagated without ACE");
    }
#endif
}

bool SocketTwoWayStream::setTypeOfService(int tos) {
    return (stream.set_option(IPPROTO_IP, IP_TOS,
                              (int *)&tos, (int)sizeof(tos) ) == 0);
}

int SocketTwoWayStream::getTypeOfService() {
    int tos = -1;
    int optlen;
    stream.get_option(IPPROTO_IP, IP_TOS,
                      (int *)&tos, &optlen);
    return tos;
}
