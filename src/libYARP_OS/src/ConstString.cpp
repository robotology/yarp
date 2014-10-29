// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/ConstString.h>

#ifndef YARP_WRAP_STL_STRING
int __dummy_symbol_yarp_os_conststring = 1;
#else


#include <yarp/os/Log.h>
#include <yarp/os/NetType.h>

#include <string>

using namespace yarp::os;



size_t ConstString::npos = std::string::npos;

#ifndef YARP_WRAP_STL_STRING_INLINE

 // implementation is a std::string
#define HELPER(x) (*((std::string*)(x)))

ConstString::ConstString() {
    implementation = new std::string();
    yAssert(implementation!=NULL);
}

ConstString::ConstString(const char *str) {
    implementation = new std::string(str);
    yAssert(implementation!=NULL);
}

ConstString::ConstString(const char *str, int len) {
    implementation = new std::string(str,len);
    yAssert(implementation!=NULL);
}

void ConstString::init(const char *str, size_t len) {
    implementation = new std::string(str,len);
    yAssert(implementation!=NULL);
}

ConstString::ConstString(size_t len, char v) {
    implementation = new std::string(len,v);
    yAssert(implementation!=NULL);
}

ConstString::~ConstString() {
    if (implementation!=NULL) {
        delete (std::string*)implementation;
        implementation = NULL;
    }
}

ConstString::ConstString(const ConstString& alt) {
    implementation = new std::string(HELPER(alt.implementation));
    yAssert(implementation!=NULL);
}

const ConstString& ConstString::operator = (const ConstString& alt) {
    HELPER(implementation) = HELPER(alt.implementation);
    return (*this);
}

const char *ConstString::c_str() const {
    return HELPER(implementation).c_str();
}

const char *ConstString::data() const {
    return HELPER(implementation).data();
}


bool ConstString::operator <(const ConstString& alt) const {
    return HELPER(implementation) < HELPER(alt.implementation);
}

bool ConstString::operator >(const ConstString& alt) const {
    return HELPER(implementation) > HELPER(alt.implementation);
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

size_t ConstString::length() const {
    return HELPER(implementation).length();
}

void ConstString::resize(size_t n) {
    HELPER(implementation).resize(n);
}


bool ConstString::operator <=(const ConstString& alt) const {
    return HELPER(implementation) <= HELPER(alt.implementation);
}

bool ConstString::operator >=(const ConstString& alt) const {
    return HELPER(implementation) >= HELPER(alt.implementation);
}

ConstString ConstString::operator + (char ch) const {
    std::string helper(HELPER(implementation));
    helper += ch;
    return ConstString(helper.c_str());
}

ConstString ConstString::operator + (const char *str) const {
    std::string helper(HELPER(implementation));
    helper += str;
    return ConstString(helper.c_str());
}

ConstString ConstString::operator + (const ConstString& alt) const {
    std::string helper(HELPER(implementation));
    helper += HELPER(alt.implementation);
    return ConstString(helper.c_str());
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

size_t ConstString::find(const ConstString& needle) const{
    return HELPER(implementation).find(HELPER(needle.implementation));
}

size_t ConstString::find(const char *needle) const {
    return HELPER(implementation).find(needle);
}

size_t ConstString::find(const char *needle, size_t start) const {
    return HELPER(implementation).find(needle,start);
}

size_t ConstString::find(const char needle, size_t start) const {
    return HELPER(implementation).find(needle,start);
}

size_t ConstString::rfind(const char needle) const {
    return HELPER(implementation).rfind(needle);
}

ConstString ConstString::substr(size_t start, size_t n) const {
    std::string result = HELPER(implementation).substr(start,n);
    return ConstString(result.c_str(),result.length());
}

void ConstString::clear() {
    HELPER(implementation).clear();
}

char& ConstString::operator[](size_t idx) {
    return HELPER(implementation)[idx];
}

const char& ConstString::operator[](size_t idx) const {
    return HELPER(implementation)[idx];
}

ConstString& ConstString::assign(const char *s, size_t n) {
    HELPER(implementation).assign(s,n);
    return *this;
}

#endif

unsigned long ConstString::hash() const {
#ifndef YARP_WRAP_STL_STRING_INLINE
    std::string& x = HELPER(implementation);
#else
    const std::string& x = s;
#endif
    unsigned long h = 0;
    for (size_t i=0; i<x.length(); i++) {
        unsigned char ch = x[i];
        h = (h << 4) + (ch * 13);
        unsigned long g = h & 0xf0000000;
        if (g) {
            h ^= (g>>24);
            h ^= g;
        }
    }
    return h;
}

ConstString yarp::os::operator + (const char *txt, 
                                  const yarp::os::ConstString& alt) {
    return ConstString(txt) + alt;
}

#endif
