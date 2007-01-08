// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <yarp/Address.h>
#include <yarp/SocketTwoWayStream.h>
#include <yarp/NameClient.h>
#include <yarp/Companion.h>
#include <yarp/PortCommand.h>

#include <yarp/os/Terminator.h>
#include <yarp/os/Network.h>


using namespace yarp;
using namespace yarp::os;

bool Terminator::terminateByName(const char *name) {
    if (name == NULL)
        return false;

    String s(name);

    if (s.strstr("/quit")<0) {
        // name doesn't include /quit
        // old mechanism won't work, let's try new
        PortCommand pc('\0',"i");
        Companion::sendMessage(s,pc,true);
        return true;
    }

    if (name[0] != '/') {
        s.clear();
        s += "/";
        s += name;
    }
    NameClient& namer = NameClient::getNameClient();
    Address a = namer.queryName(s);
    //ACE_OS::printf("address: %s port %d\n",a.getName().c_str(),a.getPort());
    SocketTwoWayStream sock;
    sock.open(a);
    sock.write(Bytes("quit",4));
    sock.close();
    return true;
}





class TermineeHelper {
public:
    ACE_INET_Addr addr;
    ACE_SOCK_Acceptor acceptor;
    SocketTwoWayStream sock;
    char data[4];
    String registeredName;
};
#define HELPER(x) (*((TermineeHelper*)(x)))

Terminee::Terminee(const char *name) {
    ok = false;
    if (name == NULL) {
        ACE_OS::printf("Terminator: Please supply a proper port name\n");
        return;
    }

    String s(name);
    if (name[0] != '/') {
        s.clear();
        s += "/";
        s += name;
    }
    
    NameClient& namer = NameClient::getNameClient();
    Address a = namer.registerName(s);
	//ACE_OS::printf("listening address: %s port %d\n",a.getName().c_str(),a.getPort());

    implementation = new TermineeHelper;
    YARP_ASSERT(implementation!=NULL);
    TermineeHelper& helper = HELPER(implementation);

    helper.addr.set(a.getPort());
    helper.acceptor.open(helper.addr, 1);
    helper.registeredName = name;
    quit = false;
    start();
    ok = true;
}


Terminee::~Terminee() {
    TermineeHelper& helper = HELPER(implementation);
    if (!quit) {
        Terminator::terminateByName(helper.registeredName.c_str());
    }
    helper.sock.interrupt();
    helper.acceptor.close();
    stop();
    Network::unregisterName(helper.registeredName.c_str());

    if (implementation!=NULL) {
        delete &HELPER(implementation);
    }
}


void Terminee::run() {
    TermineeHelper& helper = HELPER(implementation);
    while (!isStopping() && !quit) {
        helper.sock.open(helper.acceptor);
        Address a = helper.sock.getRemoteAddress();
        //ACE_OS::printf("incoming data: %s %d\n", a.getName().c_str(), a.getPort());
        helper.sock.read(Bytes(helper.data,4));
        if (helper.data[0] == 'q' &&
            helper.data[1] == 'u' &&
            helper.data[2] == 'i' &&
            helper.data[3] == 't') {
            quit = true;
            helper.sock.interrupt();
            helper.acceptor.close();
        }
    }
}
