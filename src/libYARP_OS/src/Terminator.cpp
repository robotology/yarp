// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <yarp/Address.h>
#include <yarp/SocketTwoWayStream.h>
#include <yarp/NameClient.h>

#include <yarp/os/Terminator.h>

using namespace yarp;
using namespace yarp::os;

bool Terminator::terminateByName(const char *name) {
    if (name == NULL)
        return false;
    
    String s(name);
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
    quit = false;
    start();
    ok = true;
}


Terminee::~Terminee() {
    TermineeHelper& helper = HELPER(implementation);
    helper.sock.interrupt();
    helper.acceptor.close();
    stop();

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
