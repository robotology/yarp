// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __TERMINATORH__
#define __TERMINATORH__

// $Id: Terminator.h,v 1.4 2006-08-04 22:38:06 gmetta Exp $

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <yarp/Address.h>
#include <yarp/SocketTwoWayStream.h>
#include <yarp/NameClient.h>
#include <yarp/os/Thread.h>

namespace yarp {
    namespace os {
        class Terminator;
        class Terminee;
    }
}

/**
 * \file Terminator.h classes to handle graceful process termination.
 */

/**
 * Send a message to a process that has a corresponding
 * socket port created using the Terminee class to ask to start
 * a graceful quit procedure. The receiving process must
 * collaborate and take appropriate action after receiving the
 * message. The Terminator/Terminee class pair start a tcp socket on the 
 * server side listening for incoming connections and the magin word "quit" 
 * (all lowercase). The ip-port pair is registered with a symbolic name
 * on the Yarp name server and can be queried by the client to send the 
 * termination message.
 */
class yarp::os::Terminator {
public:
    /** 
     * Send a quit message to a specific socket port.
     * @param name is the name of the socket port (as registered in 
     * the name server).
     * @return true/false on success/failure.
     */
    static bool terminateByName(const char *name) {
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
        SocketTwoWayStream sock;
        sock.open(a);
        sock.write(Bytes("quit",4));
        sock.close();
        return true;
    }
};

/**
 * A class that can be polled to see whether the process has been 
 * asked to quit gracefully.
 */ 
class yarp::os::Terminee : public yarp::os::Thread {
protected:
    ACE_INET_Addr addr;
    ACE_SOCK_Acceptor acceptor;
    SocketTwoWayStream sock;
    char data[4];
    volatile bool quit;
    volatile bool ok;

public:
    /**
     * Constructor. 
     * @param name is the nickname to register on the name server.
     */
    Terminee(const char *name) {
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
        addr.set(a.getPort());
        acceptor.open(addr);
        quit = false;
        start();
        ok = true;
    }

    /**
     * Destructor.
     */
    virtual ~Terminee() {
        sock.interrupt();
        acceptor.close();
        stop();
    }

    virtual void run() {
        while (!isStopping() && !quit) {
            sock.open(acceptor);
    
            sock.read(Bytes(data,4));
            if (data[0] == 'q' &&
                data[1] == 'u' &&
                data[2] == 'i' &&
                data[3] == 't') {
            quit = true;
            sock.interrupt();
            acceptor.close();
            }
        }
    }

    // LATER: mustQuitBlocking() to wait on a semaphore.

    /**
     * Call this method to see whether a quit message has
     * been received.
     * @return true is a quit has been received, false otherwise.
     */
    bool mustQuit() const { return quit; }

    /**
     * Check whether the message mechanism is ok.
     * @return true if ok, false otherwise.
     */
    bool isOk() const { return ok; }
};


#endif