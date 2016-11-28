/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/serversql/impl/ParseName.h>

#include <string>

using namespace yarp::os;
using namespace yarp::serversql::impl;
using namespace std;

void ParseName::apply(const ConstString& s) {
    ConstString name = s;
    carrier = "";
    networkChoice = "";
    portName = "";
    int mid = name.find(":/");
    if (mid>0) {
        carrier = name.substr(0,mid).c_str();
        ConstString tail = name.substr(mid+2);
        if (carrier.length()>0 && carrier[0]=='/') {
            carrier = string(carrier.c_str()).substr(1).c_str();
        }
        name = tail;
    }
    if (name.find("/net=") == 0 || name.find("/NET=") == 0) {
        int patStart = 5;
        int patEnd = name.find('/',patStart);
        if (patEnd>=patStart) {
            networkChoice = name.substr(patStart,patEnd-patStart).c_str();
            name = name.substr(patEnd);
        }
    }
    portName = name.c_str();
}
