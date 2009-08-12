// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef YARPDB_ALLOCATOR_INC
#define YARPDB_ALLOCATOR_INC

#include <yarp/os/Contact.h>


/**
 *
 * Upper and lower bounds on port numbers etc for allocation.
 *
 */
class AllocatorConfig {
public:
    int minPortNumber;
    int maxPortNumber;
};


/**
 *
 * Abstract interface for allocating network resources.
 *
 */
class Allocator {
public:
    virtual yarp::os::Contact completePortName(const yarp::os::Contact& c)=0;

    virtual yarp::os::Contact completeSocket(const yarp::os::Contact& c)=0;

    virtual bool freePortResources(const yarp::os::Contact& c)=0;
};


#endif
