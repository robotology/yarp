/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/NetInt16.h>

#ifdef YARP_INT16
#ifndef YARP_LITTLE_ENDIAN

using namespace yarp;
using namespace yarp::os;


unsigned YARP_INT16 NetInt16::swap(unsigned YARP_INT16 x) const {
    return (x>>8) | ((x<<8) & 0xff00);
}

YARP_INT16 NetInt16::get() const {
    return (YARP_INT16)swap(raw_value);
}

void NetInt16::set(YARP_INT16 v) {
    raw_value = (YARP_INT16)swap((unsigned YARP_INT16)v);
}

NetInt16::NetInt16() {
}

NetInt16::NetInt16(YARP_INT16 val) {
    set(val);
}

NetInt16::operator YARP_INT16() const {
    return get();
}

YARP_INT16 NetInt16::operator+(YARP_INT16 v) const {
    return get()+v;
}

YARP_INT16 NetInt16::operator-(YARP_INT16 v) const {
    return get()-v;
}

YARP_INT16 NetInt16::operator*(YARP_INT16 v) const {
    return get()*v;
}

YARP_INT16 NetInt16::operator/(YARP_INT16 v) const {
    return get()/v;
}

void NetInt16::operator+=(YARP_INT16 v) {
    set(get()+v);
}

void NetInt16::operator-=(YARP_INT16 v) {
    set(get()-v);
}

void NetInt16::operator*=(YARP_INT16 v) {
    set(get()*v);
}

void NetInt16::operator/=(YARP_INT16 v) {
    set(get()/v);
}

void NetInt16::operator++(int) {
    set(get()+1);
}

void NetInt16::operator--(int) {
    set(get()-1);
}

#endif // YARP_LITTLE_ENDIAN
#endif // YARP_INT16
