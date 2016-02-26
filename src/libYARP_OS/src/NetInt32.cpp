/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/NetInt32.h>

#ifdef YARP_INT32
#ifndef YARP_LITTLE_ENDIAN

using namespace yarp;
using namespace yarp::os;


unsigned YARP_INT32 NetInt32::swap(unsigned YARP_INT32 x) const {
    return (x>>24) | ((x>>8) & 0xff00) | ((x<<8) & 0xff0000) | (x<<24);
}

YARP_INT32 NetInt32::get() const {
    return (YARP_INT32)swap(raw_value);
}

void NetInt32::set(YARP_INT32 v) {
    raw_value = (YARP_INT32)swap((unsigned YARP_INT32)v);
}

NetInt32::NetInt32() {
}

NetInt32::NetInt32(YARP_INT32 val) {
    set(val);
}

NetInt32::operator YARP_INT32() const {
    return get();
}

YARP_INT32 NetInt32::operator+(YARP_INT32 v) const {
    return get()+v;
}

YARP_INT32 NetInt32::operator-(YARP_INT32 v) const {
    return get()-v;
}

YARP_INT32 NetInt32::operator*(YARP_INT32 v) const {
    return get()*v;
}

YARP_INT32 NetInt32::operator/(YARP_INT32 v) const {
    return get()/v;
}

void NetInt32::operator+=(YARP_INT32 v) {
    set(get()+v);
}

void NetInt32::operator-=(YARP_INT32 v) {
    set(get()-v);
}

void NetInt32::operator*=(YARP_INT32 v) {
    set(get()*v);
}

void NetInt32::operator/=(YARP_INT32 v) {
    set(get()/v);
}

void NetInt32::operator++(int) {
    set(get()+1);
}

void NetInt32::operator--(int) {
    set(get()-1);
}


#endif // YARP_LITTLE_ENDIAN
#endif // YARP_INT32
