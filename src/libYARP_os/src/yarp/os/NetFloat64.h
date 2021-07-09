/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NETFLOAT64_H
#define YARP_OS_NETFLOAT64_H

#include <yarp/conf/numeric.h>

#include <yarp/os/api.h>

////////////////////////////////////////////////////////////////////////
//
// The goal of this file is just to define a 64 bit signed little-endian
// IEC 559/IEEE 754 floating point type.
//
////////////////////////////////////////////////////////////////////////

#if !YARP_FLOAT64_IS_IEC559
  // YARP assumes that floating point values are serialized as IEC 559/IEEE 754
  // floating point types.
  // If you receive the following error, this means that float and double, on
  // your platform, are not IEC 559, and therefore some conversion must be
  // performed whenever reading or writing a floating point value from the
  // network.
  // See, for example https://github.com/MalcolmMcLean/ieee754/ for a possible
  // implementation of the read and write methods.
  YARP_COMPILER_ERROR("Unsupported compiler. Please implement yarp::os::NetFloat64")
#endif

namespace yarp {
namespace os {

/**
 * Definition of the NetFloat64 type
 */

#ifdef YARP_LITTLE_ENDIAN

typedef yarp::conf::float64_t NetFloat64;

#else // YARP_LITTLE_ENDIAN

typedef yarp::conf::float64_t RawNetFloat64;
union UnionNetFloat64
{
    yarp::conf::float64_t d;
    unsigned char c[8];
};
class YARP_os_API NetFloat64
{
private:
    double raw_value;
    double swap(double x) const;
    RawNetFloat64 get() const;
    void set(RawNetFloat64 v);

public:
    NetFloat64();
    NetFloat64(RawNetFloat64 val);
    operator RawNetFloat64() const;
    RawNetFloat64 operator+(RawNetFloat64 v) const;
    RawNetFloat64 operator-(RawNetFloat64 v) const;
    RawNetFloat64 operator*(RawNetFloat64 v) const;
    RawNetFloat64 operator/(RawNetFloat64 v) const;
    void operator+=(RawNetFloat64 v);
    void operator-=(RawNetFloat64 v);
    void operator*=(RawNetFloat64 v);
    void operator/=(RawNetFloat64 v);
};

#endif // YARP_LITTLE_ENDIAN

} // namespace os
} // namespace yarp

#endif // YARP_OS_NETFLOAT64_H
