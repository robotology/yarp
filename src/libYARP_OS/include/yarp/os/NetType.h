/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_NETTYPE_H
#define YARP_OS_NETTYPE_H

#include <yarp/conf/numeric.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/InputStream.h>

#include <yarp/os/NetInt32.h>
#include <yarp/os/NetFloat64.h>
#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        class NetType;
    }
}

/**
 * Various utilities related to types and formats.
 */
class YARP_OS_API yarp::os::NetType {
public:

    static int netInt(const yarp::os::Bytes& code);
    static bool netInt(int data, const yarp::os::Bytes& code);
    static ConstString toHexString(int x);
    static ConstString toString(int x);
    static ConstString toString(long x);
    static ConstString toString(unsigned int x);
    static int toInt(const ConstString& x);
    static unsigned long int getCrc(char *buf, size_t len);
};

#endif // YARP_OS_NETTYPE_H
