/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/serversql/impl/ConnectManager.h>

using yarp::serversql::impl::ConnectManager;

ConnectManager::ConnectManager() = default;

ConnectManager::~ConnectManager()
{
    clear();
}

void ConnectManager::clear() {
    for (std::list<ConnectThread *>::iterator it = con.begin();
         it != con.end(); it++) {
        if ((*it) != nullptr) {
            delete (*it);
        }
    }
    con.clear();
}

void ConnectManager::disconnect(const std::string& src,
                                const std::string& dest,
                                bool srcDrop)
{
    connect(src,dest,false);
}

void ConnectManager::connect(const std::string& src,
                             const std::string& dest,
                             bool positive)
{
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
