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



const size_t ConstString::npos = std::string::npos;

#ifndef YARP_WRAP_STL_STRING_INLINE

 // implementation is a std::string
#define HELPER(x) (*((std::string*)(x)))

ConstString::ConstString() {
    implementation = new std::string();
    yAssert(implementation!=NULL);
}

ConstString::ConstString(const ConstString& str) {
    implementation = new std::string(HELPER(str.implementation));
    yAssert(implementation!=NULL);
}

ConstString::ConstString(const ConstString& str, size_t pos, size_t len) {
    implementation = new std::string(HELPER(str.implementation), pos, len);
}

ConstString::ConstString(const char *str) {
    implementation = new std::string(str);
    yAssert(implementation!=NULL);
}

ConstString::ConstString(const char *str, size_t len) {
    implementation = new std::string(str, len);
    yAssert(implementation!=NULL);
}

void ConstString::init(const char *str, size_t len) {
    implementation = new std::string(str, len);
    yAssert(implementation!=NULL);
}

ConstString::ConstString(size_t len, char c) {
    implementation = new std::string(len, c);
    yAssert(implementation!=NULL);
}

ConstString::~ConstString() {
    if (implementation!=NULL) {
        delete (std::string*)implementation;
        implementation = NULL;
    }
}


ConstString& ConstString::operator=(const ConstString& str) {
    HELPER(implementation) = HELPER(str.implementation);
    return (*this);
}

ConstString& ConstString::operator=(const char* str) {
    HELPER(implementation) = str;
    return (*this);
}

ConstString& ConstString::operator=(char c) {
    HELPER(implementation) = c;
    return (*this);
}

const char *ConstString::c_str() const {
    return HELPER(implementation).c_str();
}

const char *ConstString::data() const {
    return HELPER(implementation).data();
}

ConstString::allocator_type ConstString::get_allocator() const {
    return HELPER(implementation).get_allocator();
}

size_t ConstString::copy(char* str, size_t len, size_t pos) const {
#ifdef _MSC_VER
 #pragma warning(push)
 #pragma warning(disable : 4996)
#endif
    return HELPER(implementation).copy(str, len, pos);
#ifdef _MSC_VER
 #pragma warning(pop)
#endif
}

size_t ConstString::length() const {
    return HELPER(implementation).length();
}

size_t ConstString::size() const {
    return HELPER(implementation).size();
}

size_t ConstString::max_size() const {
    return HELPER(implementation).max_size();
}

void ConstString::resize(size_t n) {
    HELPER(implementation).resize(n);
}

void ConstString::resize(size_t n, char c) {
    HELPER(implementation).resize(n, c);
}

size_t ConstString::capacity() const {
    return HELPER(implementation).capacity();
}

void ConstString::reserve(size_t n) {
    HELPER(implementation).reserve(n);
}

ConstString& ConstString::operator+=(char ch) {
    HELPER(implementation) += ch;
    return *this;
}

ConstString& ConstString::operator+=(const char *str) {
    HELPER(implementation) += str;
    return *this;
}

ConstString& ConstString::operator+=(const ConstString& str) {
    HELPER(implementation) += HELPER(str.implementation);
    return *this;
}

void ConstString::push_back (char c) {
    HELPER(implementation).push_back(c);
}

ConstString& ConstString::erase(size_t pos, size_t len) {
    HELPER(implementation).erase(pos, len);
    return *this;
}

ConstString::iterator ConstString::erase(iterator p) {
    return HELPER(implementation).erase(p);
}

ConstString::iterator ConstString::erase(iterator first, iterator last) {
    return HELPER(implementation).erase(first, last);
}

void ConstString::swap(ConstString& str) {
    HELPER(implementation).swap(HELPER(str.implementation));
}

size_t ConstString::find(const ConstString& needle, size_t start) const {
    return HELPER(implementation).find(HELPER(needle.implementation), start);
}

size_t ConstString::find(const char *needle, size_t start) const {
    return HELPER(implementation).find(needle, start);
}

size_t ConstString::find(const char *needle, size_t start, size_t len) const {
    return HELPER(implementation).find(needle, start, len);
}

size_t ConstString::find(char needle, size_t start) const {
    return HELPER(implementation).find(needle,start);
}

size_t ConstString::rfind(const ConstString& needle, size_t start) const {
    return HELPER(implementation).rfind(HELPER(needle.implementation), start);
}

size_t ConstString::rfind(const char *needle, size_t start) const {
    return HELPER(implementation).rfind(needle, start);
}

size_t ConstString::rfind(const char *needle, size_t start, size_t len) const {
    return HELPER(implementation).rfind(needle, start, len);
}

size_t ConstString::rfind(char needle, size_t start) const {
    return HELPER(implementation).rfind(needle,start);
}

ConstString ConstString::substr(size_t start, size_t n) const {
    std::string result = HELPER(implementation).substr(start,n);
    return ConstString(result.c_str(),result.length());
}

void ConstString::clear() {
    HELPER(implementation).clear();
}

bool ConstString::empty() const {
    return HELPER(implementation).empty();
}

ConstString::iterator ConstString::begin() {
    return HELPER(implementation).begin();
}

ConstString::const_iterator ConstString::begin() const {
    return HELPER(implementation).begin();
}

ConstString::iterator ConstString::end() {
    return HELPER(implementation).end();
}

ConstString::const_iterator ConstString::end() const {
    return HELPER(implementation).end();
}

ConstString::reverse_iterator ConstString::rbegin() {
    return HELPER(implementation).rbegin();
}

ConstString::const_reverse_iterator ConstString::rbegin() const {
    return HELPER(implementation).rbegin();
}

ConstString::reverse_iterator ConstString::rend() {
    return HELPER(implementation).rend();
}

ConstString::const_reverse_iterator ConstString::rend() const {
    return HELPER(implementation).rend();
}

char& ConstString::operator[](size_t idx) {
    return HELPER(implementation)[idx];
}

const char& ConstString::operator[](size_t idx) const {
    return HELPER(implementation)[idx];
}

char& ConstString::at(size_t pos) {
    return HELPER(implementation).at(pos);
}

const char& ConstString::at(size_t pos) const {
    return HELPER(implementation).at(pos);
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

#endif
