/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SERVERSQL_IMPL_COMPOSEDNAMESERVICE_H
#define YARP_SERVERSQL_IMPL_COMPOSEDNAMESERVICE_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>
#include <yarp/name/NameService.h>


namespace yarp {
namespace serversql {
namespace impl {

/**
 *
 * Compose two name services into one.
 *
 */
class ComposedNameService : public yarp::name::NameService {
public:
    ComposedNameService() {
        ns1 = 0/*NULL*/;
        ns2 = 0/*NULL*/;
    }

    void open(NameService& ns1,NameService& ns2) {
        this->ns1 = &ns1;
        this->ns2 = &ns2;
    }

    virtual bool apply(yarp::os::Bottle& cmd,
                       yarp::os::Bottle& reply,
                       yarp::os::Bottle& event,
                       const yarp::os::Contact& remote) override {
        if (ns1->apply(cmd,reply,event,remote)) {
            return true;
        }
        return ns2->apply(cmd,reply,event,remote);
    }

    virtual void onEvent(yarp::os::Bottle& event) override {
        ns1->onEvent(event);
        ns2->onEvent(event);
    }

    virtual void goPublic() override {
        ns1->goPublic();
        ns2->goPublic();
    }

    virtual yarp::os::Contact query(const yarp::os::ConstString& name) override {
        yarp::os::Contact result;
        result = ns1->query(name);
        if (!result.isValid()) result = ns2->query(name);
        return result;
    }

    virtual bool announce(const yarp::os::ConstString& name, int activity) override {
        if (ns2->announce(name,activity)) return true;
        return ns1->announce(name,activity);
    }

private:
    NameService *ns1;
    NameService *ns2;
};

} // namespace impl
} // namespace serversql
} // namespace yarp

#endif // YARP_SERVERSQL_IMPL_COMPOSEDNAMESERVICE_H
