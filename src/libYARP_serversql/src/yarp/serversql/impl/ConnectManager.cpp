/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/serversql/impl/ConnectManager.h>
#include <yarp/serversql/impl/LogComponent.h>

using yarp::serversql::impl::ConnectManager;

namespace {
YARP_SERVERSQL_LOG_COMPONENT(CONNECTMANAGER, "yarp.serversql.impl.ConnectManager")
} // namespace

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
    yCTrace(CONNECTMANAGER, "  ??? %s %s", src.c_str(), dest.c_str());
    ConnectThread *t = nullptr;
    yCTrace(CONNECTMANAGER, "***** %zd threads", con.size());
    std::list<ConnectThread *>::iterator it = con.begin();
    bool already = false;
    while (it != con.end()) {
        if ((*it) != nullptr) {
            if (!(*it)->needed) {
                if (t == nullptr) {
                    yCTrace(CONNECTMANAGER, "***** reusing a thread");
                    t = (*it);
                    t->stop();
                } else {
                    yCTrace(CONNECTMANAGER, "***** deleting a thread");
                    (*it)->stop();
                    delete (*it);
                    it = con.erase(it);
                    continue;
                }
            } else {
                if ((*it)->src == src && (*it)->dest == dest) {
                    mutex.lock();
                    yCTrace(CONNECTMANAGER,
                            "??? prethread %d %d",
                            (*it)->needed,
                            (*it)->ct);
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
