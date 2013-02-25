// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/NetUint32.h>

#ifdef YARP_INT32
#ifndef YARP_LITTLE_ENDIAN

using namespace yarp;
using namespace yarp::os;


unsigned YARP_INT32 NetUint32::swap(unsigned YARP_INT32 x) const {
    return (x>>24) | ((x>>8) & 0xff00) | ((x<<8) & 0xff0000) | (x<<24);
}

unsigned YARP_INT32 NetUint32::get() const {
    return (unsigned YARP_INT32)swap(raw_value);
}

void NetUint32::set(unsigned YARP_INT32 v) {
    raw_value = (unsigned YARP_INT32)swap((unsigned YARP_INT32)v);
}

NetUint32::NetUint32() {
}

NetUint32::NetUint32(unsigned YARP_INT32 val) {
    set(val);
}

NetUint32::operator unsigned YARP_INT32() const {
    return get();
}

unsigned YARP_INT32 NetUint32::operator+(unsigned YARP_INT32 v) const {
    return get()+v;
}

unsigned YARP_INT32 NetUint32::operator-(unsigned YARP_INT32 v) const {
    return get()-v;
}

unsigned YARP_INT32 NetUint32::operator*(unsigned YARP_INT32 v) const {
    return get()*v;
}

unsigned YARP_INT32 NetUint32::operator/(unsigned YARP_INT32 v) const {
    return get()/v;
}

void NetUint32::operator+=(unsigned YARP_INT32 v) {
    set(get()+v);
}

void NetUint32::operator-=(unsigned YARP_INT32 v) {
    set(get()-v);
}

void NetUint32::operator*=(unsigned YARP_INT32 v) {
    set(get()*v);
}

void NetUint32::operator/=(unsigned YARP_INT32 v) {
    set(get()/v);
}

void NetUint32::operator++(int) {
    set(get()+1);
}

void NetUint32::operator--(int) {
    set(get()-1);
}


#endif // YARP_LITTLE_ENDIAN
#endif // YARP_INT32
