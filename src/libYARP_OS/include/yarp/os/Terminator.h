// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __TERMINATORH__
#define __TERMINATORH__

// $Id: Terminator.h,v 1.1 2006-08-04 15:42:47 babybot Exp $

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <yarp/Address.h>
#include <yarp/SocketTwoWayStream.h>
#include <yarp/NameClient.h>

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
 * port created using the Terminator class to ask to start
 * a graceful quit procedure. The receiving process must
 * collaborate.
 */
class yarp::os::Terminator {
public:
    static bool terminateByName(const char *name) {
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
class yarp::os::Terminee : public Thread {
protected:
    ACE_INET_Addr addr;
    ACE_SOCK_Acceptor acceptor;
    SocketTwoWayStream sock;
    char data[4];
    bool quit;

public:
    /**
     * Constructor. 
     * @param name is the nickname to register to the name server.
     */
    Terminee(const char *name) {
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

    /**
     * Call this method to see whether a quit message has
     * been received.
     * @return true is a quit has been received, false otherwise.
     */
    bool mustQuit() const { return quit; }
};


#endif