/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_SHIFTSTREAM_H
#define YARP_OS_SHIFTSTREAM_H

#include <yarp/os/TwoWayStream.h>

namespace yarp {
    namespace os {
        class ShiftStream;
    }
}

/**
 * A container for a stream, allowing the stream implementation to
 * be replaced when needed.  This is important in YARP since
 * connections "bootstrap" from an initial stream type to
 * an optimized stream type with user-preferred properties and
 * trade-offs.
 */
class YARP_OS_API yarp::os::ShiftStream : public TwoWayStream {
public:
    /**
     * Constructor.
     */
    ShiftStream() : stream(YARP_NULLPTR)
    {
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
        if (stream == YARP_NULLPTR) {
            return nullStream;
        }
        return stream->getInputStream();
    }

    virtual OutputStream& getOutputStream() {
        check();
        if (stream == YARP_NULLPTR) {
            return nullStream;
        }
        return stream->getOutputStream();
    }

    virtual const Contact& getLocalAddress() {
        check();
        return (stream == YARP_NULLPTR) ? nullStream.getLocalAddress()
                                        : (stream->getLocalAddress());
    }

    virtual const Contact& getRemoteAddress() {
        check();
        return (stream == YARP_NULLPTR) ? nullStream.getRemoteAddress()
                                        : (stream->getRemoteAddress());
    }

    virtual void close() {
        if (stream != YARP_NULLPTR) {
            stream->close();
            delete stream;
            stream = YARP_NULLPTR;
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
        stream = YARP_NULLPTR;
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
        return stream == YARP_NULLPTR;
    }

    virtual bool isOk() {
        if (stream != YARP_NULLPTR) {
            return stream->isOk();
        }
        return false;
    }

    virtual void reset() {
        if (stream != YARP_NULLPTR) {
            stream->reset();
        }
    }

    virtual void beginPacket() {
        if (stream != YARP_NULLPTR) {
            stream->beginPacket();
        }
    }

    virtual void endPacket() {
        if (stream != YARP_NULLPTR) {
            stream->endPacket();
        }
    }

private:
    TwoWayStream *stream;
    NullStream nullStream;
};

#endif // YARP_OS_SHIFTSTREAM_H
