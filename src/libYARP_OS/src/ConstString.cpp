// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/String.h>
#include <yarp/Logger.h>
#include <yarp/os/ConstString.h>

using namespace yarp;
using namespace yarp::os;


// implementation is a String
#define HELPER(x) (*((String*)(x)))


ConstString::ConstString() {
    implementation = new String;
    YARP_ASSERT(implementation!=NULL);
}


ConstString::ConstString(const char *str) {
    implementation = new String(str);
    YARP_ASSERT(implementation!=NULL);
}


ConstString::~ConstString() {
    if (implementation!=NULL) {
        delete (String*)implementation;
        implementation = NULL;
    }
}


ConstString::ConstString(const ConstString& alt) {
    implementation = new String();
    YARP_ASSERT(implementation!=NULL);
    HELPER(implementation) = HELPER(alt.implementation);
}


const ConstString& ConstString::operator = (const ConstString& alt) {
    HELPER(implementation) = HELPER(alt.implementation);
    return (*this);
}


const char *ConstString::c_str() const {
    return HELPER(implementation).c_str();
}

bool ConstString::operator ==(const ConstString& alt) const {
    return HELPER(implementation) == HELPER(alt.implementation);
}

bool ConstString::operator !=(const ConstString& alt) const {
    return HELPER(implementation) != HELPER(alt.implementation);
}

bool ConstString::operator ==(const char *str) const {
    return HELPER(implementation) == str;
}


bool ConstString::operator !=(const char *str) const {
    return HELPER(implementation) != str;
}

