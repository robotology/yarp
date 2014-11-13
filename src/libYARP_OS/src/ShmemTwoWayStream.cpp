// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


// this whole class removed
// not needed anymore, and triggers a strange ACE static object issue
// during compiles on certain Fedora systems.

#if 0

#include <yarp/os/impl/ShmemTwoWayStream.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Time.h>

#include <ace/INET_Addr.h>

// shmem buffer size
#define INIT_SHMEM_BUFFER (1000000)

using namespace yarp::os::impl;
using namespace yarp::os;

int ShmemTwoWayStream::open(const Address& address, bool sender) {
    int result = -1;
    happy = false;
    ACE_INET_Addr addr(address.getPort(),address.getHost().c_str());
    YARP_DEBUG(Logger::get(),String("trying to open shmem port ") +
               NetType::toString(address.getPort()));
    if (sender) {
        ACE_MEM_Connector connector;

        // begin configuration option suggested by Giorgio Metta
        connector.preferred_strategy(ACE_MEM_IO::Reactive);
        // end configuration option

        result = connector.connect(stream,addr,0,ACE_Addr::sap_any,1);
        if (result>=0) {
            YARP_DEBUG(Logger::get(),"shmem sender connect succeeded");
            happy = true;
        } else {
            YARP_ERROR(Logger::get(),"*** Shared memory connection failed");
            // perror doesn't seem to get set in any useful way, omit it
            // perror("send connect");
            YARP_ERROR(Logger::get(),"Shared memory connections can fail due to a network configuration problem.");
            YARP_ERROR(Logger::get(),"There is a check to make sure shared memory connections are local,");
            YARP_ERROR(Logger::get(),"and this check relies on your hostname being configured well.");
            YARP_ERROR(Logger::get(),"Possible fixes:");
            YARP_ERROR(Logger::get(),"   - On linux, edit /etc/hosts to link 127.0.0.1 with your hostname");
            YARP_ERROR(Logger::get(),"   - (you can find out your hostname with the \"hostname\" command)");
            YARP_ERROR(Logger::get(),"   - On a windows laptop, make sure you have a network card attached");
            YARP_ERROR(Logger::get(),"For up to date information, see mailing list linked on yarp0.sf.net");
        }
        currentLength = INIT_SHMEM_BUFFER;
        updateAddresses();
        return result;
    } else {
        ACE_MEM_Addr server_addr(address.getPort());
        result = acceptor.open(server_addr,1);
        acceptor.get_local_addr(server_addr);
        localAddress = Address(address.getHost(),
                               server_addr.get_port_number());
        remoteAddress = localAddress; // finalized in call to accept()

        if (result>=0) {
            YARP_DEBUG(Logger::get(),"shmem receiver open succeeded");
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
        YARP_DEBUG(Logger::get(),"shmem receiver accept succeeded");
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
            YARP_ERROR(Logger::get(),"shmem read failed");
            return -1;
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
            YARP_ERROR(Logger::get(),"shmem write failed");
            return;
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

#endif
