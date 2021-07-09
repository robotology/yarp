/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARPDB_NAMESERVERMANAGER_INC
#define YARPDB_NAMESERVERMANAGER_INC

#include <yarp/name/NameServerConnectionHandler.h>

#include <yarp/os/PortReaderCreator.h>
#include <mutex>
#include <yarp/os/Port.h>


namespace yarp {
    namespace name {
        class NameServerManager;
    }
}

/**
 *
 * Manage the name server.
 *
 */
class yarp::name::NameServerManager : public NameService,
                                      public yarp::os::PortReaderCreator {
private:
    NameService& ns;
    yarp::os::Port *port;
    std::mutex mutex;
public:
    NameServerManager(NameService& ns,
                      yarp::os::Port *port = NULL) : ns(ns),
                                                     port(port), mutex() {
    }

    void setPort(yarp::os::Port& port) {
        this->port = &port;
    }

    void onEvent(yarp::os::Bottle& event) override {
        ns.onEvent(event);
        if (port!=NULL) {
            port->write(event);
        }
    }

    yarp::os::PortReader *create() const override {
        return new NameServerConnectionHandler(const_cast<NameServerManager*>(this));
    }

    void lock() override {
        mutex.lock();
    }

    void unlock() override {
        mutex.unlock();
    }

    virtual bool apply(yarp::os::Bottle& cmd,
                       yarp::os::Bottle& reply,
                       yarp::os::Bottle& event,
                       const yarp::os::Contact& remote) override {
        bool ok = ns.apply(cmd,reply,event,remote);
        return ok;
    }
};

#endif
