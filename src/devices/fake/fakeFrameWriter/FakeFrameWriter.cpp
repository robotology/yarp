/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FakeFrameWriter.h"

#include <string>

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/sig/Image.h>

using yarp::dev::DeviceDriver;
using yarp::dev::IFrameWriterImage;

using yarp::os::Property;
using yarp::os::Searchable;
using yarp::os::Value;

using yarp::sig::ImageOf;
using yarp::sig::PixelRgb;

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


namespace {
YARP_LOG_COMPONENT(FAKEIMAGEWRITER, "yarp.device.fakeImageWriter")
}


bool FakeFrameWriter::open(Searchable & config)
{
    if (!parseParams(config))
    {
        return false;
    }

    m_isInitialized = true;

    yCInfo(FAKEIMAGEWRITER, "FakeFrameWriter opened");
    return true;
}

bool FakeFrameWriter::close()
{
    if (m_isInitialized)
    {
        m_isInitialized = false;
    }
    return true;
}

bool FakeFrameWriter::putImage(ImageOf<PixelRgb> & image)
{
    yCDebug(FAKEIMAGEWRITER) << "Frame received:" << image.width() << image.height();
    return true;
}
