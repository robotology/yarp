/*
 * Copyright (C) 2006 Eric Mislivec and RobotCub Consortium
 * Authors: Eric Mislivec and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


/*
 * A Yarp 2 frame grabber device driver using OpenCV to implement
 * image capture from cameras and AVI files.
 *
 * Eric Mislivec
 */


// This define prevents Yarp from declaring its own copy of IplImage
// which OpenCV provides as well. Since Yarp's Image class depends on
// IplImage, we need to define this, then include the OpenCV headers
// before Yarp's.
#define YARP_CVTYPES_H_

#include <yarp/dev/Drivers.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/os/ConstString.h>
#include <yarp/os/Property.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Value.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/Image.h>

#include <stdio.h>

#ifdef YARP2_WINDOWS
#include <cv.h>
#include <highgui.h>
#else
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#include <OpenCVGrabber.h>


using yarp::dev::DeviceDriver;
using yarp::dev::DriverCreatorOf;
using yarp::dev::Drivers;
using yarp::dev::IFrameGrabberImage;
using yarp::dev::PolyDriver;

using yarp::os::ConstString;
using yarp::os::Property;
using yarp::os::Searchable;
using yarp::os::Value;

using yarp::sig::ImageOf;
using yarp::sig::PixelRgb;

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


#define DBG if (0)

#ifndef CV_CAP_ANY
#define CV_CAP_ANY (-1)
#endif

bool OpenCVGrabber::open(Searchable & config) {
    // Release any previously allocated resources, just in case
    close();

    m_saidSize = false;
    m_saidResize = false;
    m_transpose = false;
    m_flip_x = false;
    m_flip_y = false;

    // Are we capturing from a file or a camera ?
    ConstString file = config.check("movie", Value(""),
                                    "if present, read from specified file rather than camera").asString();
    fromFile = (file!="");
    if (fromFile) {

        // Try to open a capture object for the file
        m_capture = (void*)cvCaptureFromAVI(file.c_str());
        if (0 == m_capture) {
            yError("Unable to open file '%s' for capture!", file.c_str());
            return false;
        }

        // Should we loop?
        m_loop = config.check("loop","if present, loop movie");

    } else {

        m_loop = false;

        int camera_idx = 
            config.check("camera", 
                         Value(CV_CAP_ANY), 
                         "if present, read from camera identified by this index").asInt();

        if (camera_idx == -1)
        {
            yError() << "You are using an old, deprcated C-API cvCaptureFromCAM. Code should be fixed to use opencv VideoCapture class. In the meanwhile you have to relaunch the grabber with option --camera <id_of_the_camera>";
        }

        // Try to open a capture object for the first camera
        m_capture = (void*)cvCaptureFromCAM(camera_idx);
        if (0 == m_capture) {
            yError("Unable to open camera for capture!");
            return false;
        }
        else
        {
            yInfo("Capturing from camera: %d",camera_idx);
        }

        if ( config.check("framerate","if present, specifies desired camera device framerate") ) {
            double m_fps = config.check("framerate", Value(-1)).asDouble();
            cvSetCaptureProperty((CvCapture*)m_capture,
                                 CV_CAP_PROP_FPS,m_fps);
        }

        if (config.check("flip_x", "if present, flip the image along the x-axis"))         m_flip_x = true;
        if (config.check("flip_y", "if present, flip the image along the y-axis"))         m_flip_y = true;
        if (config.check("transpose", "if present, rotate the image along of 90 degrees")) m_transpose = true;
    }


    // Extract the desired image size from the configuration if
    // present, otherwise query the capture device
    if (config.check("width","if present, specifies desired image width")) {
        m_w = config.check("width", Value(-1)).asInt();
        if (!fromFile && m_w>0) {
            cvSetCaptureProperty((CvCapture*)m_capture,
                                 CV_CAP_PROP_FRAME_WIDTH, m_w);
        }
    } else {
        m_w = (int)cvGetCaptureProperty((CvCapture*)m_capture,
                                        CV_CAP_PROP_FRAME_WIDTH);
    }

    if (config.check("height","if present, specifies desired image height")) {
        m_h = config.check("height", Value(-1)).asInt();
        if (!fromFile && m_h>0) {
            cvSetCaptureProperty((CvCapture*)m_capture,
                                 CV_CAP_PROP_FRAME_HEIGHT, m_h);
        }
    } else {
        m_h = (int)cvGetCaptureProperty((CvCapture*)m_capture,
                                        CV_CAP_PROP_FRAME_HEIGHT);
    }


    // Ignore capture properties - they are unreliable
    // yDebug("Capture properties: %ld x %ld pixels @ %lf frames/sec.", m_w, m_h, cvGetCaptureProperty(m_capture, CV_CAP_PROP_FPS));

    yInfo("OpenCVGrabber opened");
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
    // Release the capture object, the pointer should be set null
    if (0 != m_capture) cvReleaseCapture((CvCapture**)(&m_capture));
    if (0 != m_capture) {
        m_capture = 0; return false;
    } else return true;
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
bool OpenCVGrabber::sendImage(IplImage* iplFrame, ImageOf<PixelRgb> & image)
{
    // Resize the output image, this should not result in new
    // memory allocation if the image is already the correct size
    image.resize(iplFrame->width, iplFrame->height);

    if (!m_saidSize) {
        yDebug("Received image of size %dx%d\n", image.width(), image.height());
        m_saidSize = true;
    }

    // Get an IplImage, the Yarp Image owns the memory pointed to
    IplImage * iplImage = (IplImage*)image.getIplImage();
    // create the timestamp
    m_laststamp.update();

    // Copy the captured image to the output image, flipping it if
    // the coordinate origin is not the top left
    if (IPL_ORIGIN_TL == iplFrame->origin)
        cvCopy(iplFrame, iplImage, 0);
    else
        cvFlip(iplFrame, iplImage, 0);

    if (iplFrame->channelSeq[0] == 'B') {
        cvCvtColor(iplImage, iplImage, CV_BGR2RGB);
    }

    if (m_w <= 0) {
        m_w = image.width();
    }
    if (m_h <= 0) {
        m_h = image.height();
    }
    if (fromFile) {
        if (m_w>0 && m_h>0) {
            if (image.width() != m_w || image.height() != m_h) {
                if (!m_saidResize) {
                    yDebug("Software scaling from %dx%d to %dx%d",  image.width(), image.height(), m_w, m_h);
                    m_saidResize = true;
                }
                image.copy(image, m_w, m_h);
            }
        }
    }

    DBG yDebug("%d by %d %s image\n", image.width(), image.height(), iplFrame->channelSeq);

    return true;

}

bool OpenCVGrabber::getImage(ImageOf<PixelRgb> & image) {

    //yTrace("-->getImage123");

    // Must have a capture object
    if (0 == m_capture) {
        image.zero(); return false;
    }

    //yTrace("-->HERE1");
    // Grab and retrieve a frame, OpenCV owns the returned image
    IplImage * iplFrame = cvQueryFrame((CvCapture*)m_capture);
    //yTrace("-->HERE2");

    if (0 == iplFrame && m_loop) {
        bool ok = open(m_config);
        if (!ok) return false;
        iplFrame = cvQueryFrame((CvCapture*)m_capture);
    }

    if (0 == iplFrame) {
        image.zero(); return false;
    }
    
    if (m_transpose == false && m_flip_x == false && m_flip_y == false)
    {
        return sendImage(iplFrame, image);
    }

    IplImage * iplFrame_out;
    if (m_transpose)
    {
        iplFrame_out = cvCreateImage(cvSize(iplFrame->height, iplFrame->width), iplFrame->depth, iplFrame->nChannels);
        cvTranspose(iplFrame, iplFrame_out);
    }
    else
    {
        iplFrame_out = cvCreateImage(cvSize(iplFrame->width, iplFrame->height), iplFrame->depth, iplFrame->nChannels);
        cvCopy(iplFrame, iplFrame_out);
    }

    if (m_flip_x && m_flip_y)
    {
        cvFlip(iplFrame_out, 0, -1);
    }
    else
    {
        if (m_flip_x)
        {
            cvFlip(iplFrame_out, 0, 0);
        }
        else if (m_flip_y)
        {
            cvFlip(iplFrame_out, 0, 1);
        }
    }
    bool ret = sendImage(iplFrame_out, image);
    cvReleaseImage(&iplFrame_out);

    return ret;
}


// End: OpenCVGrabber.cpp
