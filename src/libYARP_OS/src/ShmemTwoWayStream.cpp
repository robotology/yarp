// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/ShmemTwoWayStream.h>
#include <yarp/NetType.h>
#include <yarp/IOException.h>
#include <yarp/os/Time.h>

#include <ace/INET_Addr.h>

// shmem buffer size
#define INIT_SHMEM_BUFFER (1000000)

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
        currentLength = INIT_SHMEM_BUFFER;
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
    currentLength = INIT_SHMEM_BUFFER;
    acceptor.init_buffer_size(currentLength*4);
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
    char *base = b.get();
    int remaining = b.length();
    int total = 0;
    while (remaining>0) {
        int len = remaining;
        if (len>INIT_SHMEM_BUFFER) {
            len = INIT_SHMEM_BUFFER;
        }
        int result = stream.recv_n(base,len);
        if (result<=0) {
            happy = false;
            YARP_DEBUG(Logger::get(),"bad socket read");
            total = -1;
            throw IOException("shmem read failed");
            break;
        }
        remaining -= len;
        base += len;
        total += len;
    }

    return total;
}

void ShmemTwoWayStream::write(const Bytes& b) {
    char *base = b.get();
    int remaining = b.length();
    while (remaining>0) {
        int len = remaining;
        if (len>INIT_SHMEM_BUFFER) {
            YARP_ERROR(Logger::get(),
                       "shmem writing a long packet, may be unreliable");
            len = INIT_SHMEM_BUFFER;
        }
        int result = stream.send_n(base,len);
        if (result<0) {
            happy = false;
            YARP_DEBUG(Logger::get(),"bad socket write");
            throw IOException("shmem write failed");
            break;
        }
        remaining -= len;
        base += len;
    }
}



void ShmemTwoWayStream::reset() {
}

void ShmemTwoWayStream::beginPacket() { 
}

void ShmemTwoWayStream::endPacket() { 
}
