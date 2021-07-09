/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NETINT64_H
#define YARP_OS_NETINT64_H

#include <yarp/conf/numeric.h>

#include <yarp/os/api.h>

////////////////////////////////////////////////////////////////////////
//
// The goal of this file is just to define a 64 bit signed little-endian
// integer type
//
////////////////////////////////////////////////////////////////////////

namespace yarp {
namespace os {

/**
 * Definition of the NetInt64 type
 */

#ifdef YARP_LITTLE_ENDIAN

typedef std::int64_t NetInt64;

#else // YARP_LITTLE_ENDIAN

typedef std::int64_t RawNetInt64;
union UnionNetInt64
{
    std::int64_t d;
    unsigned char c[8];
};
class YARP_os_API NetInt64
{
private:
    std::int64_t raw_value;
    std::int64_t swap(std::int64_t x) const;
    RawNetInt64 get() const;
    void set(RawNetInt64 v);

public:
    NetInt64();
    NetInt64(RawNetInt64 val);
    operator RawNetInt64() const;
    RawNetInt64 operator+(RawNetInt64 v) const;
    RawNetInt64 operator-(RawNetInt64 v) const;
    RawNetInt64 operator*(RawNetInt64 v) const;
    RawNetInt64 operator/(RawNetInt64 v) const;
    void operator+=(RawNetInt64 v);
    void operator-=(RawNetInt64 v);
    void operator*=(RawNetInt64 v);
    void operator/=(RawNetInt64 v);
};

#endif // YARP_LITTLE_ENDIAN

} // namespace os
} // namespace yarp

#endif // YARP_OS_NETINT64_H
