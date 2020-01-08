/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SERVERSQL_IMPL_CONNECTTHREAD_H
#define YARP_SERVERSQL_IMPL_CONNECTTHREAD_H

#include <yarp/os/Thread.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>

#include <list>
#include <mutex>


namespace yarp {
namespace serversql {
namespace impl {

class ConnectThread : public yarp::os::Thread {
public:
    bool needed;
    bool positive;
    int ct;
    std::mutex& mutex;

    //yarp::os::Contact src;
    std::string src;
    std::string dest;

    ConnectThread(std::mutex& mutex) : mutex(mutex) {
        needed = true;
        ct = 0;
        positive = true;
    }

    void run() override {
        do {
            mutex.lock();
            if (ct==0) {
                needed = false;
            }
            ct--;
            mutex.unlock();
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
    std::mutex mutex;
public:

    ConnectManager() : mutex() {
    }

    virtual ~ConnectManager() {
        clear();
    }

    void clear() {
        for (std::list<ConnectThread *>::iterator it = con.begin();
             it != con.end(); it++) {
            if ((*it) != nullptr) {
                delete (*it);
            }
        }
        con.clear();
    }

    void disconnect(const std::string& src,
                    const std::string& dest,
                    bool srcDrop) {
        connect(src,dest,false);
    }

    void connect(const std::string& src,
                 const std::string& dest,
                 bool positive = true) {
        //printf("  ??? %s %s\n", src, dest);
        ConnectThread *t = nullptr;
        //printf("***** %d threads\n", con.size());
        std::list<ConnectThread *>::iterator it = con.begin();
        bool already = false;
        while (it != con.end()) {
            if ((*it) != nullptr) {
                if (!(*it)->needed) {
                    if (t == nullptr) {
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
                        mutex.lock();
                        /*
                        printf("??? prethread %d %d\n", (*it)->needed,
                               (*it)->ct);
                        */
                        if ((*it)->needed) {
                            (*it)->positive = positive;
                            (*it)->ct++;
                            already = true;
                        }
                        mutex.unlock();
                    }
                }
            }
            it++;
        }
        if (!already) {
            if (t == nullptr) {
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

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_CONNECTTHREAD_H
