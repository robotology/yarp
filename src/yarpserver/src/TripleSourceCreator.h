// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#ifndef YARPDB_NAMEDATABASE_INC
#define YARPDB_NAMEDATABASE_INC

#include "TripleSource.h"

/**
 *
 * Open and close a database, viewed as a collection of triples.
 *
 */
class TripleSourceCreator {
public:
    TripleSourceCreator() {
        implementation = NULL;
        accessor = NULL;
    }

    virtual ~TripleSourceCreator() {
        if (implementation!=NULL) {
            close();
        }
    }

    TripleSource *open(const char *filename, bool fresh = false);

    bool close();

private:
    void *implementation;
    TripleSource *accessor;
};


#endif
