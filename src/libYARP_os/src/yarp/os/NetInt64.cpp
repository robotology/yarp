/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/NetInt64.h>

#ifndef YARP_LITTLE_ENDIAN

using namespace yarp;
using namespace yarp::os;


std::int64_t NetInt64::swap(std::int64_t x) const
{
    UnionNetInt64 in, out;
    in.d = x;
    for (int i = 0; i < 8; i++) {
        out.c[i] = in.c[7 - i];
    }
    return out.d;
}
RawNetInt64 NetInt64::get() const
{
    return (RawNetInt64)swap((RawNetInt64)raw_value);
}
void NetInt64::set(RawNetInt64 v)
{
    raw_value = (RawNetInt64)swap((RawNetInt64)v);
}
NetInt64::NetInt64()
{
}
NetInt64::NetInt64(RawNetInt64 val)
{
    set(val);
}
NetInt64::operator RawNetInt64() const
{
    return get();
}
RawNetInt64 NetInt64::operator+(RawNetInt64 v) const
{
    return get() + v;
}
RawNetInt64 NetInt64::operator-(RawNetInt64 v) const
{
    return get() - v;
}
RawNetInt64 NetInt64::operator*(RawNetInt64 v) const
{
    return get() * v;
}
RawNetInt64 NetInt64::operator/(RawNetInt64 v) const
{
    return get() / v;
}
void NetInt64::operator+=(RawNetInt64 v)
{
    set(get() + v);
}
void NetInt64::operator-=(RawNetInt64 v)
{
    set(get() - v);
}
void NetInt64::operator*=(RawNetInt64 v)
{
    set(get() * v);
}
void NetInt64::operator/=(RawNetInt64 v)
{
    set(get() / v);
}
#endif // YARP_LITTLE_ENDIAN
