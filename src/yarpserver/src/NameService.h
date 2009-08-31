// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef YARPDB_NAMESERVICE_INC
#define YARPDB_NAMESERVICE_INC

#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>

/**
 *
 * Abstract interface for a name server operator.
 *
 */
class NameService {
public:
    virtual bool apply(yarp::os::Bottle& cmd, 
                       yarp::os::Bottle& reply, 
                       yarp::os::Bottle& event,
                       yarp::os::Contact& remote) = 0;

    virtual void onEvent(yarp::os::Bottle& event) {}

    virtual void lock() {}
    virtual void unlock() {}
};

/**
 *
 * Abstract interface for a database of port names.
 *
 */
class NameStore {
public:
    virtual ~NameStore() {}
    virtual yarp::os::Contact query(const char *name) = 0;
};

#endif
