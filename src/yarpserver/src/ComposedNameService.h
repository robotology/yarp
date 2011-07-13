// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPDB_COMPOSEDNAMESERVICE_INC
#define YARPDB_COMPOSEDNAMESERVICE_INC

#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>
#include <yarp/name/NameService.h>

namespace yarp {
    namespace name {
        class ComposedNameService;
    }
}

/**
 *
 * Compose two name services into one.
 *
 */
class yarp::name::ComposedNameService : public NameService {
public:
    ComposedNameService(NameService& ns1,NameService& ns2):ns1(ns1),ns2(ns2) {
    }

    virtual bool apply(yarp::os::Bottle& cmd, 
                       yarp::os::Bottle& reply, 
                       yarp::os::Bottle& event,
                       yarp::os::Contact& remote) {
        if (ns1.apply(cmd,reply,event,remote)) {
            return true;
        }
        return ns2.apply(cmd,reply,event,remote);
    }

    virtual void onEvent(yarp::os::Bottle& event) {
        ns1.onEvent(event);
        ns2.onEvent(event);
    }

private:
    NameService& ns1;
    NameService& ns2;
};

#endif
