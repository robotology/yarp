/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NETTYPE_H
#define YARP_OS_NETTYPE_H

#include <yarp/conf/numeric.h>

#include <yarp/os/Bytes.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/NetFloat64.h>
#include <yarp/os/NetInt32.h>

#include <string>

namespace yarp {
namespace os {

/**
 * Various utilities related to types and formats.
 */
class YARP_os_API NetType
{
public:
    static int netInt(const yarp::os::Bytes& code);
    static bool netInt(int data, yarp::os::Bytes& code);

    static unsigned long int getCrc(char* buf, size_t len);

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5
    YARP_DEPRECATED_MSG("Use yarp::conf::numeric::to_hex_string instead")
    static std::string toHexString(int x);

    YARP_DEPRECATED_MSG("Use yarp::conf::numeric::to_hex_string instead")
    static std::string toHexString(long x);

    YARP_DEPRECATED_MSG("Use yarp::conf::numeric::to_hex_string instead")
    static std::string toHexString(unsigned int x);

    YARP_DEPRECATED_MSG("Use yarp::conf::numeric::to_string instead")
    static std::string toString(int x);

    YARP_DEPRECATED_MSG("Use yarp::conf::numeric::to_string instead")
    static std::string toString(long x);

    YARP_DEPRECATED_MSG("Use yarp::conf::numeric::to_string instead")
    static std::string toString(unsigned int x);

    YARP_DEPRECATED_MSG("Use yarp::conf::numeric::to_string instead")
    static std::string toString(yarp::conf::float32_t x);

    YARP_DEPRECATED_MSG("Use yarp::conf::numeric::to_string instead")
    static std::string toString(yarp::conf::float64_t x);

    YARP_DEPRECATED_MSG("Use yarp::conf::numeric::from_string instead")
    static yarp::conf::float32_t toFloat32(const std::string& s);

    YARP_DEPRECATED_MSG("Use yarp::conf::numeric::from_string instead")
    static yarp::conf::float64_t toFloat64(const std::string& s);

    YARP_DEPRECATED_MSG("Use yarp::conf::numeric::from_string instead")
    static yarp::conf::float32_t toFloat32(std::string&& s);

    YARP_DEPRECATED_MSG("Use yarp::conf::numeric::from_string instead")
    static yarp::conf::float64_t toFloat64(std::string&& s);

    YARP_DEPRECATED_MSG("Use yarp::conf::numeric::from_string instead")
    static int toInt(const std::string& x);
#endif // YARP_NO_DEPRECATED
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_NETTYPE_H
