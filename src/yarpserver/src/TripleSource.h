// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPDB_LIVETRIPLE_INC
#define YARPDB_LIVETRIPLE_INC

#include "Triple.h"

#include <string>
#include <list>

/**
 *
 * Side information for controlling access to triples.
 *
 */
class TripleContext {
public:
    int rid;    

    TripleContext() {
        rid = -1;
    }

    void setRid(int rid) {
        this->rid = rid;
    }
};

/**
 *
 * Abstract view of a database as a collection of triples.  These are
 * the minimum functions that need to be implemented in order for the
 * name server to use a particular kind of database.
 *
 */
class TripleSource {
public:
    TripleSource() {
        verbose = 0;
        reset();
    }

    virtual ~TripleSource() {
    }

    virtual void reset() {
        //verbose = 0;
    }

    void setVerbose(int level) {
        verbose = level;
    }

    virtual int find(Triple& t, TripleContext *context) = 0;

    virtual void prune(TripleContext *context) = 0;

    virtual std::list<Triple> query(Triple& ti, 
                                    TripleContext *context) = 0;

    virtual void remove_query(Triple& ti,
                              TripleContext *context) = 0;

    virtual void insert(Triple& t, TripleContext *context) = 0;

    virtual void update(Triple& t, TripleContext *context) = 0;

    virtual void begin(TripleContext *context) = 0;
    virtual void end(TripleContext *context) = 0;
protected:
    int verbose;
};


#endif
