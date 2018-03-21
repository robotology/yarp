/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARPDB_NAMESERVERMANAGER_INC
#define YARPDB_NAMESERVERMANAGER_INC

#include <yarp/name/NameServerConnectionHandler.h>

#include <yarp/os/PortReaderCreator.h>
#include <yarp/os/Semaphore.h>
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
    yarp::os::Semaphore mutex;
public:
    NameServerManager(NameService& ns,
                      yarp::os::Port *port = NULL) : ns(ns),
                                                     port(port), mutex(1) {
    }

    void setPort(yarp::os::Port& port) {
        this->port = &port;
    }

    virtual void onEvent(yarp::os::Bottle& event) override {
        ns.onEvent(event);
        if (port!=NULL) {
            port->write(event);
        }
    }

    virtual yarp::os::PortReader *create() override {
        return new NameServerConnectionHandler(this);
    }

    virtual void lock() override {
        mutex.wait();
    }

    virtual void unlock() override {
        mutex.post();
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
