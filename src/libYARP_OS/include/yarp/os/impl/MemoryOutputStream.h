// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Authors: Holger Friedrich
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_MEMORYOUTPUTSTREAM_
#define _YARP2_MEMORYOUTPUTSTREAM_

#include <yarp/os/OutputStream.h>
#include <yarp/os/Bytes.h>

namespace yarp {
    namespace os {
        namespace impl {
            class MemoryOutputStream;
        }
    }
}

/**
 * An OutputStream that writes to a given memory buffer address.  For speeding up.
 */
class yarp::os::impl::MemoryOutputStream : public yarp::os::OutputStream {
public:
    MemoryOutputStream(char* location): _location(location) {
    }
    using yarp::os::OutputStream::write;
    void write(const yarp::os::Bytes& b) {
        memcpy(_location, b.get(), b.length());
        _location += b.length();
    }
    void close() {}
    bool isOk() { return true; }
private:
    char *_location;
};

#endif

