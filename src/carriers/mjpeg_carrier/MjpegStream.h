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
#include <yarp/os/impl/TwoWayStream.h>
#include <yarp/os/impl/StringInputStream.h>
#include <yarp/os/impl/StringOutputStream.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/PortCommand.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>

#include "BlobNetworkHeader.h"

#include "MjpegDecompression.h"

namespace yarp {
    namespace os {
        namespace impl {
            class MjpegStream;
        }
    }
}

class yarp::os::impl::MjpegStream : public TwoWayStream,
                                    public yarp::os::InputStream,
                                    public yarp::os::OutputStream
{
private:
    TwoWayStream *delegate;
    StringInputStream sis;
    StringOutputStream sos;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> img;
    yarp::sig::ImageNetworkHeader imgHeader;
    BlobNetworkHeader blobHeader;
    yarp::os::ManagedBytes cimg;
    yarp::mjpeg::MjpegDecompression decompression;
    int phase;
    char *cursor;
    int remaining;
    bool sender;
    bool firstRound;
    bool autocompress;
public:
    MjpegStream(TwoWayStream *delegate, bool sender,
                bool autocompress) : sender(sender),
                                     autocompress(autocompress) {
        this->delegate = delegate;
        firstRound = true;
        phase = 0;
        cursor = NULL;
        remaining = 0;
    }

    virtual ~MjpegStream() {
        if (delegate!=NULL) {
            delete delegate;
            delegate = NULL;
        }
    }

    virtual yarp::os::InputStream& getInputStream() { return *this; }
    virtual yarp::os::OutputStream& getOutputStream() { return *this; }


    virtual const Address& getLocalAddress() {
        return delegate->getLocalAddress();
    }

    virtual const Address& getRemoteAddress() {
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

    virtual void write(const Bytes& b);

    virtual ssize_t read(const Bytes& b);

    virtual void interrupt() {
        delegate->getInputStream().interrupt();
    }

};

#endif
