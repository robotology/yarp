/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "OpenCVWriter.h"

#include <string>
#include <yarp/os/Property.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Value.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/Image.h>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>


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
YARP_LOG_COMPONENT(OPENCVWRITER, "yarp.device.opencv_writer")
}


bool OpenCVWriter::open(Searchable & config)
{
    if (!parseParams(config))
    {
        return false;
    }

    m_writer.open(m_filename, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), m_framerate, cv::Size(m_width, m_height));
    if (!m_writer.isOpened())
    {
        yCError(OPENCVWRITER, "Unable to open outputfile");
        return false;
    }
    m_isInitialized = true;
    return true;

    yCInfo(OPENCVWRITER, "OpenCVGrabber opened");
    return true;
}

bool OpenCVWriter::close()
{
    if (m_isInitialized)
    {
        m_writer.release();
    }
    yCInfo(OPENCVWRITER, "OpenCVGrabber closed");
    return true;
}

bool OpenCVWriter::putImage(ImageOf<PixelRgb> & image)
{
    if (m_width == 0)  {m_width = image.width();}
    if (m_height == 0) {m_height = image.height();}

    cv::Mat frame(m_height, m_width, CV_8UC3, (void*)image.getRawImage());
        cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);

    if (frame.empty())
    {
        yCError(OPENCVWRITER) << "Received empty frame";
        return false;
    }

    if (frame.cols != m_width || frame.rows != m_height)
    {
        yCError(OPENCVWRITER) << "Invalid frame size";
        return false;
    }

    m_writer.write(frame);

    return true;
}
