// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/impl/SocketTwoWayStream.h>

#include <ace/INET_Addr.h>
#include <ace/OS.h>
#include <ace/os_include/netinet/os_tcp.h>
#include <ace/os_include/os_netdb.h>


using namespace yarp::os::impl;

int SocketTwoWayStream::open(const Address& address) {
    if (address.getPort()==-1) {
        return -1;
    }
    ACE_SOCK_Connector connector;
    ACE_INET_Addr addr(address.getPort(),address.getName().c_str());
    int result = connector.connect(stream,addr,0,ACE_Addr::sap_any,1);
    if (result>=0) {
        happy = true;
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
    int zero = 0;
    int one = 1;
    stream.set_option (ACE_IPPROTO_TCP, TCP_NODELAY, &one,
                       sizeof(int));
    struct linger lval;
    lval.l_onoff = 0;
    lval.l_linger = 0;
    stream.set_option (ACE_IPPROTO_TCP, SO_LINGER, &lval,
                       sizeof(linger));
    ACE_INET_Addr local, remote;
    stream.get_local_addr(local);
    stream.get_remote_addr(remote);
    localAddress = Address(local.get_host_addr(),local.get_port_number());
    remoteAddress = Address(remote.get_host_addr(),remote.get_port_number());
}

