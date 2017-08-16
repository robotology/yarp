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
    ShiftStream() : stream(nullptr)
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

    virtual InputStream& getInputStream() override {
        check();
        if (stream == nullptr) {
            return nullStream;
        }
        return stream->getInputStream();
    }

    virtual OutputStream& getOutputStream() override {
        check();
        if (stream == nullptr) {
            return nullStream;
        }
        return stream->getOutputStream();
    }

    virtual const Contact& getLocalAddress() override {
        check();
        return (stream == nullptr) ? nullStream.getLocalAddress()
                                        : (stream->getLocalAddress());
    }

    virtual const Contact& getRemoteAddress() override {
        check();
        return (stream == nullptr) ? nullStream.getRemoteAddress()
                                        : (stream->getRemoteAddress());
    }

    virtual void close() override {
        if (stream != nullptr) {
            stream->close();
            delete stream;
            stream = nullptr;
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
        stream = nullptr;
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
        return stream == nullptr;
    }

    virtual bool isOk() override {
        if (stream != nullptr) {
            return stream->isOk();
        }
        return false;
    }

    virtual void reset() override {
        if (stream != nullptr) {
            stream->reset();
        }
    }

    virtual void beginPacket() override {
        if (stream != nullptr) {
            stream->beginPacket();
        }
    }

    virtual void endPacket() override {
        if (stream != nullptr) {
            stream->endPacket();
        }
    }

private:
    TwoWayStream *stream;
    NullStream nullStream;
};

#endif // YARP_OS_SHIFTSTREAM_H
