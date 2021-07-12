/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NETINT32_H
#define YARP_OS_NETINT32_H

#include <yarp/conf/numeric.h>

#include <yarp/os/api.h>

////////////////////////////////////////////////////////////////////////
//
// The goal of this file is just to define a 32 bit signed little-endian
// integer type.
//
////////////////////////////////////////////////////////////////////////

namespace yarp {
namespace os {

/**
 * Definition of the NetInt32 type
 */

#ifdef YARP_LITTLE_ENDIAN

typedef std::int32_t NetInt32;

#else // YARP_LITTLE_ENDIAN

class YARP_os_API NetInt32
{
private:
    std::uint32_t raw_value;
    std::uint32_t swap(std::uint32_t x) const;
    std::int32_t get() const;
    void set(std::int32_t v);

public:
    NetInt32();
    NetInt32(std::int32_t val);
    operator std::int32_t() const;
    std::int32_t operator+(std::int32_t v) const;
    std::int32_t operator-(std::int32_t v) const;
    std::int32_t operator*(std::int32_t v) const;
    std::int32_t operator/(std::int32_t v) const;
    void operator+=(std::int32_t v);
    void operator-=(std::int32_t v);
    void operator*=(std::int32_t v);
    void operator/=(std::int32_t v);
    void operator++(int);
    void operator--(int);
};

#endif // YARP_LITTLE_ENDIAN

} // namespace os
} // namespace yarp

#endif // YARP_OS_NETINT32_H
