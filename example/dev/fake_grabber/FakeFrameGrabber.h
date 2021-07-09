/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Time.h>

#include <yarp/sig/ImageDraw.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameGrabberImage.h>

class FakeFrameGrabber :
        public yarp::dev::IFrameGrabberImage,
        public yarp::dev::DeviceDriver
{
private:
    int w, h;
    int ct;
    yarp::sig::PixelRgb circlePixel;

public:
    FakeFrameGrabber()
    {
        h = w = 0;
        ct = 0;
        circlePixel = yarp::sig::PixelRgb{255, 0, 0};
    }

    bool open(int w, int h)
    {
        this->w = w;
        this->h = h;
        return w > 0 && h > 0;
    }

    bool open(yarp::os::Searchable& config) override
    {
        // extract width and height configuration, if present
        // otherwise use 128x128
        int desiredWidth = config.check("w", yarp::os::Value(128)).asInt32();
        int desiredHeight = config.check("h", yarp::os::Value(128)).asInt32();
        return open(desiredWidth, desiredHeight);
    }

    bool close() override
    {
        return true; // easy
    }

    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override
    {
        yarp::os::Time::delay(0.1); // simulate waiting for hardware to report
        image.resize(w, h);
        image.zero();
        yarp::sig::draw::addCrossHair(image, circlePixel, ct, h / 2, h / 8);
        ct = (ct + 4) % w;
        return true;
    }

    int height() const override
    {
        return h;
    }

    int width() const override
    {
        return w;
    }
};
