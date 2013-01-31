// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_TWOWAYSTREAM_
#define _YARP2_TWOWAYSTREAM_

#include <yarp/os/impl/Address.h>
#include <yarp/os/impl/InputStream.h>
#include <yarp/os/impl/OutputStream.h>

namespace yarp {
    namespace os {
        namespace impl {
            class TwoWayStream;
            class NullStream;
        }
    }
}

/**
 * A stream which can be asked to perform bidirectional communication.
 * It need not actually be capable of communication in both directions,
 * in which case it should fail if requested to communicate in an
 * unsupported direction.
 */
class YARP_OS_impl_API yarp::os::impl::TwoWayStream {
public:
    /**
     * Destructor.
     */
    virtual ~TwoWayStream() {
    }

    /**
     * Get an InputStream to read from.
     *
     * @return the InputStream associated with this object.
     */
    virtual InputStream& getInputStream() = 0;

    /**
     * Get an OutputStream to write to.
     *
     * @return the InputStream associated with this object.
     */
    virtual OutputStream& getOutputStream() = 0;

    /**
     * Get the address of the local side of the stream.
     *
     * @return the address of the local side of the stream.
     * The address will be tagged as invalid if the stream is not set up.
     */
    virtual const Address& getLocalAddress() = 0;

    /**
     * Get the address of the remote side of the stream.
     *
     * @return the address of the remote side of the stream.
     * The address will be tagged as invalid if the stream is not set up.
     */
    virtual const Address& getRemoteAddress() = 0;

    /**
     *
     * Check if the stream is ok or in an error state.
     *
     * @return true iff the stream is ok
     *
     */
    virtual bool isOk() = 0;

    /**
     * Reset the stream.
     */
    virtual void reset() = 0;

    /**
     *
     * Terminate the stream.
     *
     */
    virtual void close() = 0;

    /**
     *
     * Mark the beginning of a logical packet.  This is a unit that
     * should be treated in an atomic fashion by YARP.  If any part
     * is corrupted, the whole should be dropped.
     *
     */
    virtual void beginPacket() = 0;

    /**
     *
     * Mark the end of a logical packet (see beginPacket).
     *
     */
    virtual void endPacket() = 0;
};


/**
 * A "null" stream, always invalid.
 */
class yarp::os::impl::NullStream : public TwoWayStream,
            public InputStream, public OutputStream {
private:
    Address address;
public:
    virtual ~NullStream() {
    }

    virtual InputStream& getInputStream()   { return *this; }
    virtual OutputStream& getOutputStream() { return * this; }

    virtual const Address& getLocalAddress()  { return address; }
    virtual const Address& getRemoteAddress() { return address; }

    virtual bool isOk() { return false; }

    virtual void reset() {}

    virtual void close() {}

    // These should be called at the beginning and end of logical packets.
    // Streams are encouraged to handle errors and atomicity at the level of
    // logical packets
    virtual void beginPacket() {}
    virtual void endPacket()   {}

    virtual ssize_t read(const Bytes& b)  { return -1; }
    virtual void write(const Bytes& b) { }

};

#endif
