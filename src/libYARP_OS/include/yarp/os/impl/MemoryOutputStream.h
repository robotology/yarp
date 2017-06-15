/*
 * Authors: Holger Friedrich
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_MEMORYOUTPUTSTREAM_H
#define YARP_OS_IMPL_MEMORYOUTPUTSTREAM_H

#include <yarp/os/OutputStream.h>
#include <yarp/os/Bytes.h>

#include <cstring>

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
    void write(const yarp::os::Bytes& b) override {
        memcpy(_location, b.get(), b.length());
        _location += b.length();
    }
    void close() override {}
    bool isOk() override { return true; }
private:
    char *_location;
};

#endif // YARP_OS_IMPL_MEMORYOUTPUTSTREAM_H
