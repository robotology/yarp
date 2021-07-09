/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2006 Eric Mislivec
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

/*
 * A YARP frame grabber device driver using OpenCV to implement
 * image capture from cameras and AVI files.
 */

#include "OpenCVGrabber.h"

#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

#include <string>
#include <yarp/os/Property.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Value.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/Image.h>

#include <cstring> // memcpy

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>


using yarp::dev::DeviceDriver;
using yarp::dev::DriverCreatorOf;
using yarp::dev::Drivers;
using yarp::dev::IFrameGrabberImage;
using yarp::dev::PolyDriver;

using yarp::os::Property;
using yarp::os::Searchable;
using yarp::os::Value;

using yarp::sig::ImageOf;
using yarp::sig::PixelRgb;

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


namespace {
YARP_LOG_COMPONENT(OPENCVGRABBER, "yarp.device.opencv_grabber")
}


bool OpenCVGrabber::open(Searchable & config) {
    m_saidSize = false;
    m_saidResize = false;
    m_transpose = false;
    m_flip_x = false;
    m_flip_y = false;

    // Are we capturing from a file or a camera ?
    std::string file = config.check("movie", Value(""),
                                    "if present, read from specified file rather than camera").asString();
    fromFile = (file!="");
    if (fromFile) {

        // Try to open a capture object for the file
        m_cap.open(file.c_str());
        if (!m_cap.isOpened()) {
            yCError(OPENCVGRABBER, "Unable to open file '%s' for capture!", file.c_str());
            return false;
        }

        // Should we loop?
        m_loop = config.check("loop","if present, loop movie");

    } else {

        m_loop = false;
        int camera_idx =
            config.check("camera",
                         Value(cv::VideoCaptureAPIs::CAP_ANY),
                         "if present, read from camera identified by this index").asInt32();
        // Try to open a capture object for the first camera
        m_cap.open(camera_idx);
        if (!m_cap.isOpened()) {
            yCError(OPENCVGRABBER, "Unable to open camera for capture!");
            return false;
        }
        else
        {
            yCInfo(OPENCVGRABBER, "Capturing from camera: %d",camera_idx);
        }

        if ( config.check("framerate","if present, specifies desired camera device framerate") ) {
            double m_fps = config.check("framerate", Value(-1)).asFloat64();
            m_cap.set(cv::VideoCaptureProperties::CAP_PROP_FPS, m_fps);
        }

        if (config.check("flip_x", "if present, flip the image along the x-axis"))         m_flip_x = true;
        if (config.check("flip_y", "if present, flip the image along the y-axis"))         m_flip_y = true;
        if (config.check("transpose", "if present, rotate the image along of 90 degrees")) m_transpose = true;
    }


    // Extract the desired image size from the configuration if
    // present, otherwise query the capture device
    if (config.check("width","if present, specifies desired image width")) {
        m_w = config.check("width", Value(0)).asInt32();
        if (!fromFile && m_w>0) {
            m_cap.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH, m_w);
        }
    } else {
        m_w = (size_t)m_cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH);
    }

    if (config.check("height","if present, specifies desired image height")) {
        m_h = config.check("height", Value(0)).asInt32();
        if (!fromFile && m_h>0) {
            m_cap.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, m_h);
        }
    } else {
        m_h = (size_t)m_cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT);
    }

    // Ignore capture properties - they are unreliable

    yCInfo(OPENCVGRABBER, "OpenCVGrabber opened");
    // Success!

    // save our configuration for future reference
    m_config.fromString(config.toString());

    return true;

}


/**
 * Close a grabber. This is called by yarp to free any allocated
 * hardware or software resources when the driver instance is no
 * longer needed.
 *
 * @return True if the device was successfully closed. In any case
 * the device will be unusable after this function is called.
 */
bool OpenCVGrabber::close() {
    // Resources will be automatically deinitialized in VideoCapture
    // destructor
    return true;
}


/**
 * Read an image from the grabber.
 *
 * @param image The image to read. The image will be resized to
 * the dimensions the grabber is using, and the captured image
 * data will be written to it.
 *
 * @return True if an image was successfully captured. If false
 * returned, the image will be resized to the dimensions used by
 * the grabber, but all pixels will be zeroed.
 */
bool OpenCVGrabber::sendImage(const cv::Mat & frame, ImageOf<PixelRgb> & image)
{
    // Resize the output image, this should not result in new
    // memory allocation if the image is already the correct size
    image.resize(frame.cols, frame.rows);

    if (!m_saidSize) {
        yCDebug(OPENCVGRABBER, "Received image of size %zux%zu", image.width(), image.height());
        m_saidSize = true;
    }

    // create the timestamp
    m_laststamp.update();

    // Convert to RGB color space
    cv::Mat frame_rgb;
    cv::cvtColor(frame, frame_rgb, cv::COLOR_BGR2RGB);

    // Copy the captured image to the output image
    memcpy(image.getRawImage(), frame_rgb.data, sizeof(unsigned char) * frame_rgb.rows * frame_rgb.cols * frame_rgb.channels());

    if (m_w == 0) {
        m_w = image.width();
    }
    if (m_h == 0) {
        m_h = image.height();
    }
    if (fromFile) {
        if (m_w>0 && m_h>0) {
            if (image.width() != m_w || image.height() != m_h) {
                if (!m_saidResize) {
                    yCDebug(OPENCVGRABBER, "Software scaling from %zux%zu to %zux%zu",  image.width(), image.height(), m_w, m_h);
                    m_saidResize = true;
                }
                image.copy(image, m_w, m_h);
            }
        }
    }

    yCTrace(OPENCVGRABBER, "%zu by %zu image", image.width(), image.height());

    return true;

}

bool OpenCVGrabber::getImage(ImageOf<PixelRgb> & image) {

    // Must have a capture object
    if (!m_cap.isOpened()) {
        image.zero();
        return false;
    }

    // Grab and retrieve a frame
    cv::Mat frame;
    m_cap.read(frame);

    if (frame.empty() && m_loop) {
        bool ok = open(m_config);
        if (!ok) return false;
        m_cap.read(frame);
    }

    if (frame.empty()) {
        image.zero();
        return false;
    }

    if (m_transpose)
    {
        cv::transpose(frame, frame);
    }

    if (m_flip_x && m_flip_y)
    {
        cv::flip(frame, frame, -1);
    }
    else if (m_flip_x)
    {
        cv::flip(frame, frame, 0);
    }
    else if (m_flip_y)
    {
        cv::flip(frame, frame, 1);
    }

    return sendImage(frame, image);
}


// End: OpenCVGrabber.cpp
