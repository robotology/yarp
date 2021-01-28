/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/NetUint32.h>

#ifndef YARP_LITTLE_ENDIAN

using namespace yarp;
using namespace yarp::os;


std::uint32_t NetUint32::swap(std::uint32_t x) const
{
    return (x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | (x << 24);
}

std::uint32_t NetUint32::get() const
{
    return (std::uint32_t)swap(raw_value);
}

void NetUint32::set(std::uint32_t v)
{
    raw_value = (std::uint32_t)swap((std::uint32_t)v);
}

NetUint32::NetUint32()
{
}

NetUint32::NetUint32(std::uint32_t val)
{
    set(val);
}

NetUint32::operator std::uint32_t() const
{
    return get();
}

std::uint32_t NetUint32::operator+(std::uint32_t v) const
{
    return get() + v;
}

std::uint32_t NetUint32::operator-(std::uint32_t v) const
{
    return get() - v;
}

std::uint32_t NetUint32::operator*(std::uint32_t v) const
{
    return get() * v;
}

std::uint32_t NetUint32::operator/(std::uint32_t v) const
{
    return get() / v;
}

void NetUint32::operator+=(std::uint32_t v)
{
    set(get() + v);
}

void NetUint32::operator-=(std::uint32_t v)
{
    set(get() - v);
}

void NetUint32::operator*=(std::uint32_t v)
{
    set(get() * v);
}

void NetUint32::operator/=(std::uint32_t v)
{
    set(get() / v);
}

void NetUint32::operator++(int)
{
    set(get() + 1);
}

void NetUint32::operator--(int)
{
    set(get() - 1);
}


#endif // YARP_LITTLE_ENDIAN
