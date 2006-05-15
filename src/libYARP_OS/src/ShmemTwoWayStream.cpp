// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/ShmemTwoWayStream.h>
#include <yarp/NetType.h>
#include <yarp/os/Time.h>

#include <ace/INET_Addr.h>

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
        result = connector.connect(stream,addr);
        if (result>=0) {
            happy = true;
        } else {
            YARP_DEBUG(Logger::get(),"shmem receiver connect failed");
        }
        updateAddresses();
        return result;
    } else {
        ACE_MEM_Acceptor acceptor;
        ACE_MEM_Addr server_addr(address.getPort());
        result = acceptor.open(server_addr,1);
        if (result>=0) {
            result = acceptor.accept(stream);
            if (result>=0) {
                happy = true;
            } else {
                YARP_DEBUG(Logger::get(),"shmem sender accept failed");
            }
            updateAddresses();
        } else {
            YARP_DEBUG(Logger::get(),"shmem sender open failed");
        }
        return result;
    }
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
