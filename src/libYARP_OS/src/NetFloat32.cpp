// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/NetFloat32.h>

#ifdef YARP_FLOAT32
#ifndef YARP_LITTLE_ENDIAN

using namespace yarp;
using namespace yarp::os;


double NetFloat32::swap(double x) const {
    UnionNetFloat32 in, out;
    in.d = x;
    for (int i=0; i<4; i++) {
        out.c[i] = in.c[3-i];
    }
    return out.d;
}

RawNetFloat32 NetFloat32::get() const {
    return (double)swap((double)raw_value);
}

void NetFloat32::set(RawNetFloat32 v) {
    raw_value = (double)swap((double)v);
}

NetFloat32::NetFloat32() {
}

NetFloat32::NetFloat32(RawNetFloat32 val) {
    set(val);
}

NetFloat32::operator RawNetFloat32() const {
    return get();
}

RawNetFloat32 NetFloat32::operator+(RawNetFloat32 v) const {
    return get()+v;
}

RawNetFloat32 NetFloat32::operator-(RawNetFloat32 v) const {
    return get()-v;
}

RawNetFloat32 NetFloat32::operator*(RawNetFloat32 v) const {
    return get()*v;
}

RawNetFloat32 NetFloat32::operator/(RawNetFloat32 v) const {
    return get()/v;
}

void NetFloat32::operator+=(RawNetFloat32 v) {
    set(get()+v);
}

void NetFloat32::operator-=(RawNetFloat32 v) {
    set(get()-v);
}

void NetFloat32::operator*=(RawNetFloat32 v) {
    set(get()*v);
}

void NetFloat32::operator/=(RawNetFloat32 v) {
    set(get()/v);
}

#endif // YARP_LITTLE_ENDIAN
#endif // YARP_FLOAT32
