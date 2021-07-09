/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/NetInt32.h>

#ifndef YARP_LITTLE_ENDIAN

using namespace yarp;
using namespace yarp::os;


std::uint32_t NetInt32::swap(std::uint32_t x) const
{
    return (x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | (x << 24);
}

std::int32_t NetInt32::get() const
{
    return (std::int32_t)swap(raw_value);
}

void NetInt32::set(std::int32_t v)
{
    raw_value = (std::int32_t)swap((std::uint32_t)v);
}

NetInt32::NetInt32()
{
}

NetInt32::NetInt32(std::int32_t val)
{
    set(val);
}

NetInt32::operator std::int32_t() const
{
    return get();
}

std::int32_t NetInt32::operator+(std::int32_t v) const
{
    return get() + v;
}

std::int32_t NetInt32::operator-(std::int32_t v) const
{
    return get() - v;
}

std::int32_t NetInt32::operator*(std::int32_t v) const
{
    return get() * v;
}

std::int32_t NetInt32::operator/(std::int32_t v) const
{
    return get() / v;
}

void NetInt32::operator+=(std::int32_t v)
{
    set(get() + v);
}

void NetInt32::operator-=(std::int32_t v)
{
    set(get() - v);
}

void NetInt32::operator*=(std::int32_t v)
{
    set(get() * v);
}

void NetInt32::operator/=(std::int32_t v)
{
    set(get() / v);
}

void NetInt32::operator++(int)
{
    set(get() + 1);
}

void NetInt32::operator--(int)
{
    set(get() - 1);
}


#endif // YARP_LITTLE_ENDIAN
