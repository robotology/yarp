// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_MJPEGDECOMPRESSION_INC
#define YARP2_MJPEGDECOMPRESSION_INC

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/InputStream.h>
#include <yarp/sig/Image.h>

namespace yarp {
    namespace mjpeg {
        class MjpegDecompression;
    }
}

class yarp::mjpeg::MjpegDecompression {
private:
    void *system_resource;
public:
    MjpegDecompression();

    virtual ~MjpegDecompression();

    bool decompress(const yarp::os::Bytes& data,
                    yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

    bool isAutomatic() const;

    bool setReadEnvelopeCallback(yarp::os::InputStream::readEnvelopeCallbackType callback,
                                 void* data);
};

#endif
