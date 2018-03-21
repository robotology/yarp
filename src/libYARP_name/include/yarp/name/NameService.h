/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARPDB_NAMESERVICE_INC
#define YARPDB_NAMESERVICE_INC

#include <yarp/name/api.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>
#include <yarp/os/NameStore.h>

namespace yarp {
    /**
     *
     * Classes for constructing name servers.  While YARP comes with
     * a standard, centralized server for mapping port names to
     * resources, there are plenty of alternatives in how this could
     * be done.  This namespace collects classes that are of use for
     * creating alternative name servers (or avoiding name servers
     * altogether).
     *
     */
    namespace name {
        class NameService;
    }
}

/**
 *
 * Abstract interface for a name server operator.
 *
 */
class YARP_name_API yarp::name::NameService : public yarp::os::NameStore {
public:
    virtual ~NameService() {}

    virtual bool apply(yarp::os::Bottle& cmd,
                       yarp::os::Bottle& reply,
                       yarp::os::Bottle& event,
                       const yarp::os::Contact& remote) = 0;

    virtual void onEvent(yarp::os::Bottle& event) {}

    virtual void lock() {}
    virtual void unlock() {}

    virtual void goPublic() {}

    virtual yarp::os::Contact query(const yarp::os::ConstString& name) override {
        return yarp::os::Contact();
    }

    virtual bool announce(const yarp::os::ConstString& name, int activity) override {
        return false;
    }

    virtual bool process(yarp::os::PortWriter& in,
                         yarp::os::PortReader& out,
                         const yarp::os::Contact& remote) override;
};


#endif
