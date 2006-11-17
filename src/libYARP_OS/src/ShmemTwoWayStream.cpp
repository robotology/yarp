// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/ShmemTwoWayStream.h>
#include <yarp/NetType.h>
#include <yarp/os/Time.h>

#include <ace/INET_Addr.h>

// random shmem buffer size
#define MAX_SHMEM_BUFFER (1000000)

using namespace yarp;
using namespace yarp::os;

int ShmemTwoWayStream::open(const Address& address, bool sender) {
    int result = -1;
    happy = false;
    ACE_INET_Addr addr(address.getPort(),address.getName().c_str());
    YARP_DEBUG(Logger::get(),String("trying to open shmem port ") + 
               NetType::toString(address.getPort()));
    if (sender) {
        ACE_MEM_Connector connector;

        // begin configuration option suggested by Giorgio Metta
        connector.preferred_strategy(ACE_MEM_IO::Reactive);
        // end configuration option

        result = connector.connect(stream,addr,0,ACE_Addr::sap_any,1);
        if (result>=0) {
            happy = true;
        } else {
            YARP_ERROR(Logger::get(),"shmem sender connect failed");
            perror("send connect");
        }
        updateAddresses();
        return result;
    } else {
        ACE_MEM_Addr server_addr(address.getPort());
        result = acceptor.open(server_addr,1);
        acceptor.get_local_addr(server_addr);
        localAddress = Address(address.getName(),
                               server_addr.get_port_number());
        remoteAddress = localAddress; // finalized in call to accept()

        if (result>=0) {

        } else {
            YARP_ERROR(Logger::get(),"shmem receiver open failed");
            perror("recv open");
        }
        return result;
    }
}


int ShmemTwoWayStream::accept() {
    int result = -1;

    // begin configuration option suggested by Giorgio Metta
    currentLength = MAX_SHMEM_BUFFER;
    acceptor.init_buffer_size(currentLength);
    acceptor.preferred_strategy(ACE_MEM_IO::Reactive);
    // end configuration option
    
    result = acceptor.accept(stream);
    if (result>=0) {
        happy = true;
    } else {
        YARP_ERROR(Logger::get(),"shmem receiver accept failed");
        perror("recv accept");
    }
    updateAddresses();
    return result;
}


void ShmemTwoWayStream::updateAddresses() {
    ACE_INET_Addr local, remote;
    stream.get_local_addr(local);
    stream.get_remote_addr(remote);
    localAddress = Address(local.get_host_addr(),local.get_port_number());
    remoteAddress = Address(remote.get_host_addr(),remote.get_port_number());
}


void ShmemTwoWayStream::flush() {
    //stream.flush();
}


int ShmemTwoWayStream::read(const Bytes& b) {
    int result = stream.recv_n(b.get(),b.length());
    if (result<=0) {
        happy = false;
        YARP_DEBUG(Logger::get(),"bad socket read");
    }
    return result;
}

void ShmemTwoWayStream::write(const Bytes& b) {
    int result = stream.send_n(b.get(),b.length());
    if (result<0) {
        happy = false;
        YARP_DEBUG(Logger::get(),"bad socket write");
    }
}
