/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>
#include <yarp/os/all.h>

#include <tcpros_carrier_api.h>

class YARP_tcpros_carrier_API RosLookup {
public:
    bool valid;
    yarp::os::ConstString hostname;
    int portnum;
    yarp::os::ConstString protocol;
    bool verbose;

    RosLookup(bool verbose) :
        valid(false),
        portnum(-1),
        verbose(verbose)
    {}

    bool lookupCore(const yarp::os::ConstString& name);

    bool lookupTopic(const yarp::os::ConstString& name);

    yarp::os::ConstString toString() {
        char buf[1000];
        sprintf(buf,"/%s:%d/", hostname.c_str(), portnum);
        return buf;
    }

    yarp::os::Contact toContact(const char *carrier) {
        return yarp::os::Contact(carrier,hostname.c_str(), portnum);
    }

    static yarp::os::Contact getRosCoreAddressFromEnv();
    static yarp::os::Contact getRosCoreAddress();
};
