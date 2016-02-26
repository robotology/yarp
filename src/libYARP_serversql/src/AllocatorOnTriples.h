/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
    AllocatorOnTriples() {
        regid = -1;
        tmpid = -1;
        mcastCursor = -1;
        db = 0 /*NULL*/;
    }

    void open(TripleSource *db, const AllocatorConfig& config) {
        this->db = db;
        this->config = config;
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
