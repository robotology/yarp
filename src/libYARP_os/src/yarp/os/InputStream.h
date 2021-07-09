/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_INPUTSTREAM_H
#define YARP_OS_INPUTSTREAM_H

#include <yarp/conf/numeric.h>

#include <yarp/os/api.h>

#include <string>

namespace yarp {
namespace os {

class Bytes;

/**
 * Simple specification of the minimum functions needed from input streams.
 * The streams could be TCP, UDP, MCAST, ...
 */
class YARP_os_API InputStream
{
public:
    /**
     * Constructor.
     */
    InputStream();

    /**
     * Destructor
     */
    virtual ~InputStream();

    /**
     * Perform maintenance actions, if needed.  This method is
     * called frequently by YARP when a stream is being accessed.
     * By default, it does nothing.
     */
    virtual void check();

    /**
     * Read and return a single byte.  Should block and wait for data.
     * By default, this calls read(Bytes& b) to do its work.
     *
     * @return a byte from the stream (0-255), or -1 on failure.
     */
    virtual int read();

    /**
     * Read a block of data from the stream.  Should block and wait
     * for data. By default, this calls read(Bytes& b) to do
     * its work.
     *
     * @param[out] b the block of data to read to
     * @param offset an offset within the block to start at
     * @param len the number of bytes to read
     *
     * @return the number of bytes read, or -1 upon error
     */
    virtual yarp::conf::ssize_t read(Bytes& b, size_t offset, yarp::conf::ssize_t len);

    /**
     * Read a block of data from the stream.  Should block and wait
     * for data.
     *
     * @param b[out] the block of data to read to
     *
     * @return the number of bytes read, or -1 upon error
     */
    virtual yarp::conf::ssize_t read(yarp::os::Bytes& b) = 0;


    /**
     * Like read, but solicit partial responses.
     */
    virtual yarp::conf::ssize_t partialRead(yarp::os::Bytes& b);

    /**
     * Terminate the stream.
     */
    virtual void close() = 0;

    /**
     * Interrupt the stream.  If the stream is currently in
     * a blocked state, it must be unblocked.
     */
    virtual void interrupt();

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
    virtual bool setReadTimeout(double timeout);

    /**
     * Read a block of text terminated with a specific marker (or EOF).
     */
    std::string readLine(const char terminal = '\n', bool* success = nullptr);

    /**
     * Keep reading until buffer is full.
     */
    yarp::conf::ssize_t readFull(Bytes& b);

    /**
     * Read and discard a fixed number of bytes.
     */
    yarp::conf::ssize_t readDiscard(size_t len);

    /**
     * Callback type for setting the envelope from a message in carriers that
     * cannot be escaped.
     */
    typedef void (*readEnvelopeCallbackType)(void*, const yarp::os::Bytes& envelope);

    /**
     * Install a callback that the InputStream will have to call when the
     * envelope is read from a message in carriers that cannot be escaped.
     *
     * @param callback the callback to execute
     * @param data a pointer that should be passed as first parameter to the
     *        \c callback function
     * @return true iff the \c callback was installed.
     */
    virtual bool setReadEnvelopeCallback(readEnvelopeCallbackType callback, void* data);
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_INPUTSTREAM_H
