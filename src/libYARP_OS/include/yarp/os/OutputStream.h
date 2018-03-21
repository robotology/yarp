/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_OUTPUTSTREAM_H
#define YARP_OS_OUTPUTSTREAM_H

#include <yarp/os/Bytes.h>

namespace yarp {
    namespace os {
        class OutputStream;
    }
}

/**
 * Simple specification of the minimum functions needed from output streams.
 * The streams could be TCP, UDP, MCAST, ...
 */
class YARP_OS_API yarp::os::OutputStream {
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
        write(yarp::os::Bytes(&ch, 1));
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
        write(yarp::os::Bytes(b.get()+offset, len));
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
    virtual void writeLine(const char *data, int len) {
        yarp::os::Bytes b((char*)data, len);
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
    virtual bool setWriteTimeout(double timeout) { YARP_UNUSED(timeout); return false; }

    /*
     *
     * Set the Type Of Service field of all outgoing packets.
     * @return true iff the TOS is set.
     */
    virtual bool setTypeOfService(int tos) { YARP_UNUSED(tos); return false; }

    /*
     *
     * Get the Type Of Service field of all outgoing packets.
     * @return the TOS byte or -1 if not implemented.
     */
    virtual int getTypeOfService() { return -1; }
};

#endif // YARP_OS_OUTPUTSTREAM_H
