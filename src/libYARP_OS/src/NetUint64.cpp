// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/NetUint64.h>

#ifdef YARP_INT64
#ifndef YARP_LITTLE_ENDIAN

using namespace yarp;
using namespace yarp::os;


unsigned YARP_INT64 NetUint64::swap(unsigned YARP_INT64 x) const {
    UnionNetUint64 in, out;
    in.d = x;
    for (int i=0; i<8; i++) {
        out.c[i] = in.c[7-i];
    }
    return out.d;
}
RawNetUint64 NetUint64::get() const {
    return (RawNetUint64)swap((RawNetUint64)raw_value);
}
void NetUint64::set(RawNetUint64 v) {
    raw_value = (RawNetUint64)swap((RawNetUint64)v);
}
NetUint64::NetUint64() {
}
NetUint64::NetUint64(RawNetUint64 val) {
    set(val);
}
NetUint64::operator RawNetUint64() const {
    return get();
}
RawNetUint64 NetUint64::operator+(RawNetUint64 v) const {
    return get()+v;
}
RawNetUint64 NetUint64::operator-(RawNetUint64 v) const {
    return get()-v;
}
RawNetUint64 NetUint64::operator*(RawNetUint64 v) const {
    return get()*v;
}
RawNetUint64 NetUint64::operator/(RawNetUint64 v) const {
    return get()/v;
}
void NetUint64::operator+=(RawNetUint64 v) {
    set(get()+v);
}
void NetUint64::operator-=(RawNetUint64 v) {
    set(get()-v);
}
void NetUint64::operator*=(RawNetUint64 v) {
    set(get()*v);
}
void NetUint64::operator/=(RawNetUint64 v) {
    set(get()/v);
}
#endif // YARP_LITTLE_ENDIAN
#endif // YARP_INT64
