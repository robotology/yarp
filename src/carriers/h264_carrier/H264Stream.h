/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef H264STREAM_INC
#define H264STREAM_INC

#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>
#include "BlobNetworkHeader.h"
#include "H264Decoder.h"
#include <yarp/os/InputStream.h>


namespace yarp {
    namespace os {
        class H264Stream;
    }
}


class yarp::os::H264Stream : public yarp::os::impl::DgramTwoWayStream
{
private:

    DgramTwoWayStream *delegate;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> img;
    yarp::sig::ImageNetworkHeader imgHeader;
    BlobNetworkHeader blobHeader;
    int phase;
    char *cursor;
    int remaining;
    H264Decoder *decoder;
    int remotePort;
    h264Decoder_cfgParamters cfg;
public:
    H264Stream(h264Decoder_cfgParamters &config);

    virtual ~H264Stream();

    bool setStream(yarp::os::impl::DgramTwoWayStream *stream);

    void start (void);

    virtual InputStream& getInputStream() override;
    virtual OutputStream& getOutputStream() override;

    using yarp::os::OutputStream::write;
    virtual void write(const Bytes& b) override;

    using yarp::os::InputStream::read;
    virtual yarp::conf::ssize_t read(Bytes& b) override;

    virtual bool setReadEnvelopeCallback(InputStream::readEnvelopeCallbackType callback, void* data) override;

};

#endif
