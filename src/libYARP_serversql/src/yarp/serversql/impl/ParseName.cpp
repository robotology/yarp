/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/serversql/impl/ParseName.h>

#include <string>

using namespace yarp::serversql::impl;
using namespace std;

void ParseName::apply(const std::string& s) {
    std::string name = s;
    carrier = "";
    networkChoice = "";
    portName = "";
    int mid = name.find(":/");
    if (mid>0) {
        carrier = name.substr(0,mid);
        std::string tail = name.substr(mid+2);
        if (carrier.length()>0 && carrier[0]=='/') {
            carrier = carrier.substr(1);
        }
        name = tail;
    }
    if (name.find("/net=") == 0 || name.find("/NET=") == 0) {
        int patStart = 5;
        int patEnd = name.find('/',patStart);
        if (patEnd>=patStart) {
            networkChoice = name.substr(patStart,patEnd-patStart);
            name = name.substr(patEnd);
        }
    }
    portName = name;
}
