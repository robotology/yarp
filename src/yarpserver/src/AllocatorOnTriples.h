// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef YARPDB_ALLOCATORONTRIPLES_INC
#define YARPDB_ALLOCATORONTRIPLES_INC

#include "Allocator.h"
#include "TripleSource.h"

/**
 *
 * Allocates network resources, and tracks them using a TripleSource.
 *
 */
class AllocatorOnTriples : public Allocator {
public:
    AllocatorOnTriples(TripleSource *db, const AllocatorConfig& config) : db(db), config(config) {
        regid = -1;
        tmpid = -1;
        mcastCursor = -1;
    }

    virtual yarp::os::Contact completePortName(const yarp::os::Contact& c);

    virtual yarp::os::Contact completeSocket(const yarp::os::Contact& c);

    yarp::os::Contact completePortNumber(const yarp::os::Contact& c);

    yarp::os::Contact completeHost(const yarp::os::Contact& c);

    virtual bool freePortResources(const yarp::os::Contact& c);

private:

    int regid;
    int tmpid;
    int mcastCursor;
    TripleSource *db;
    AllocatorConfig config;
};


#endif
