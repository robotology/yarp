/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2006 Eric Mislivec
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_OPENCV_GRABBER_DEVICE_OPENCVGRABBER_H
#define YARP_OPENCV_GRABBER_DEVICE_OPENCVGRABBER_H

/*
 * A YARP frame grabber device driver using OpenCV to implement
 * image capture from cameras and AVI files.
 */


#include <yarp/os/Property.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/IPreciselyTimed.h>
#include "OpenCVGrabber_ParamsParser.h"

#include <opencv2/videoio.hpp>

/**
 * @ingroup dev_impl_media
 *
 * @brief `opencv_grabber`: An image frame grabber device using OpenCV to
 * capture images from cameras and AVI files.
 */
class OpenCVGrabber :
        public yarp::dev::IFrameGrabberImage,
        public yarp::dev::DeviceDriver,
        public yarp::dev::IPreciselyTimed,
        public OpenCVGrabber_ParamsParser
{
public:

    /**
     * Create an OpenCV image grabber. This initializes members to
     * reasonable default values, the real initialization is done in
     * open().
     */
    OpenCVGrabber() :
        m_saidSize(false),
        m_saidResize(false),
        fromFile(false),
        m_cap()
    {}

    /** Destroy an OpenCV image grabber. */
    virtual ~OpenCVGrabber() { ; }



    bool open(yarp::os::Searchable & config) override;

    bool close() override;

    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) override;


    /** Get the height of images a grabber produces.
     * @return The image height. */
    inline int height() const override { return m_height; }

    /** Get the width of images a grabber produces.
     * @return The image width. */
    inline int width() const override { return m_width; }

    /**
    * Implements the IPreciselyTimed interface.
    * @return the yarp::os::Stamp of the last image acquired
    */
    inline yarp::os::Stamp getLastInputStamp() override { return m_laststamp; }

protected:

    bool m_saidSize = false;
    bool m_saidResize = false;

    /** Whether reading from file or camera. */
    bool fromFile = false;

    /** OpenCV image capture object. */
    cv::VideoCapture m_cap;

    /** Saved copy of the device configuration. */
    yarp::os::Property m_config;

    yarp::os::Stamp m_laststamp;
};

#endif // YARP_OPENCV_GRABBER_DEVICE_OPENCVGRABBER_H
