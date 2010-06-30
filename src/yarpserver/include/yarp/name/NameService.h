// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPDB_NAMESERVICE_INC
#define YARPDB_NAMESERVICE_INC

#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>

namespace yarp {
    namespace name {
        class NameService;
    }
}

/**
 *
 * Abstract interface for a name server operator.
 *
 */
class yarp::name::NameService {
public:
    virtual ~NameService() {}

    virtual bool apply(yarp::os::Bottle& cmd, 
                       yarp::os::Bottle& reply, 
                       yarp::os::Bottle& event,
                       yarp::os::Contact& remote) = 0;

    virtual void onEvent(yarp::os::Bottle& event) {}

    virtual void lock() {}
    virtual void unlock() {}

    virtual void goPublic() {}
};


#endif
