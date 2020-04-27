/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

    static std::string toHexString(int x);
    static std::string toHexString(long x);
    static std::string toHexString(unsigned int x);

    static std::string toString(int x);
    static std::string toString(long x);
    static std::string toString(unsigned int x);

    static std::string toString(yarp::conf::float32_t x);
    static std::string toString(yarp::conf::float64_t x);

    static yarp::conf::float32_t toFloat32(const std::string& s);
    static yarp::conf::float64_t toFloat64(const std::string& s);
    static yarp::conf::float32_t toFloat32(std::string&& s);
    static yarp::conf::float64_t toFloat64(std::string&& s);

    static int toInt(const std::string& x);
    static unsigned long int getCrc(char* buf, size_t len);
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_NETTYPE_H
