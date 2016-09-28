/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_INPUTSTREAM_H
#define YARP_OS_INPUTSTREAM_H

#include <yarp/conf/numeric.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        class InputStream;
    }
}

/**
 * Simple specification of the minimum functions needed from input streams.
 * The streams could be TCP, UDP, MCAST, ...
 */
class YARP_OS_API yarp::os::InputStream {
public:
    /**
     *
     * Constructor.
     *
     */
    InputStream() {
    }

    /**
     *
     * Destructor
     *
     */
    virtual ~InputStream() { }

    /**
     *
     * Perform maintenance actions, if needed.  This method is
     * called frequently by YARP when a stream is being accessed.
     * By default, it does nothing.
     *
     */
    virtual void check() {}

    /**
     *
     * Read and return a single byte.  Should block and wait for data.
     * By default, this calls read(const Bytes& b) to do its work.
     *
     * @return a byte from the stream (0-255), or -1 on failure.
     *
     */
    virtual int read() {
        unsigned char result;
        YARP_SSIZE_T ct = read(yarp::os::Bytes((char*)&result,1));
        if (ct<1) {
            return -1;
        }
        return result;
    }

    /**
     *
     * Read a block of data from the stream.  Should block and wait
     * for data. By default, this calls read(const Bytes& b) to do
     * its work.
     *
     * @param b the block of data to read to
     * @param offset an offset within the block to start at
     * @param len the number of bytes to read
     *
     * @return the number of bytes read, or -1 upon error
     *
     */
    virtual YARP_SSIZE_T read(const Bytes& b, size_t offset, YARP_SSIZE_T len) {
        return read(yarp::os::Bytes(b.get()+offset,len));
    }

    /**
     *
     * Read a block of data from the stream.  Should block and wait
     * for data.
     *
     * @param b the block of data to read to
     *
     * @return the number of bytes read, or -1 upon error
     *
     */
    virtual YARP_SSIZE_T read(const yarp::os::Bytes& b) = 0;


    /**
     *
     * Like read, but solicit partial responses.
     *
     */
    virtual YARP_SSIZE_T partialRead(const yarp::os::Bytes& b) {
        return read(b);
    }

    /**
     *
     * Terminate the stream.
     *
     */
    virtual void close() = 0;

    /**
     *
     * Interrupt the stream.  If the stream is currently in
     * a blocked state, it must be unblocked.
     *
     */
    virtual void interrupt() {}

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
    virtual bool setReadTimeout(double timeout) { return false; }

    /**
     *
     * Read a block of text terminated with a specific marker (or EOF).
     *
     */
    ConstString readLine(int terminal = '\n', bool *success = YARP_NULLPTR);

    /**
     *
     * Keep reading until buffer is full.
     *
     */
    YARP_SSIZE_T readFull(const Bytes& b);

    /**
     *
     * Read and discard a fixed number of bytes.
     *
     */
    YARP_SSIZE_T readDiscard(size_t len);

    /**
     *
     * Callback type for setting the envelope from a message in carriers that
     * cannot be escaped.
     *
     */
    typedef void (*readEnvelopeCallbackType)(void*, const yarp::os::Bytes& envelope);

    /**
     *
     * Install a callback that the InputStream will have to call when the
     * envelope is read from a message in carriers that cannot be escaped.
     *
     * @param callback the callback to execute
     * @param data a pointer that should be passed as first parameter to the
     *        \c callback function
     * @return true iff the \c callback was installed.
     *
     */
    virtual bool setReadEnvelopeCallback(readEnvelopeCallbackType callback, void* data) { return false; }
};

#endif // YARP_OS_INPUTSTREAM_H
