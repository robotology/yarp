// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_NETTYPE_
#define _YARP2_NETTYPE_

#include <yarp/String.h>
#include <yarp/Bytes.h>
#include <yarp/InputStream.h>
#include <yarp/IOException.h>
#include <yarp/Logger.h>

#include <ace/OS_NS_stdlib.h>

#include <yarp/os/NetInt32.h>

namespace yarp {
    class NetType;
}

/**
 * Various utilities related to types and formats.
 */
class yarp::NetType {
public:

    static int netInt(const Bytes& code) {
        YARP_ASSERT(code.length()==sizeof(NetType::NetInt32));
        NetType::NetInt32& i = *((NetType::NetInt32*)(code.get()));
        return i;
    }

    static void netInt(int data, const Bytes& code) {
        NetType::NetInt32 i = data;
        Bytes b((char*)(&i),sizeof(i));
        if (code.length()!=sizeof(i)) {
            throw IOException("not enough room for integer");
        }
        ACE_OS::memcpy(code.get(),b.get(),code.length());
    }

    static String readLine(InputStream& is, int terminal = '\n');

    static int readFull(InputStream& is, const Bytes& b);

    static int readDiscard(InputStream& is, int len);

    static String toString(int x);

    static String toString(unsigned int x);

    static int toInt(String x);

    static unsigned long int getCrc(char *buf, int len);

    typedef yarp::os::NetInt32 NetInt32;
};

#endif
