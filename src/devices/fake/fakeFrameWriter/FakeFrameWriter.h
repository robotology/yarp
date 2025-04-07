/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_OPENCV_DEVICE_OPENCVWRITER_H
#define YARP_OPENCV_DEVICE_OPENCVWRITER_H

#include <yarp/os/Property.h>
#include <yarp/dev/IFrameWriterImage.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/Stamp.h>

#include "FakeFrameWriter_ParamsParser.h"

/**
 * @ingroup dev_impl_media
 *
 * @brief `FakeFrameWriter`: An image frame writer for testing purposes
 *
 * Parameters required by this device are shown in class: FakeFrameWriter_ParamsParser
 */
class FakeFrameWriter :
        public yarp::dev::IFrameWriterImage,
        public yarp::dev::DeviceDriver,
        public FakeFrameWriter_ParamsParser
{
public:

    FakeFrameWriter() {}
    virtual ~FakeFrameWriter() {}


    bool open(yarp::os::Searchable & config) override;
    bool close() override;
    bool putImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override;

protected:
    int  m_width;
    int  vheight;
    bool m_isInitialized;
};

#endif // YARP_OPENCV_DEVICE_OPENCVWRITER_H
