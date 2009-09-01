// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include "ParseName.h"

#include <string>

using namespace yarp::os;
using namespace std;

void ParseName::apply(const char *str) {
    string name(str);
    carrier = "";
    networkChoice = "";
    portName = "";
    int mid = name.find(":/");
    if (mid>0) {
        carrier = name.substr(0,mid).c_str();
        string tail = name.substr(mid+2);
        if (carrier[0]=='/') {
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
