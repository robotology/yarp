/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
