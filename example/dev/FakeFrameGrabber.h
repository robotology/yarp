/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

class FakeFrameGrabber : public yarp::dev::IFrameGrabberImage,
                         public yarp::dev::DeviceDriver {
private:
    int w, h;
    int ct;
    yarp::sig::PixelRgb circlePixel;
public:
    FakeFrameGrabber() {
        h = w = 0;
        ct = 0;
        circlePixel = yarp::sig::PixelRgb(255,0,0);
    }

    bool open(int w, int h) {
        this->w = w;
        this->h = h;
        return w>0 && h>0;
    }

    virtual bool open(yarp::os::Searchable& config) {
        // extract width and height configuration, if present
        // otherwise use 128x128
        int desiredWidth = config.check("w",yarp::os::Value(128)).asInt32();
        int desiredHeight = config.check("h",yarp::os::Value(128)).asInt32();
        return open(desiredWidth,desiredHeight);
    }

    virtual bool close() {
        return true; // easy
    }

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
        yarp::os::Time::delay(0.1);  // simulate waiting for hardware to report
        image.resize(w,h);
        image.zero();
        yarp::sig::draw::addCrossHair(image,circlePixel,
                                      ct,h/2,h/8);
        ct = (ct+4)%w;
        return true;
    }

    virtual int height() const {
        return h;
    }

    virtual int width() const {
        return w;
    }
};
