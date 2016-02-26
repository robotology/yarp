/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <yarp/os/all.h>

#include <tcpros_carrier_api.h>

class YARP_tcpros_carrier_API RosLookup {
public:
    bool valid;
    yarp::os::ConstString hostname;
    int portnum;
    yarp::os::ConstString protocol;
    bool verbose;

    RosLookup(bool verbose) : verbose(verbose) {
        valid = false;
    }

    bool lookupCore(const yarp::os::ConstString& name);

    bool lookupTopic(const yarp::os::ConstString& name);

    yarp::os::ConstString toString() {
        char buf[1000];
        sprintf(buf,"/%s:%d/", hostname.c_str(), portnum);
        return buf;
    }

    yarp::os::Contact toContact(const char *carrier) {
        return yarp::os::Contact::bySocket(carrier,hostname.c_str(), portnum);
    }

    static yarp::os::Contact getRosCoreAddressFromEnv();
    static yarp::os::Contact getRosCoreAddress();
};
