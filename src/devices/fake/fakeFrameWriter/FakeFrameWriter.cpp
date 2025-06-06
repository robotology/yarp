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


bool FakeFrameWriter::open(Searchable& config)
{
    if (!parseParams(config)) {
        return false;
    }

    if (m_filename == "") {
        yCError(FAKEIMAGEWRITER) << "Invalid filename null";
        return false;
    }

    if (m_framerate == 0) {
        yCError(FAKEIMAGEWRITER) << "Invalid framerate 0";
        return false;
    }

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

yarp::dev::ReturnValue FakeFrameWriter::putImage(ImageOf<PixelRgb>& image)
{
    if (image.width() == 0 || image.height() == 0) {
        yCError(FAKEIMAGEWRITER) << "Received empty frame";
        return ReturnValue::return_code::return_value_error_method_failed;
    }

    if (m_width == 0)  {m_width = image.width();}
    if (m_height == 0) {m_height = image.height();}

    if (!m_isInitialized)
    {
        m_isInitialized = true;
    }

    if (m_width != image.width())
    {
        yCError(FAKEIMAGEWRITER) << "Received frame has a width different from the current configuration" << m_width << "<<" << image.width();
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    if (m_height != image.height()) {
        yCError(FAKEIMAGEWRITER) << "Received frame has a height different from the current configuration: " << m_height << "<<" << image.height();
        return ReturnValue::return_code::return_value_error_method_failed;
    }

    yCDebug(FAKEIMAGEWRITER) << "Frame received:" << image.width() << image.height();

    return ReturnValue_ok;
}
