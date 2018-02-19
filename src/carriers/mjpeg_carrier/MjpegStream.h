/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef MJPEGSTREAM_INC
#define MJPEGSTREAM_INC

#include <yarp/os/InputStream.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>

#include "BlobNetworkHeader.h"

#include "MjpegDecompression.h"

namespace yarp {
    namespace os {
        class MjpegStream;
    }
}

class yarp::os::MjpegStream : public TwoWayStream,
                              public InputStream,
                              public OutputStream
{
private:
    TwoWayStream *delegate;
    StringInputStream sis;
    StringOutputStream sos;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> img;
    yarp::sig::ImageNetworkHeader imgHeader;
    BlobNetworkHeader blobHeader;
    ManagedBytes cimg;
    yarp::mjpeg::MjpegDecompression decompression;
    int phase;
    char *cursor;
    int remaining;
    bool sender;
    bool firstRound;
    bool autocompress;
    yarp::os::Bytes envelope;
    readEnvelopeCallbackType readEnvelopeCallback;
    void* readEnvelopeCallbackData;
public:
    MjpegStream(TwoWayStream *delegate, bool sender, bool autocompress) :
            delegate(delegate),
            blobHeader(BlobNetworkHeader{0,0,0}),
            phase(0),
            cursor(NULL),
            remaining(0),
            sender(sender),
            firstRound(true),
            autocompress(autocompress),
            readEnvelopeCallback(NULL),
            readEnvelopeCallbackData(NULL)
    {}

    virtual ~MjpegStream() {
        if (delegate!=NULL) {
            delete delegate;
            delegate = NULL;
        }
    }

    virtual InputStream& getInputStream() override { return *this; }
    virtual OutputStream& getOutputStream() override { return *this; }


    virtual const Contact& getLocalAddress() override {
        return delegate->getLocalAddress();
    }

    virtual const Contact& getRemoteAddress() override {
        return delegate->getRemoteAddress();
    }

    virtual bool isOk() override {
        return delegate->isOk();
    }

    virtual void reset() override {
        delegate->reset();
    }

    virtual void close() override {
        delegate->close();
    }

    virtual void beginPacket() override {
        delegate->beginPacket();
    }

    virtual void endPacket() override {
        delegate->endPacket();
    }

    using yarp::os::OutputStream::write;
    virtual void write(const Bytes& b) override;

    using yarp::os::InputStream::read;
    virtual YARP_SSIZE_T read(const Bytes& b) override;

    virtual void interrupt() override {
        delegate->getInputStream().interrupt();
    }

    virtual bool setReadEnvelopeCallback(InputStream::readEnvelopeCallbackType callback, void* data) override {
        if (!autocompress) {
            return false;
        }
        return decompression.setReadEnvelopeCallback(callback, data);
    }
};

#endif
