/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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

#include <yarp/wire_rep_utils/BlobNetworkHeader.h>

#include "MjpegDecompression.h"

class MjpegStream :
        public yarp::os::TwoWayStream,
        public yarp::os::InputStream,
        public yarp::os::OutputStream
{
private:
    yarp::os::TwoWayStream *delegate;
    yarp::os::StringInputStream sis;
    yarp::os::StringOutputStream sos;
    yarp::sig::FlexImage img;
    yarp::sig::ImageNetworkHeader imgHeader;
    yarp::wire_rep_utils::BlobNetworkHeader blobHeader;
    yarp::os::ManagedBytes cimg;
    MjpegDecompression decompression;
    int phase;
    char *cursor;
    int remaining;
    bool autocompress;
    yarp::os::Bytes envelope;
public:
    MjpegStream(TwoWayStream *delegate, bool autocompress) :
            delegate(delegate),
            blobHeader(yarp::wire_rep_utils::BlobNetworkHeader{0,0,0}),
            phase(0),
            cursor(NULL),
            remaining(0),
            autocompress(autocompress)
    {}

    virtual ~MjpegStream() {
        delete delegate;
    }

    yarp::os::InputStream& getInputStream() override { return *this; }
    yarp::os::OutputStream& getOutputStream() override { return *this; }


    const yarp::os::Contact& getLocalAddress() const override {
        return delegate->getLocalAddress();
    }

    const yarp::os::Contact& getRemoteAddress() const override {
        return delegate->getRemoteAddress();
    }

    bool isOk() const override {
        return delegate->isOk();
    }

    void reset() override {
        delegate->reset();
    }

    void close() override {
        delegate->close();
    }

    void beginPacket() override {
        delegate->beginPacket();
    }

    void endPacket() override {
        delegate->endPacket();
    }

    using yarp::os::OutputStream::write;
    void write(const yarp::os::Bytes& b) override;

    using yarp::os::InputStream::read;
    yarp::conf::ssize_t read(yarp::os::Bytes& b) override;

    void interrupt() override {
        delegate->getInputStream().interrupt();
    }

    bool setReadEnvelopeCallback(yarp::os::InputStream::readEnvelopeCallbackType callback, void* data) override {
        if (!autocompress) {
            return false;
        }
        return decompression.setReadEnvelopeCallback(callback, data);
    }
};

#endif
