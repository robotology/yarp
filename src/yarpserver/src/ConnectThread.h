// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPDB_CONNECTTHREAD_INC
#define YARPDB_CONNECTTHREAD_INC

#include <yarp/os/Thread.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>

#include <list>

class ConnectThread : public yarp::os::Thread {
public:
    yarp::os::Contact src;
    yarp::os::ConstString dest;

    virtual void run() {
        yarp::os::Bottle cmd, reply;
        cmd.addVocab(VOCAB3('a','d','d'));
        cmd.addString(dest.c_str());
        yarp::os::Network::write(src,
                                 cmd,
                                 reply,
                                 true);
    }
};


class ConnectManager {
private:
    std::list<ConnectThread *> con;
public:

    virtual ~ConnectManager() {
        for (std::list<ConnectThread *>::iterator it = con.begin();
             it != con.end(); it++) {
            if ((*it)!=0/*NULL*/) {
                delete (*it);
            }
        }
        con.clear();
    }

    void connect(const yarp::os::Contact& src,
                 const yarp::os::ConstString& dest) {
        ConnectThread *t = 0/*NULL*/;
        for (std::list<ConnectThread *>::iterator it = con.begin();
             it != con.end(); it++) {
            if ((*it)!=0/*NULL*/) {
                if (!(*it)->isRunning()) {
                    t = (*it);
                    t->stop();
                    break;
                }
            }
        }
        if (t==NULL) {
            t = new ConnectThread();
            con.push_back(t);
        }
        t->src = src;
        t->dest = dest;
        t->start();
    }
};


#endif

