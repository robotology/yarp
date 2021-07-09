/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NETUINT16_H
#define YARP_OS_NETUINT16_H

#include <yarp/conf/numeric.h>

#include <yarp/os/api.h>

////////////////////////////////////////////////////////////////////////
//
// The goal of this file is just to define a 16 bit unsigned little-endian
// integer type.
//
////////////////////////////////////////////////////////////////////////

namespace yarp {
namespace os {

/**
 * Definition of the NetUint16 type
 */

#ifdef YARP_LITTLE_ENDIAN

typedef std::uint16_t NetUint16;

#else // YARP_LITTLE_ENDIAN

class YARP_os_API NetUint16
{
private:
    std::uint16_t raw_value;
    std::uint16_t swap(std::uint16_t x) const;
    std::uint16_t get() const;
    void set(std::uint16_t v);

public:
    NetUint16();
    NetUint16(std::uint16_t val);
    operator std::uint16_t() const;
    std::uint16_t operator+(std::uint16_t v) const;
    std::uint16_t operator-(std::uint16_t v) const;
    std::uint16_t operator*(std::uint16_t v) const;
    std::uint16_t operator/(std::uint16_t v) const;
    void operator+=(std::uint16_t v);
    void operator-=(std::uint16_t v);
    void operator*=(std::uint16_t v);
    void operator/=(std::uint16_t v);
    void operator++(int);
    void operator--(int);
};

#endif // YARP_LITTLE_ENDIAN

} // namespace os
} // namespace yarp

#endif // YARP_OS_NETUINT16_H
