// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef YARPDB_DBNAMESERVICE_INC
#define YARPDB_DBNAMESERVICE_INC

#include "NameService.h"
#include "TripleSource.h"
#include "Allocator.h"

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
    yarp::os::Contact& remote;
    TripleSource& mem;
    bool bottleMode;
    bool nestedMode;

    NameTripleState(yarp::os::Bottle& cmd, 
                    yarp::os::Bottle& reply, 
                    yarp::os::Bottle& event, 
                    yarp::os::Contact& remote,
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
class NameServiceOnTriples : public NameService {
private:
    TripleSource *db;
    Allocator *alloc;
    std::string lastRegister;
public:
    NameServiceOnTriples(TripleSource *db,
                         Allocator *alloc) : db(db), alloc(alloc) {
        lastRegister = "";
    }

    yarp::os::Contact query(const char *portName, 
                            NameTripleState& act,
                            const char *prefix);

    bool cmdQuery(NameTripleState& act);

    bool cmdRegister(NameTripleState& act);

    bool cmdUnregister(NameTripleState& act);

    bool cmdList(NameTripleState& act);

    bool cmdSet(NameTripleState& act);

    bool cmdGet(NameTripleState& act);

    bool cmdCheck(NameTripleState& act);

    bool cmdRoute(NameTripleState& act);

    bool cmdGc(NameTripleState& act);

    virtual bool apply(yarp::os::Bottle& cmd, 
                       yarp::os::Bottle& reply, 
                       yarp::os::Bottle& event,
                       yarp::os::Contact& remote);
};


#endif
