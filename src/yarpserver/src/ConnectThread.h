// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
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
#include <yarp/os/Semaphore.h>

#include <list>

class ConnectThread : public yarp::os::Thread {
public:
    bool needed;
    bool positive;
    int ct;
    yarp::os::Semaphore& mutex;

    //yarp::os::Contact src;
    yarp::os::ConstString src;
    yarp::os::ConstString dest;

    ConnectThread(yarp::os::Semaphore& mutex) : mutex(mutex) {
        needed = true;
        ct = 0;
        positive = true;
    }

    virtual void run() {
        do {
            mutex.wait();
            if (ct==0) {
                needed = false;
            }
            ct--;
            mutex.post();
            /*
            printf(" ]]] con %s %s / %d %d\n", src.c_str(),
                   dest.c_str(),
                   ct,
                   needed);
            */
            if (!needed) { break; }
            if (positive) {
                if (!yarp::os::NetworkBase::isConnected(src,dest)) {
                    //printf("   (((Trying to connect %s and %s)))\n", 
                    //     src.c_str(),
                    //     dest.c_str());
                    yarp::os::NetworkBase::connect(src,dest);
                }
            } else {
                if (yarp::os::NetworkBase::isConnected(src,dest)) {
                    //printf("   (((Trying to disconnect %s and %s)))\n", 
                    //       src.c_str(),
                    //       dest.c_str());
                    yarp::os::NetworkBase::disconnect(src,dest);
                }
            }
        } while (true);
    }
};


class ConnectManager {
private:
    std::list<ConnectThread *> con;
    yarp::os::Semaphore mutex;
public:

    ConnectManager() : mutex(1) {
    }

    virtual ~ConnectManager() {
        for (std::list<ConnectThread *>::iterator it = con.begin();
             it != con.end(); it++) {
            if ((*it)!=0/*NULL*/) {
                delete (*it);
            }
        }
        con.clear();
    }

    void disconnect(const yarp::os::ConstString& src,
                    const yarp::os::ConstString& dest,
                    bool srcDrop) {
        connect(src,dest,false);
    }

    void connect(const yarp::os::ConstString& src,
                 const yarp::os::ConstString& dest,
                 bool positive = true) {
        //printf("  ??? %s %s\n", src, dest);
        ConnectThread *t = 0/*NULL*/;
        //printf("***** %d threads\n", con.size());
        std::list<ConnectThread *>::iterator it = con.begin();
        bool already = false;
        while (it != con.end()) {
            if ((*it)!=0/*NULL*/) {
                if (!(*it)->needed) {
                    if (t==NULL) {
                        //printf("***** reusing a thread\n");
                        t = (*it);
                        t->stop();
                    } else {
                        //printf("***** deleting a thread\n");
                        (*it)->stop();
                        delete (*it);
                        it = con.erase(it);
                        continue;
                    }
                } else {
                    if ((*it)->src == src && (*it)->dest == dest) {
                        mutex.wait();
                        /*
                        printf("??? prethread %d %d\n", (*it)->needed,
                               (*it)->ct);
                        */
                        if ((*it)->needed) {
                            (*it)->positive = positive;
                            (*it)->ct++;
                            already = true;
                        }
                        mutex.post();
                    }
                }
            }
            it++;
        }
        if (!already) {
            if (t==NULL) {
                t = new ConnectThread(mutex);
                con.push_back(t);
            }
            t->src = src;
            t->dest = dest;
            t->ct = 1;
            t->needed = true;
            t->positive = positive;
            t->start();
        }
    }
};


#endif
