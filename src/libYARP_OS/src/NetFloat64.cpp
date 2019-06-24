/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/NetFloat64.h>

#ifndef YARP_LITTLE_ENDIAN

using namespace yarp;
using namespace yarp::os;


double NetFloat64::swap(double x) const
{
    UnionNetFloat64 in, out;
    in.d = x;
    for (int i = 0; i < 8; i++) {
        out.c[i] = in.c[7 - i];
    }
    return out.d;
}

RawNetFloat64 NetFloat64::get() const
{
    return (double)swap((double)raw_value);
}

void NetFloat64::set(RawNetFloat64 v)
{
    raw_value = (double)swap((double)v);
}

NetFloat64::NetFloat64()
{
}

NetFloat64::NetFloat64(RawNetFloat64 val)
{
    set(val);
}

NetFloat64::operator RawNetFloat64() const
{
    return get();
}

RawNetFloat64 NetFloat64::operator+(RawNetFloat64 v) const
{
    return get() + v;
}

RawNetFloat64 NetFloat64::operator-(RawNetFloat64 v) const
{
    return get() - v;
}

RawNetFloat64 NetFloat64::operator*(RawNetFloat64 v) const
{
    return get() * v;
}

RawNetFloat64 NetFloat64::operator/(RawNetFloat64 v) const
{
    return get() / v;
}

void NetFloat64::operator+=(RawNetFloat64 v)
{
    set(get() + v);
}

void NetFloat64::operator-=(RawNetFloat64 v)
{
    set(get() - v);
}

void NetFloat64::operator*=(RawNetFloat64 v)
{
    set(get() * v);
}

void NetFloat64::operator/=(RawNetFloat64 v)
{
    set(get() / v);
}


#endif // YARP_LITTLE_ENDIAN
