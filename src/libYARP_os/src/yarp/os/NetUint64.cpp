/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/NetUint64.h>

#ifndef YARP_LITTLE_ENDIAN

using namespace yarp;
using namespace yarp::os;


std::uint64_t NetUint64::swap(std::uint64_t x) const
{
    UnionNetUint64 in, out;
    in.d = x;
    for (int i = 0; i < 8; i++) {
        out.c[i] = in.c[7 - i];
    }
    return out.d;
}
RawNetUint64 NetUint64::get() const
{
    return (RawNetUint64)swap((RawNetUint64)raw_value);
}
void NetUint64::set(RawNetUint64 v)
{
    raw_value = (RawNetUint64)swap((RawNetUint64)v);
}
NetUint64::NetUint64()
{
}
NetUint64::NetUint64(RawNetUint64 val)
{
    set(val);
}
NetUint64::operator RawNetUint64() const
{
    return get();
}
RawNetUint64 NetUint64::operator+(RawNetUint64 v) const
{
    return get() + v;
}
RawNetUint64 NetUint64::operator-(RawNetUint64 v) const
{
    return get() - v;
}
RawNetUint64 NetUint64::operator*(RawNetUint64 v) const
{
    return get() * v;
}
RawNetUint64 NetUint64::operator/(RawNetUint64 v) const
{
    return get() / v;
}
void NetUint64::operator+=(RawNetUint64 v)
{
    set(get() + v);
}
void NetUint64::operator-=(RawNetUint64 v)
{
    set(get() - v);
}
void NetUint64::operator*=(RawNetUint64 v)
{
    set(get() * v);
}
void NetUint64::operator/=(RawNetUint64 v)
{
    set(get() / v);
}
#endif // YARP_LITTLE_ENDIAN
