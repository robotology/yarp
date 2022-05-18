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

namespace yarp::os {

/**
 * Various utilities related to types and formats.
 */
class YARP_os_API NetType
{
public:
    static int netInt(const yarp::os::Bytes& code);
    static bool netInt(int data, yarp::os::Bytes& code);

    static unsigned long int getCrc(char* buf, size_t len);

};

} // namespace yarp::os

#endif // YARP_OS_NETTYPE_H
