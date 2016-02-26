/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

    AllocatorConfig() {
        minPortNumber = maxPortNumber = -1;
    }
};


/**
 *
 * Abstract interface for allocating network resources.
 *
 */
class Allocator {
public:
    virtual ~Allocator() {}

    virtual yarp::os::Contact completePortName(const yarp::os::Contact& c)=0;

    virtual yarp::os::Contact completeSocket(const yarp::os::Contact& c)=0;

    virtual bool freePortResources(const yarp::os::Contact& c)=0;
};


#endif
