/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Contact.h>

#include <cstdio>
#include <string>

class RosLookup
{
public:
    bool valid;
    std::string hostname;
    int portnum;
    std::string protocol;

    RosLookup() :
        valid(false),
        portnum(-1)
    {}

    bool lookupCore(const std::string& name);

    bool lookupTopic(const std::string& name);

    std::string toString() const {
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
