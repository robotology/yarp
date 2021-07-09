/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NETUINT64_H
#define YARP_OS_NETUINT64_H

#include <yarp/conf/numeric.h>

#include <yarp/os/api.h>

////////////////////////////////////////////////////////////////////////
//
// The goal of this file is just to define a 64 bit unsigned little-endian
// integer type
//
////////////////////////////////////////////////////////////////////////

namespace yarp {
namespace os {

/**
 * Definition of the NetInt64 type
 */

#ifdef YARP_LITTLE_ENDIAN

typedef std::uint64_t NetUint64;

#else

typedef std::uint64_t RawNetUint64;
union UnionNetUint64
{
    std::uint64_t d;
    unsigned char c[8];
};
class YARP_os_API NetUint64
{
private:
    std::uint64_t raw_value;
    std::uint64_t swap(std::uint64_t x) const;
    RawNetUint64 get() const;
    void set(RawNetUint64 v);

public:
    NetUint64();
    NetUint64(RawNetUint64 val);
    operator RawNetUint64() const;
    RawNetUint64 operator+(RawNetUint64 v) const;
    RawNetUint64 operator-(RawNetUint64 v) const;
    RawNetUint64 operator*(RawNetUint64 v) const;
    RawNetUint64 operator/(RawNetUint64 v) const;
    void operator+=(RawNetUint64 v);
    void operator-=(RawNetUint64 v);
    void operator*=(RawNetUint64 v);
    void operator/=(RawNetUint64 v);
};

#endif // YARP_LITTLE_ENDIAN

} // namespace os
} // namespace yarp

#endif // YARP_OS_NETUINT64_H
