/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/NetUint16.h>

#ifdef YARP_INT16
#ifndef YARP_LITTLE_ENDIAN

using namespace yarp;
using namespace yarp::os;


unsigned YARP_INT16 NetUint16::swap(unsigned YARP_INT16 x) const {
    return (x>>8) | ((x<<8) & 0xff00);
}

unsigned YARP_INT16 NetUint16::get() const {
    return (unsigned YARP_INT16)swap(raw_value);
}

void NetUint16::set(unsigned YARP_INT16 v) {
    raw_value = (unsigned YARP_INT16)swap((unsigned YARP_INT16)v);
}

NetUint16::NetUint16() {
}

NetUint16::NetUint16(unsigned YARP_INT16 val) {
    set(val);
}

NetUint16::operator unsigned YARP_INT16() const {
    return get();
}

unsigned YARP_INT16 NetUint16::operator+(unsigned YARP_INT16 v) const {
    return get()+v;
}

unsigned YARP_INT16 NetUint16::operator-(unsigned YARP_INT16 v) const {
    return get()-v;
}

unsigned YARP_INT16 NetUint16::operator*(unsigned YARP_INT16 v) const {
    return get()*v;
}

unsigned YARP_INT16 NetUint16::operator/(unsigned YARP_INT16 v) const {
    return get()/v;
}

void NetUint16::operator+=(unsigned YARP_INT16 v) {
    set(get()+v);
}

void NetUint16::operator-=(unsigned YARP_INT16 v) {
    set(get()-v);
}

void NetUint16::operator*=(unsigned YARP_INT16 v) {
    set(get()*v);
}

void NetUint16::operator/=(unsigned YARP_INT16 v) {
    set(get()/v);
}

void NetUint16::operator++(int) {
    set(get()+1);
}

void NetUint16::operator--(int) {
    set(get()-1);
}


#endif // YARP_LITTLE_ENDIAN
#endif // YARP_INT16
