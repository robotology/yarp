// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_OUTPUTSTREAM_
#define _YARP2_OUTPUTSTREAM_

#include <yarp/os/Bytes.h>
#include <yarp/os/impl/String.h>

namespace yarp {
    namespace os {
        namespace impl {
            class OutputStream;
        }
    }
}

/**
 * Simple specification of the minimum functions needed from output streams.
 * The streams could be TCP, UDP, MCAST, ...
 */
class YARP_OS_impl_API yarp::os::impl::OutputStream {
public:

    /**
     *
     * Destructor
     *
     */
    virtual ~OutputStream() { }

    /**
     *
     * Write a single byte to the stream.  By default, this
     * calls write(const Bytes& b) to do its work.
     *
     * @param ch the byte to write
     *
     */
    virtual void write(char ch) {
        write(yarp::os::Bytes(&ch,1));
    }

    /**
     *
     * Write a block of bytes to the stream.  By default, this
     * calls write(const Bytes& b) to do its work.
     *
     * @param b the bytes to write
     * @param offset an offset within the block to start at
     * @param len the number of bytes to write
     *
     */
    virtual void write(const Bytes& b, int offset, int len) {
        write(yarp::os::Bytes(b.get()+offset,len));
    }

    /**
     *
     * Write a block of bytes to the stream.
     *
     * @param b the bytes to write
     *
     */
    virtual void write(const yarp::os::Bytes& b) = 0;

    /**
     *
     * Terminate the stream.
     *
     */
    virtual void close() = 0;

    /**
     *
     * Make sure all pending write operations are finished.
     *
     */
    virtual void flush() {
    }

    /**
     *
     * Write some text followed by a line feed.  By default,
     * this calls write(const Bytes& b) to do its work.
     *
     * @param data the text to write
     *
     */
    virtual void writeLine(const String& data) {
        yarp::os::Bytes b((char*)(data.c_str()),data.length());
        write(b);
        write('\n');
    }

    /**
     *
     * Check if the stream is ok or in an error state.
     *
     * @return true iff the stream is ok
     *
     */
    virtual bool isOk() = 0;

    /**
     *
     * Set activity timeout.  Support for this is optional. 0 = wait forever.
     * @return true iff timeout is supported.
     *
     */
    virtual bool setWriteTimeout(double timeout) { return false; }
};

#endif
