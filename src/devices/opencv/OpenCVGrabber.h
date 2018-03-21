/*
 * Copyright (C) 2006  Eric Mislivec and RobotCub Consortium
 * Authors: Eric Mislivec and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 *
 */


#ifndef YARP_OPENCV_GRABBER_DEVICE_OPENCVGRABBER_H
#define YARP_OPENCV_GRABBER_DEVICE_OPENCVGRABBER_H

/*
 * A Yarp 2 frame grabber device driver using OpenCV to implement
 * image capture from cameras and AVI files.
 *
 * written by Eric Mislivec
 *
 * edited by paulfitz
 *
 */

namespace yarp {
    namespace dev {
        class OpenCVGrabber;
    }
}

#include <yarp/os/Property.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/PreciselyTimed.h>

#include <opencv2/highgui/highgui.hpp>

#include <OpenCVGrabber.h>

/**
 * @ingroup dev_impl_media
 *
 * An image frame grabber device using OpenCV to capture images from
 * cameras and AVI files.
 * @author Eric Mislivec
 */
class yarp::dev::OpenCVGrabber : public IFrameGrabberImage, public DeviceDriver, public IPreciselyTimed
{
public:

    /**
     * Create an OpenCV image grabber. This initializes members to
     * reasonable default values, the real initialization is done in
     * open().
     *
     * \author Eric Mislevic
     *
     */
    OpenCVGrabber() : IFrameGrabberImage(), DeviceDriver(),
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



    virtual bool open(yarp::os::Searchable & config) override;

    virtual bool close() override;

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) override;
    virtual bool sendImage(const cv::Mat & frame, yarp::sig::ImageOf<yarp::sig::PixelRgb> & image);


    /** Get the height of images a grabber produces.
     * @return The image height. */
    inline virtual int height() const override { return m_h; }

    /** Get the width of images a grabber produces.
     * @return The image width. */
    inline virtual int width() const override { return m_w; }

    /**
    * Implements the IPreciselyTimed interface.
    * @return the yarp::os::Stamp of the last image acquired
    */
    inline virtual yarp::os::Stamp getLastInputStamp() override { return m_laststamp; }

protected:

    /** Width of the images a grabber produces. */
    int m_w;
    /** Height of the images a grabber produces. */
    int m_h;

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


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_opencv_grabber opencv_grabber

 A wrapper for the opencv library's image sources, see yarp::dev::OpenCVGrabber.

*/


#endif // YARP_OPENCV_GRABBER_DEVICE_OPENCVGRABBER_H
