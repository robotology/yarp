// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_SHIFTSTREAM_
#define _YARP2_SHIFTSTREAM_

#include <yarp/os/impl/TwoWayStream.h>
#include <yarp/os/impl/IOException.h>

namespace yarp {
    namespace os {
        namespace impl {
            class ShiftStream;
        }
    }
}

/**
 * A container for a stream, allowing the stream implementation to
 * be replaced when needed.  This is important in YARP since
 * connections "bootstrap" from an initial stream type to
 * an optimized stream type with user-preferred properties and
 * trade-offs.
 */
class YARP_OS_impl_API yarp::os::impl::ShiftStream : public TwoWayStream {
public:
    /**
     * Constructor.
     */
    ShiftStream() {
        stream = NULL;
    }

    /**
     * Destructor.
     */
    virtual ~ShiftStream() {
        close();
    }

    /**
     * Perform maintenance actions, if needed.
     */
    virtual void check() {
    }

    virtual InputStream& getInputStream() {
        check();
        if (stream==NULL) return nullStream;
        return stream->getInputStream();
    }

    virtual OutputStream& getOutputStream() {
        check();
        if (stream==NULL) return nullStream;
        return stream->getOutputStream();
    }

    virtual const Address& getLocalAddress() {
        check();
        return (stream==NULL)?nullStream.getLocalAddress():(stream->getLocalAddress());
    }

    virtual const Address& getRemoteAddress() {
        check();
        return (stream==NULL)?nullStream.getRemoteAddress():(stream->getRemoteAddress());
    }

    virtual void close() {
        if (stream!=NULL) {
            stream->close();
            delete stream;
            stream = NULL;
        }
    }

    /**
     * Wrap the supplied stream.  If a stream is already wrapped,
     * it will be closed and destroyed.
     * @param stream the stream to wrap.
     */
    virtual void takeStream(TwoWayStream *stream) {
        close();
        this->stream = stream;
    }

    /**
     * Removes the wrapped stream and returns it.
     * The caller will be responsible for closing the stream.
     * @return the wrapped stream (which after this call will be the
     * caller's responsibility).
     */
    virtual TwoWayStream *giveStream() {
        TwoWayStream *result = stream;
        stream = NULL;
        return result;
    }

    /**
     * @return the wrapped stream (which after this call will remain
     * this container's responsibility - compare with giveStream).
     */
    virtual TwoWayStream *getStream() {
        return stream;
    }

    /**
     * @return true if there is no wrapped stream.
     */
    virtual bool isEmpty() {
        return stream==NULL;
    }

    virtual bool isOk() {
        if (stream!=NULL) {
            return stream->isOk();
        }
        return false;
    }

    virtual void reset() {
        if (stream!=NULL) {
            stream->reset();
        }
    }

    virtual void beginPacket() {
        if (stream!=NULL) {
            stream->beginPacket();
        }
    }

    virtual void endPacket() {
        if (stream!=NULL) {
            stream->endPacket();
        }
    }

private:
    TwoWayStream *stream;
    NullStream nullStream;
};

#endif
