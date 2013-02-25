// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/NetInt64.h>

#ifdef YARP_INT64
#ifndef YARP_LITTLE_ENDIAN

using namespace yarp;
using namespace yarp::os;


YARP_INT64 NetInt64::swap(YARP_INT64 x) const {
    UnionNetInt64 in, out;
    in.d = x;
    for (int i=0; i<8; i++) {
        out.c[i] = in.c[7-i];
    }
    return out.d;
}
RawNetInt64 NetInt64::get() const {
    return (RawNetInt64)swap((RawNetInt64)raw_value);
}
void NetInt64::set(RawNetInt64 v) {
    raw_value = (RawNetInt64)swap((RawNetInt64)v);
}
NetInt64::NetInt64() {
}
NetInt64::NetInt64(RawNetInt64 val) {
    set(val);
}
NetInt64::operator RawNetInt64() const {
    return get();
}
RawNetInt64 NetInt64::operator+(RawNetInt64 v) const {
    return get()+v;
}
RawNetInt64 NetInt64::operator-(RawNetInt64 v) const {
    return get()-v;
}
RawNetInt64 NetInt64::operator*(RawNetInt64 v) const {
    return get()*v;
}
RawNetInt64 NetInt64::operator/(RawNetInt64 v) const {
    return get()/v;
}
void NetInt64::operator+=(RawNetInt64 v) {
    set(get()+v);
}
void NetInt64::operator-=(RawNetInt64 v) {
    set(get()-v);
}
void NetInt64::operator*=(RawNetInt64 v) {
    set(get()*v);
}
void NetInt64::operator/=(RawNetInt64 v) {
    set(get()/v);
}
#endif // YARP_LITTLE_ENDIAN
#endif // YARP_INT64
