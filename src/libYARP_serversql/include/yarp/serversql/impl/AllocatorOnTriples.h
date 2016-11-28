/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_SERVERSQL_IMPL_ALLOCATORONTRIPLES_H
#define YARP_SERVERSQL_IMPL_ALLOCATORONTRIPLES_H

#include <yarp/serversql/impl/Allocator.h>
#include <yarp/serversql/impl/TripleSource.h>

namespace yarp {
namespace serversql {
namespace impl {

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
        db = YARP_NULLPTR;
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

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_ALLOCATORONTRIPLES_H
