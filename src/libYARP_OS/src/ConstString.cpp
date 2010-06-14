// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/String.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/ConstString.h>

using namespace yarp::os::impl;
using namespace yarp::os;


// implementation is a String
#define HELPER(x) (*((String*)(x)))

int ConstString::npos = -1;

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

int ConstString::length() const {
    return HELPER(implementation).length();
}

ConstString ConstString::operator + (const char *str) const {
    return (HELPER(implementation) + str).c_str();
}

ConstString ConstString::operator + (const ConstString& alt) const {
    return (HELPER(implementation) + HELPER(alt.implementation)).c_str();
}

int ConstString::find(const char *needle) const {
    return (int)YARP_STRSTR(HELPER(implementation),needle);
}

ConstString ConstString::substr(int start, int n) const {
    return ConstString(HELPER(implementation).substr(start,n).c_str());
}
