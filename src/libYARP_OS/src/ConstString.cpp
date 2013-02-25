// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/String.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NetType.h>
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


ConstString::ConstString(const char *str, int len) {
    implementation = new String(str,len);
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

ConstString::operator const char *() const {
    return c_str();
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
    return (int)HELPER(implementation).length();
}

ConstString ConstString::operator + (char ch) const {
    return (HELPER(implementation) + ch).c_str();
}

ConstString ConstString::operator + (const char *str) const {
    return (HELPER(implementation) + str).c_str();
}

ConstString ConstString::operator + (const ConstString& alt) const {
    String result = HELPER(implementation) + HELPER(alt.implementation);
    return ConstString(result.c_str(),result.length());
}

const ConstString& ConstString::operator += (char ch) {
    HELPER(implementation) += ch;
    return *this;
}

const ConstString& ConstString::operator += (const char *str) {
    HELPER(implementation) += str;
    return *this;
}

const ConstString& ConstString::operator += (const ConstString& alt) {
    HELPER(implementation) += HELPER(alt.implementation);
    return *this;
}

int ConstString::find(const char *needle) const {
    return (int)YARP_STRSTR(HELPER(implementation),needle);
}

int ConstString::find(const char *needle, int start) const {
    return (int)YARP_STRSTR_OFFSET(HELPER(implementation),needle,
                                   (YARP_STRING_INDEX)start);
}


ConstString ConstString::substr(int start, int n) const {
    return ConstString(HELPER(implementation).substr(start,n).c_str());
}


ConstString ConstString::toString(int x) {
    return ConstString(NetType::toString(x).c_str());
}

