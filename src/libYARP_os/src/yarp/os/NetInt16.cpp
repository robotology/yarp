/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/NetInt16.h>

#ifndef YARP_LITTLE_ENDIAN

using namespace yarp;
using namespace yarp::os;


std::uint16_t NetInt16::swap(std::uint16_t x) const
{
    return (x >> 8) | ((x << 8) & 0xff00);
}

std::int16_t NetInt16::get() const
{
    return (std::int16_t)swap(raw_value);
}

void NetInt16::set(std::int16_t v)
{
    raw_value = (std::int16_t)swap((std::uint16_t)v);
}

NetInt16::NetInt16()
{
}

NetInt16::NetInt16(std::int16_t val)
{
    set(val);
}

NetInt16::operator std::int16_t() const
{
    return get();
}

std::int16_t NetInt16::operator+(std::int16_t v) const
{
    return get() + v;
}

std::int16_t NetInt16::operator-(std::int16_t v) const
{
    return get() - v;
}

std::int16_t NetInt16::operator*(std::int16_t v) const
{
    return get() * v;
}

std::int16_t NetInt16::operator/(std::int16_t v) const
{
    return get() / v;
}

void NetInt16::operator+=(std::int16_t v)
{
    set(get() + v);
}

void NetInt16::operator-=(std::int16_t v)
{
    set(get() - v);
}

void NetInt16::operator*=(std::int16_t v)
{
    set(get() * v);
}

void NetInt16::operator/=(std::int16_t v)
{
    set(get() / v);
}

void NetInt16::operator++(int)
{
    set(get() + 1);
}

void NetInt16::operator--(int)
{
    set(get() - 1);
}

#endif // YARP_LITTLE_ENDIAN
