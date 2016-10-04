/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef YARP_SERVERSQL_IMPL_TRIPLESOURCECREATOR_H
#define YARP_SERVERSQL_IMPL_TRIPLESOURCECREATOR_H

#include <yarp/serversql/impl/TripleSource.h>
#include <yarp/conf/compiler.h>


namespace yarp {
namespace serversql {
namespace impl {

/**
 *
 * Open and close a database, viewed as a collection of triples.
 *
 */
class TripleSourceCreator {
public:
    TripleSourceCreator() :
            implementation(YARP_NULLPTR),
            accessor(YARP_NULLPTR) {
    }

    virtual ~TripleSourceCreator() {
        if (implementation != YARP_NULLPTR) {
            close();
        }
    }

    TripleSource *open(const char *filename,
                       bool cautious = false,
                       bool fresh = false);

    bool close();

private:
    void *implementation;
    TripleSource *accessor;
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_TRIPLESOURCECREATOR_H
