/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NETFLOAT32_H
#define YARP_OS_NETFLOAT32_H

#include <yarp/conf/numeric.h>

#include <yarp/os/api.h>

////////////////////////////////////////////////////////////////////////
//
// The goal of this file is just to define a 32 bit signed little-endian
// IEC559/IEEE 754 floating point type.
//
////////////////////////////////////////////////////////////////////////

#if !YARP_FLOAT32_IS_IEC559
  // YARP assumes that floating point values are serialized as IEC 559/IEEE 754
  // floating point types.
  // If you receive the following error, this means that float and double, on
  // your platform, are not IEC 559, and therefore some conversion must be
  // performed whenever reading or writing a floating point value from the
  // network.
  // See, for example https://github.com/MalcolmMcLean/ieee754/ for a possible
  // implementation of the read and write methods.
  YARP_COMPILER_ERROR("Unsupported compiler. Please implement yarp::os::NetFloat32")
#endif

namespace yarp {
namespace os {

/**
 * Definition of the NetFloat32 type
 */

#ifdef YARP_LITTLE_ENDIAN

typedef yarp::conf::float32_t NetFloat32;

#else // YARP_LITTLE_ENDIAN

typedef yarp::conf::float32_t RawNetFloat32;
union UnionNetFloat32
{
    yarp::conf::float32_t d;
    unsigned char c[4];
};
class YARP_os_API NetFloat32
{
private:
    double raw_value;
    double swap(double x) const;
    RawNetFloat32 get() const;
    void set(RawNetFloat32 v);

public:
    NetFloat32();
    NetFloat32(RawNetFloat32 val);
    operator RawNetFloat32() const;
    RawNetFloat32 operator+(RawNetFloat32 v) const;
    RawNetFloat32 operator-(RawNetFloat32 v) const;
    RawNetFloat32 operator*(RawNetFloat32 v) const;
    RawNetFloat32 operator/(RawNetFloat32 v) const;
    void operator+=(RawNetFloat32 v);
    void operator-=(RawNetFloat32 v);
    void operator*=(RawNetFloat32 v);
    void operator/=(RawNetFloat32 v);
};

#endif // YARP_LITTLE_ENDIAN

} // namespace os
} // namespace yarp

#endif // YARP_OS_NETFLOAT32_H
