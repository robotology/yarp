// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006  Eric Mislivec and RobotCub Consortium
 * Authors:  Eric Mislivec and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef OpenCVGrabber_INC
#define OpenCVGrabber_INC

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

/**
 * @ingroup dev_impl_media
 *
 * An image frame grabber device using OpenCV to capture images from
 * cameras and AVI files.
 * @author Eric Mislivec
 */
class yarp::dev::OpenCVGrabber : public IFrameGrabberImage, public DeviceDriver
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
                      m_w(0), m_h(0), m_capture(0) { ; }

    /** Destroy an OpenCV image grabber. */
    virtual ~OpenCVGrabber() { ; }



    virtual bool open(yarp::os::Searchable & config);

    virtual bool close();

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image);


    /** Get the height of images a grabber produces.
     * @return The image height. */
    inline virtual int height() const { return m_h; }

    /** Get the width of images a grabber produces.
     * @return The image width. */
    inline virtual int width() const { return m_w; }


protected:

    /** Width of the images a grabber produces. */
    int m_w;
    /** Height of the images a grabber produces. */
    int m_h;

    /** Whether to loop or not */
    bool m_loop;

    bool m_saidSize;
    bool m_saidResize;

    /* reading from file or camera */
    bool fromFile;

    /** Opaque OpenCV structure for image capture. */
    void * m_capture;

    /** Saved copy of the device configuration */
    yarp::os::Property m_config;
};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_opencv_grabber opencv_grabber

 A wrapper for the opencv library's image sources, see yarp::dev::OpenCVGrabber.

*/


#endif

