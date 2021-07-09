/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SERVERSQL_IMPL_ALLOCATORONTRIPLES_H
#define YARP_SERVERSQL_IMPL_ALLOCATORONTRIPLES_H

#include <yarp/serversql/impl/Allocator.h>
#include <yarp/serversql/impl/TripleSource.h>

namespace yarp {
namespace serversql {
namespace impl {

/**
 * Allocates network resources, and tracks them using a TripleSource.
 */
class AllocatorOnTriples : public Allocator
{
public:
    AllocatorOnTriples() {
        regid = -1;
        tmpid = -1;
        mcastCursor = -1;
        db = nullptr;
    }

    void open(TripleSource *db, const AllocatorConfig& config) {
        this->db = db;
        this->config = config;
        regid = -1;
        tmpid = -1;
        mcastCursor = -1;
    }

    yarp::os::Contact completePortName(const yarp::os::Contact& c) override;

    yarp::os::Contact completeSocket(const yarp::os::Contact& c) override;

    yarp::os::Contact completePortNumber(const yarp::os::Contact& c);

    yarp::os::Contact completeHost(const yarp::os::Contact& c);

    bool freePortResources(const yarp::os::Contact& c) override;

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
