/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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

    virtual void onEvent(yarp::os::Bottle& event) YARP_OVERRIDE {
        ns.onEvent(event);
        if (port!=NULL) {
            port->write(event);
        }
    }

    virtual yarp::os::PortReader *create() YARP_OVERRIDE {
        return new NameServerConnectionHandler(this);
    }

    virtual void lock() YARP_OVERRIDE {
        mutex.wait();
    }

    virtual void unlock() YARP_OVERRIDE {
        mutex.post();
    }

    virtual bool apply(yarp::os::Bottle& cmd,
                       yarp::os::Bottle& reply,
                       yarp::os::Bottle& event,
                       const yarp::os::Contact& remote) YARP_OVERRIDE {
        bool ok = ns.apply(cmd,reply,event,remote);
        return ok;
    }
};

#endif
