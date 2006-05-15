// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_SHIFTSTREAM_
#define _YARP2_SHIFTSTREAM_

#include <yarp/TwoWayStream.h>
#include <yarp/IOException.h>

namespace yarp {
    class ShiftStream;
}

/**
 * Container for a replacable I/O stream.
 * Classes implementing this interface can 
 * have their streams "borrowed" or replaced.
 */
class yarp::ShiftStream : public TwoWayStream {
public:
    ShiftStream() {
        stream = NULL;
    }

    virtual ~ShiftStream() {
        close();
    }

    void check() {
        if (stream==NULL) {
            throw IOException("no stream present");
        }
    }

    virtual InputStream& getInputStream() {
        check();
        return stream->getInputStream();
    }

    virtual OutputStream& getOutputStream() {
        check();
        return stream->getOutputStream();
    }

    virtual const Address& getLocalAddress() {
        check();
        return stream->getLocalAddress();
    }

    virtual const Address& getRemoteAddress() {
        check();
        return stream->getRemoteAddress();
    }

    virtual void close() {
        if (stream!=NULL) {
            stream->close();
            delete stream;
            stream = NULL;
        }
    }

    virtual void takeStream(TwoWayStream *stream) {
        close();
        this->stream = stream;
    }

    virtual TwoWayStream *giveStream() {
        TwoWayStream *result = stream;
        stream = NULL;
        return result;
    }

    virtual TwoWayStream *getStream() {
        return stream;
    }

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
};

#endif
