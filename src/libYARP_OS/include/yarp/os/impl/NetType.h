// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_NETTYPE_
#define _YARP2_NETTYPE_

#include <yarp/os/impl/String.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/impl/InputStream.h>
#include <yarp/os/impl/Logger.h>

#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/PlatformSize.h>

#include <yarp/os/NetInt32.h>
#include <yarp/os/NetFloat64.h>
#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        namespace impl {
            class NetType;
        }
    }
}

/**
 * Various utilities related to types and formats.
 */
class YARP_OS_impl_API yarp::os::impl::NetType {
public:

    static int netInt(const yarp::os::Bytes& code) {
        YARP_ASSERT(code.length()==sizeof(NetType::NetInt32));
        //// this does not work on Solaris with gcc 3.2
        //NetType::NetInt32& i = *((NetType::NetInt32*)(code.get()));
        //return i;
        NetType::NetInt32 tmp;
        ACE_OS::memcpy((char*)(&tmp),code.get(),code.length());
        return tmp;
    }

    static bool netInt(int data, const yarp::os::Bytes& code) {
        NetType::NetInt32 i = data;
        yarp::os::Bytes b((char*)(&i),sizeof(i));
        if (code.length()!=sizeof(i)) {
            YARP_ERROR(Logger::get(),"not enough room for integer");
            return false;
        }
        ACE_OS::memcpy(code.get(),b.get(),code.length());
        return true;
    }

    static String readLine(InputStream& is, int terminal = '\n',
                           bool *success = NULL);

    static ssize_t readFull(InputStream& is, const yarp::os::Bytes& b);

    static ssize_t readDiscard(InputStream& is, size_t len);

    static String toHexString(int x);

    static String toString(int x);

    static String toString(long x);

    static String toString(unsigned int x);

    static int toInt(const char *x);

    static unsigned long int getCrc(char *buf, size_t len);

    typedef yarp::os::NetInt32 NetInt32;
    typedef yarp::os::NetFloat64 NetFloat64;
};

#endif
