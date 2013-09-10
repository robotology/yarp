// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/NestedContact.h>

using namespace yarp::os;

NestedContact::NestedContact(const char *nFullName) {
    fromString(nFullName);
}

bool NestedContact::fromString(const char *nFullName) {
    fullName = nFullName;
    ConstString::size_type idx = fullName.find("#");
    if (idx==ConstString::npos) return false;
    nodeName = fullName.substr(0,idx);
    nestedName = fullName.substr(idx+1,fullName.length());
    idx = nestedName.find("/");
    if (idx==0) return true;
    category = nestedName.substr(0,idx);
    nestedName = nestedName.substr(idx+1,nestedName.length());
    return true;
}
