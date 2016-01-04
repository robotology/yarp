// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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
            phase(0),
            cursor(NULL),
            remaining(0),
            sender(sender),
            firstRound(true),
            autocompress(autocompress),
            readEnvelopeCallback(NULL),
            readEnvelopeCallbackData(NULL) {
    }

    virtual ~MjpegStream() {
        if (delegate!=NULL) {
            delete delegate;
            delegate = NULL;
        }
    }

    virtual InputStream& getInputStream() { return *this; }
    virtual OutputStream& getOutputStream() { return *this; }


    virtual const Contact& getLocalAddress() {
        return delegate->getLocalAddress();
    }

    virtual const Contact& getRemoteAddress() {
        return delegate->getRemoteAddress();
    }

    virtual bool isOk() {
        return delegate->isOk();
    }

    virtual void reset() {
        delegate->reset();
    }

    virtual void close() {
        delegate->close();
    }

    virtual void beginPacket() {
        delegate->beginPacket();
    }

    virtual void endPacket() {
        delegate->endPacket();
    }

    using yarp::os::OutputStream::write;
    virtual void write(const Bytes& b);

    using yarp::os::InputStream::read;
    virtual YARP_SSIZE_T read(const Bytes& b);

    virtual void interrupt() {
        delegate->getInputStream().interrupt();
    }

    virtual bool setReadEnvelopeCallback(InputStream::readEnvelopeCallbackType callback, void* data) {
        if (!autocompress) {
            return false;
        }
        return decompression.setReadEnvelopeCallback(callback, data);
    }
};

#endif
