/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SERVERSQL_IMPL_TRIPLESOURCECREATOR_H
#define YARP_SERVERSQL_IMPL_TRIPLESOURCECREATOR_H

#include <yarp/serversql/impl/TripleSource.h>
#include <yarp/conf/compiler.h>


namespace yarp {
namespace serversql {
namespace impl {

/**
 * Open and close a database, viewed as a collection of triples.
 */
class TripleSourceCreator
{
public:
    TripleSourceCreator() = default;

    virtual ~TripleSourceCreator()
    {
        if (implementation != nullptr) {
            close();
        }
    }

    TripleSource *open(const char *filename,
                       bool cautious = false,
                       bool fresh = false);

    bool close();

private:
    void* implementation {nullptr};
    TripleSource* accessor {nullptr};
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_TRIPLESOURCECREATOR_H
