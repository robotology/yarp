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

#include <opencv2/highgui/highgui.hpp>

/**
 * @ingroup dev_impl_media
 *
 * @brief `opencv_grabber`: An image frame grabber device using OpenCV to
 * capture images from cameras and AVI files.
 */
class OpenCVGrabber :
        public yarp::dev::IFrameGrabberImage,
        public yarp::dev::DeviceDriver,
        public yarp::dev::IPreciselyTimed
{
public:

    /**
     * Create an OpenCV image grabber. This initializes members to
     * reasonable default values, the real initialization is done in
     * open().
     */
    OpenCVGrabber() :
        m_w(0),
        m_h(0),
        m_loop(false),
        m_saidSize(false),
        m_saidResize(false),
        fromFile(false),
        m_cap(),
        m_transpose(false),
        m_flip_x(false),
        m_flip_y(false)
    {}

    /** Destroy an OpenCV image grabber. */
    virtual ~OpenCVGrabber() { ; }



    bool open(yarp::os::Searchable & config) override;

    bool close() override;

    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) override;
    virtual bool sendImage(const cv::Mat & frame, yarp::sig::ImageOf<yarp::sig::PixelRgb> & image);


    /** Get the height of images a grabber produces.
     * @return The image height. */
    inline int height() const override { return m_h; }

    /** Get the width of images a grabber produces.
     * @return The image width. */
    inline int width() const override { return m_w; }

    /**
    * Implements the IPreciselyTimed interface.
    * @return the yarp::os::Stamp of the last image acquired
    */
    inline yarp::os::Stamp getLastInputStamp() override { return m_laststamp; }

protected:

    /** Width of the images a grabber produces. */
    size_t m_w;
    /** Height of the images a grabber produces. */
    size_t m_h;

    /** Whether to loop or not. */
    bool m_loop;

    bool m_saidSize;
    bool m_saidResize;

    /** Whether reading from file or camera. */
    bool fromFile;

    /** OpenCV image capture object. */
    cv::VideoCapture m_cap;

    /* optional image modifiers */
    bool m_transpose;
    bool m_flip_x;
    bool m_flip_y;

    /** Saved copy of the device configuration. */
    yarp::os::Property m_config;

    yarp::os::Stamp m_laststamp;
};

#endif // YARP_OPENCV_GRABBER_DEVICE_OPENCVGRABBER_H
