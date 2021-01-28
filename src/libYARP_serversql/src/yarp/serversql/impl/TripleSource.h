/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SERVERSQL_IMPL_TRIPLESOURCE_H
#define YARP_SERVERSQL_IMPL_TRIPLESOURCE_H

#include <yarp/serversql/impl/Triple.h>

#include <string>
#include <list>


namespace yarp {
namespace serversql {
namespace impl {

/**
 * Side information for controlling access to triples.
 */
class TripleContext
{
public:
    int rid {-1};

    TripleContext()  = default;

    void setRid(int rid) {
        this->rid = rid;
    }
};

/**
 * Abstract view of a database as a collection of triples.  These are
 * the minimum functions that need to be implemented in order for the
 * name server to use a particular kind of database.
 */
class TripleSource
{
public:
    TripleSource()
    {
        reset();
    }

    virtual ~TripleSource() = default;

    virtual void reset()
    {
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
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_TRIPLESOURCE_H
