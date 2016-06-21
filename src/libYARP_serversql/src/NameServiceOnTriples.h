/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPDB_DBNAMESERVICE_INC
#define YARPDB_DBNAMESERVICE_INC

#include <yarp/name/NameService.h>
#include "TripleSource.h"
#include "Allocator.h"
#include "Subscriber.h"
#include <yarp/os/NameStore.h>
#include <yarp/os/Semaphore.h>

/**
 *
 * State information for a single name server operation on a database.
 *
 */
class NameTripleState {
public:
    yarp::os::Bottle& cmd;
    yarp::os::Bottle& reply;
    yarp::os::Bottle& event;
    const yarp::os::Contact& remote;
    TripleSource& mem;
    bool bottleMode;
    bool nestedMode;

    NameTripleState(yarp::os::Bottle& cmd, 
                    yarp::os::Bottle& reply, 
                    yarp::os::Bottle& event, 
                    const yarp::os::Contact& remote,
                    TripleSource& mem) : cmd(cmd), 
                                         reply(reply), 
                                         event(event), 
                                         remote(remote),
                                         mem(mem)
    {
        bottleMode = false;
        nestedMode = false;
    }
};

/**
 *
 * An implementation of name service operators on a triple store.
 *
 */
class NameServiceOnTriples : public yarp::name::NameService {
private:
    TripleSource *db;
    Allocator *alloc;
    Subscriber *subscriber;
    std::string lastRegister;
    yarp::os::Contact serverContact;
    yarp::os::Semaphore mutex, access;
    bool gonePublic;
    bool silent;
    yarp::os::NameSpace *delegate;
public:
    NameServiceOnTriples() : mutex(1), access(1) {
        db = 0 /*NULL*/;
        alloc = 0 /*NULL*/;
        lastRegister = "";
        subscriber = NULL;
        gonePublic = false;
        silent = false;
        delegate = 0 /*NULL*/;
    }

    void open(TripleSource *db,
              Allocator *alloc,
              const yarp::os::Contact& serverContact) {
        this->db = db;
        this->alloc = alloc;
        this->serverContact = serverContact;
    }

    void setSubscriber(Subscriber *subscriber) {
        this->subscriber = subscriber;
    }

    void setSilent(bool flag) {
        this->silent = flag;
    }

    yarp::os::Contact query(const yarp::os::ConstString& portName, 
                            NameTripleState& act,
                            const yarp::os::ConstString& prefix,
                            bool nested = false);

    virtual bool announce(const yarp::os::ConstString& name, int activity);

    virtual yarp::os::Contact query(const yarp::os::ConstString& portName);

    bool cmdQuery(NameTripleState& act, bool nested = false);

    bool cmdRegister(NameTripleState& act);

    bool cmdUnregister(NameTripleState& act);

    bool cmdList(NameTripleState& act);

    bool cmdSet(NameTripleState& act);

    bool cmdGet(NameTripleState& act);

    bool cmdCheck(NameTripleState& act);

    bool cmdRoute(NameTripleState& act);

    bool cmdGc(NameTripleState& act);

    bool cmdHelp(NameTripleState& act);

    virtual bool apply(yarp::os::Bottle& cmd, 
                       yarp::os::Bottle& reply, 
                       yarp::os::Bottle& event,
                       const yarp::os::Contact& remote);

    virtual void goPublic() {
        gonePublic = true;
    }

    void lock();

    void unlock();

    void setDelegate(yarp::os::NameSpace *delegate) {
        this->delegate = delegate;
    }

    yarp::os::NameSpace *getDelegate() {
        return delegate;
    }
};


#endif
