/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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
#include <yarp/wire_rep_utils/BlobNetworkHeader.h>
#include "H264Decoder.h"
#include <yarp/os/InputStream.h>


class H264Stream :
        public yarp::os::impl::DgramTwoWayStream
{
private:

    DgramTwoWayStream *delegate;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> img;
    yarp::sig::ImageNetworkHeader imgHeader;
    yarp::wire_rep_utils::BlobNetworkHeader blobHeader;
    int phase;
    char *cursor;
    size_t remaining;
    H264Decoder *decoder;
    h264Decoder_cfgParamters cfg;
public:
    H264Stream(h264Decoder_cfgParamters &config);

    virtual ~H264Stream();

    bool setStream(yarp::os::impl::DgramTwoWayStream *stream);

    void start();

    InputStream& getInputStream() override;
    OutputStream& getOutputStream() override;

    using yarp::os::OutputStream::write;
    void write(const yarp::os::Bytes& b) override;

    using yarp::os::InputStream::read;
    yarp::conf::ssize_t read(yarp::os::Bytes& b) override;

    bool setReadEnvelopeCallback(InputStream::readEnvelopeCallbackType callback, void* data) override;

};

#endif
