/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_OUTPUTSTREAM_H
#define YARP_OS_OUTPUTSTREAM_H

#include <yarp/os/api.h>

namespace yarp {
namespace os {

class Bytes;

/**
 * Simple specification of the minimum functions needed from output streams.
 * The streams could be TCP, UDP, MCAST, ...
 */
class YARP_os_API OutputStream
{
public:
    /**
     * Destructor
     */
    virtual ~OutputStream();

    /**
     * Write a single byte to the stream.  By default, this
     * calls write(const Bytes& b) to do its work.
     *
     * @param ch the byte to write
     */
    virtual void write(char ch);

    /**
     * Write a block of bytes to the stream.  By default, this
     * calls write(const Bytes& b) to do its work.
     *
     * @param b the bytes to write
     * @param offset an offset within the block to start at
     * @param len the number of bytes to write
     */
    virtual void write(const Bytes& b, int offset, int len);

    /**
     * Write a block of bytes to the stream.
     *
     * @param b the bytes to write
     */
    virtual void write(const yarp::os::Bytes& b) = 0;

    /**
     * Terminate the stream.
     */
    virtual void close() = 0;

    /**
     * Make sure all pending write operations are finished.
     */
    virtual void flush();

    /**
     * Write some text followed by a line feed.  By default,
     * this calls write(const Bytes& b) to do its work.
     *
     * @param data the text to write
     */
    virtual void writeLine(const char* data, int len);

    /**
     * Check if the stream is ok or in an error state.
     *
     * @return true iff the stream is ok
     */
    virtual bool isOk() const = 0;

    /**
     * Set activity timeout.  Support for this is optional. 0 = wait forever.
     * @return true iff timeout is supported.
     */
    virtual bool setWriteTimeout(double timeout);

    /*
     * Set the Type Of Service field of all outgoing packets.
     * @return true iff the TOS is set.
     */
    virtual bool setTypeOfService(int tos);

    /*
     * Get the Type Of Service field of all outgoing packets.
     * @return the TOS byte or -1 if not implemented.
     */
    virtual int getTypeOfService();
};


} // namespace os
} // namespace yarp

#endif // YARP_OS_OUTPUTSTREAM_H
