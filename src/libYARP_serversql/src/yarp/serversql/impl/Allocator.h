/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SERVERSQL_IMPL_ALLOCATOR_H
#define YARP_SERVERSQL_IMPL_ALLOCATOR_H

#include <yarp/os/Contact.h>


namespace yarp {
namespace serversql {
namespace impl {

/**
 * Upper and lower bounds on port numbers etc for allocation.
 */
class AllocatorConfig
{
public:
    int minPortNumber;
    int maxPortNumber;

    AllocatorConfig()
    {
        minPortNumber = maxPortNumber = -1;
    }
};


/**
 *
 * Abstract interface for allocating network resources.
 *
 */
class Allocator
{
public:
    virtual ~Allocator() = default;

    virtual yarp::os::Contact completePortName(const yarp::os::Contact& c) = 0;

    virtual yarp::os::Contact completeSocket(const yarp::os::Contact& c) = 0;

    virtual bool freePortResources(const yarp::os::Contact& c) = 0;
};

} // namespace impl
} // namespace serversql
} // namespace yarp

#endif // YARP_SERVERSQL_IMPL_ALLOCATOR_H
