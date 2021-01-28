/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP2_MJPEGDECOMPRESSION_INC
#define YARP2_MJPEGDECOMPRESSION_INC

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/InputStream.h>
#include <yarp/sig/Image.h>


class MjpegDecompression
{
private:
    void *system_resource;
public:
    MjpegDecompression();

    virtual ~MjpegDecompression();

    bool decompress(const yarp::os::Bytes& data,
                    yarp::sig::FlexImage& image);

    bool isAutomatic() const;

    bool setReadEnvelopeCallback(yarp::os::InputStream::readEnvelopeCallbackType callback,
                                 void* data);
};

#endif
