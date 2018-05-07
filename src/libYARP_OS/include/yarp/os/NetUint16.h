/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
// If you are having trouble with it, and your system has a 16 bit unsigned
// little-endian type called e.g. ___my_system_uint16, you can replace
// this whole file with:
//    typedef ___my_system_uint16 NetUint16;
//
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//   YARP_BIG_ENDIAN should be defined if we are big endian
//   YARP_LITTLE_ENDIAN should be defined if we are little endian

namespace yarp {
namespace os {
/**
 * Definition of the NetUint16 type
 */

#ifdef YARP_LITTLE_ENDIAN

typedef std::uint16_t NetUint16;

#else // YARP_LITTLE_ENDIAN

class YARP_OS_API NetUint16 {
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
