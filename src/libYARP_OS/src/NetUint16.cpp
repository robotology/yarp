/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/NetUint16.h>

#ifndef YARP_LITTLE_ENDIAN

using namespace yarp;
using namespace yarp::os;


std::uint16_t NetUint16::swap(std::uint16_t x) const
{
    return (x >> 8) | ((x << 8) & 0xff00);
}

std::uint16_t NetUint16::get() const
{
    return (std::uint16_t)swap(raw_value);
}

void NetUint16::set(std::uint16_t v)
{
    raw_value = (std::uint16_t)swap((std::uint16_t)v);
}

NetUint16::NetUint16()
{
}

NetUint16::NetUint16(std::uint16_t val)
{
    set(val);
}

NetUint16::operator std::uint16_t() const
{
    return get();
}

std::uint16_t NetUint16::operator+(std::uint16_t v) const
{
    return get() + v;
}

std::uint16_t NetUint16::operator-(std::uint16_t v) const
{
    return get() - v;
}

std::uint16_t NetUint16::operator*(std::uint16_t v) const
{
    return get() * v;
}

std::uint16_t NetUint16::operator/(std::uint16_t v) const
{
    return get() / v;
}

void NetUint16::operator+=(std::uint16_t v)
{
    set(get() + v);
}

void NetUint16::operator-=(std::uint16_t v)
{
    set(get() - v);
}

void NetUint16::operator*=(std::uint16_t v)
{
    set(get() * v);
}

void NetUint16::operator/=(std::uint16_t v)
{
    set(get() / v);
}

void NetUint16::operator++(int)
{
    set(get() + 1);
}

void NetUint16::operator--(int)
{
    set(get() - 1);
}


#endif // YARP_LITTLE_ENDIAN
